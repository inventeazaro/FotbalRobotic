/*
Copyright (c) 2009-2012 Roger Light <roger@atchoo.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of mosquitto nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <config.h>

#include <mosquitto_broker.h>
#include <memory_mosq.h>
#include <mosquitto.h>
#include <send_mosq.h>
#include <util_mosq.h>

static int max_inflight = 20;
static int max_queued = 100;

uint64_t g_bytes_received = 0;
uint64_t g_bytes_sent = 0;
uint64_t g_pub_bytes_received = 0;
uint64_t g_pub_bytes_sent = 0;
unsigned long g_msgs_received = 0;
unsigned long g_msgs_sent = 0;
unsigned long g_pub_msgs_received = 0;
unsigned long g_pub_msgs_sent = 0;

int mqtt3_db_open(mqtt3_config *config, mosquitto_db *db)
{
	int rc = 0;
	struct _mosquitto_subhier *child;

	if(!config || !db) return MOSQ_ERR_INVAL;

	db->last_db_id = 0;

	db->context_count = 1;
	db->contexts = _mosquitto_malloc(sizeof(struct mosquitto*)*db->context_count);
	if(!db->contexts) return MOSQ_ERR_NOMEM;
	db->contexts[0] = NULL;

	db->subs.next = NULL;
	db->subs.subs = NULL;
	db->subs.topic = "";

	child = _mosquitto_malloc(sizeof(struct _mosquitto_subhier));
	if(!child){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	child->next = NULL;
	child->topic = _mosquitto_strdup("");
	if(!child->topic){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	child->subs = NULL;
	child->children = NULL;
	child->retained = NULL;
	db->subs.children = child;

	child = _mosquitto_malloc(sizeof(struct _mosquitto_subhier));
	if(!child){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	child->next = NULL;
	child->topic = _mosquitto_strdup("$SYS");
	if(!child->topic){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	child->subs = NULL;
	child->children = NULL;
	child->retained = NULL;
	db->subs.children->next = child;

	db->unpwd = NULL;

#ifdef WITH_PERSISTENCE
	if(config->persistence && config->persistence_filepath){
		if(mqtt3_db_restore(db)) return 1;
	}
#endif

	return rc;
}

static void subhier_clean(struct _mosquitto_subhier *subhier)
{
	struct _mosquitto_subhier *next;
	struct _mosquitto_subleaf *leaf, *nextleaf;

	while(subhier){
		next = subhier->next;
		leaf = subhier->subs;
		while(leaf){
			nextleaf = leaf->next;
			_mosquitto_free(leaf);
			leaf = nextleaf;
		}
		if(subhier->retained){
			subhier->retained->ref_count--;
		}
		subhier_clean(subhier->children);
		if(subhier->topic) _mosquitto_free(subhier->topic);

		_mosquitto_free(subhier);
		subhier = next;
	}
}

int mqtt3_db_close(mosquitto_db *db)
{
	subhier_clean(db->subs.children);
	mqtt3_db_store_clean(db);

	return MOSQ_ERR_SUCCESS;
}

/* Returns the number of client currently in the database.
 * This includes inactive clients.
 * Returns 1 on failure (count is NULL)
 * Returns 0 on success.
 */
int mqtt3_db_client_count(mosquitto_db *db, int *count, int *inactive_count)
{
	int i;

	if(!db || !count || !inactive_count) return MOSQ_ERR_INVAL;

	*count = 0;
	*inactive_count = 0;
	for(i=0; i<db->context_count; i++){
		if(db->contexts[i]){
			(*count)++;
			if(db->contexts[i]->sock == INVALID_SOCKET){
				(*inactive_count)++;
			}
		}
	}

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_message_delete(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_direction dir)
{
	mosquitto_client_msg *tail, *last = NULL;
	int msg_index = 0;
	bool deleted = false;

	if(!context) return MOSQ_ERR_INVAL;

	tail = context->msgs;
	while(tail){
		msg_index++;
		if(tail->state == ms_queued && msg_index <= max_inflight){
			tail->timestamp = time(NULL);
			if(tail->direction == mosq_md_out){
				switch(tail->qos){
					case 0:
						tail->state = ms_publish_qos0;
						break;
					case 1:
						tail->state = ms_publish_qos1;
						break;
					case 2:
						tail->state = ms_publish_qos2;
						break;
				}
			}else{
				if(tail->qos == 2){
					tail->state = ms_wait_for_pubrel;
				}
			}
		}
		if(tail->mid == mid && tail->direction == dir){
			msg_index--;
			/* FIXME - it would be nice to be able to remove the stored message here if ref_count==0 */
			tail->store->ref_count--;
			if(last){
				last->next = tail->next;
			}else{
				context->msgs = tail->next;
			}
			_mosquitto_free(tail);
			if(last){
				tail = last->next;
			}else{
				tail = context->msgs;
			}
			deleted = true;
		}else{
			last = tail;
			tail = tail->next;
		}
		if(msg_index > max_inflight && deleted){
			return MOSQ_ERR_SUCCESS;
		}
	}

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_message_insert(mosquitto_db *db, struct mosquitto *context, uint16_t mid, enum mosquitto_msg_direction dir, int qos, bool retain, struct mosquitto_msg_store *stored)
{
	mosquitto_client_msg *msg, *tail = NULL;
	enum mqtt3_msg_state state = ms_invalid;
	int msg_count = 0;
	int rc = 0;
	int i;
	char **dest_ids;

	assert(stored);
	if(!context) return MOSQ_ERR_INVAL;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	if(dir == mosq_md_out && retain == false && stored->dest_ids){
		for(i=0; i<stored->dest_id_count; i++){
			if(!strcmp(stored->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				return MOSQ_ERR_SUCCESS;
			}
		}
	}
	if(context->sock == INVALID_SOCKET){
		/* Client is not connected only queue messages with QoS>0. */
		if(qos == 0){
			if(!context->bridge){
				return 2;
			}else{
				if(context->bridge->start_type != bst_lazy){
					return 2;
				}
			}
		}
	}
	if(context->msgs){
		tail = context->msgs;
		msg_count = 1;
		while(tail && tail->next){
			if(tail->qos > 0){
				msg_count++;
			}
			tail = tail->next;
		}
	}

	if(context->sock != INVALID_SOCKET){
		if( (qos == 0 && !db->config->queue_qos0_messages) || max_inflight == 0 || msg_count < max_inflight){
			if(dir == mosq_md_out){
				switch(qos){
					case 0:
						state = ms_publish_qos0;
						break;
					case 1:
						state = ms_publish_qos1;
						break;
					case 2:
						state = ms_publish_qos2;
						break;
				}
			}else{
				if(qos == 2){
					state = ms_wait_for_pubrel;
				}else{
					return 1;
				}
			}
		}else if(max_queued == 0 || msg_count-max_inflight < max_queued){
			state = ms_queued;
			rc = 2;
		}else{
			/* Dropping message due to full queue.
		 	* FIXME - should this be logged? */
			return 2;
		}
	}else{
		if(msg_count >= max_queued){
			return 2;
		}else{
			state = ms_queued;
		}
	}
	assert(state != ms_invalid);

#ifdef WITH_PERSISTENCE
	if(state == ms_queued){
		db->persistence_changes++;
	}
#endif

	msg = _mosquitto_malloc(sizeof(mosquitto_client_msg));
	if(!msg) return MOSQ_ERR_NOMEM;
	msg->next = NULL;
	msg->store = stored;
	msg->store->ref_count++;
	msg->mid = mid;
	msg->timestamp = time(NULL);
	msg->direction = dir;
	msg->state = state;
	msg->dup = false;
	msg->qos = qos;
	msg->retain = retain;
	if(tail){
		tail->next = msg;
	}else{
		context->msgs = msg;
	}

	if(dir == mosq_md_out && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = _mosquitto_realloc(stored->dest_ids, sizeof(char *)*(stored->dest_id_count+1));
		if(dest_ids){
			stored->dest_ids = dest_ids;
			stored->dest_id_count++;
			stored->dest_ids[stored->dest_id_count-1] = _mosquitto_strdup(context->id);
			if(!stored->dest_ids[stored->dest_id_count-1]){
				return MOSQ_ERR_NOMEM;
			}
		}else{
			return MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	msg_count++; /* We've just added a message to the list */
	if(context->bridge && context->bridge->start_type == bst_lazy
			&& context->sock == INVALID_SOCKET
			&& msg_count >= context->bridge->threshold){

		context->state = mosq_cs_new;
		mqtt3_bridge_connect(db, context);
	}
#endif

	return rc;
}

int mqtt3_db_message_update(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_direction dir, enum mqtt3_msg_state state)
{
	mosquitto_client_msg *tail;

	tail = context->msgs;
	while(tail){
		if(tail->mid == mid && tail->direction == dir){
			tail->state = state;
			tail->timestamp = time(NULL);
			return MOSQ_ERR_SUCCESS;
		}
		tail = tail->next;
	}
	return 1;
}

int mqtt3_db_messages_delete(struct mosquitto *context)
{
	mosquitto_client_msg *tail, *next;

	if(!context) return MOSQ_ERR_INVAL;

	tail = context->msgs;
	while(tail){
		/* FIXME - it would be nice to be able to remove the stored message here if rec_count==0 */
		tail->store->ref_count--;
		next = tail->next;
		_mosquitto_free(tail);
		tail = next;
	}
	context->msgs = NULL;

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_messages_easy_queue(mosquitto_db *db, struct mosquitto *context, const char *topic, int qos, uint32_t payloadlen, const void *payload, int retain)
{
	struct mosquitto_msg_store *stored;
	char *source_id;

	assert(db);

	if(!topic) return MOSQ_ERR_INVAL;

	if(context){
		source_id = context->id;
	}else{
		source_id = "";
	}
	if(mqtt3_db_message_store(db, source_id, 0, topic, qos, payloadlen, payload, retain, &stored, 0)) return 1;

	return mqtt3_db_messages_queue(db, source_id, topic, qos, retain, stored);
}

int mqtt3_db_message_store(mosquitto_db *db, const char *source, uint16_t source_mid, const char *topic, int qos, uint32_t payloadlen, const void *payload, int retain, struct mosquitto_msg_store **stored, dbid_t store_id)
{
	struct mosquitto_msg_store *temp;

	assert(db);
	assert(stored);

	if(!topic) return MOSQ_ERR_INVAL;

	temp = _mosquitto_malloc(sizeof(struct mosquitto_msg_store));
	if(!temp) return MOSQ_ERR_NOMEM;

	temp->next = db->msg_store;
	temp->ref_count = 0;
	if(source){
		temp->source_id = _mosquitto_strdup(source);
	}else{
		temp->source_id = _mosquitto_strdup("");
	}
	if(!temp->source_id){
		_mosquitto_free(temp);
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	temp->source_mid = source_mid;
	temp->msg.mid = 0;
	temp->msg.qos = qos;
	temp->msg.retain = retain;
	temp->msg.topic = _mosquitto_strdup(topic);
	if(!temp->msg.topic){
		_mosquitto_free(temp->source_id);
		_mosquitto_free(temp);
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		return MOSQ_ERR_NOMEM;
	}
	temp->msg.payloadlen = payloadlen;
	if(payloadlen){
		temp->msg.payload = _mosquitto_malloc(sizeof(char)*payloadlen);
		if(!temp->msg.payload){
			if(temp->source_id) _mosquitto_free(temp->source_id);
			if(temp->msg.topic) _mosquitto_free(temp->msg.topic);
			if(temp->msg.payload) _mosquitto_free(temp->msg.payload);
			_mosquitto_free(temp);
			return MOSQ_ERR_NOMEM;
		}
		memcpy(temp->msg.payload, payload, sizeof(char)*payloadlen);
	}else{
		temp->msg.payload = NULL;
	}

	if(!temp->source_id || !temp->msg.topic || (payloadlen && !temp->msg.payload)){
		if(temp->source_id) _mosquitto_free(temp->source_id);
		if(temp->msg.topic) _mosquitto_free(temp->msg.topic);
		if(temp->msg.payload) _mosquitto_free(temp->msg.payload);
		_mosquitto_free(temp);
		return 1;
	}
	temp->dest_ids = NULL;
	temp->dest_id_count = 0;
	db->msg_store_count++;
	db->msg_store = temp;
	(*stored) = temp;

	if(!store_id){
		temp->db_id = ++db->last_db_id;
	}else{
		temp->db_id = store_id;
	}

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_message_store_find(struct mosquitto *context, uint16_t mid, struct mosquitto_msg_store **stored)
{
	mosquitto_client_msg *tail;

	if(!context) return MOSQ_ERR_INVAL;

	*stored = NULL;
	tail = context->msgs;
	while(tail){
		if(tail->store->source_mid == mid && tail->direction == mosq_md_in){
			*stored = tail->store;
			return MOSQ_ERR_SUCCESS;
		}
		tail = tail->next;
	}

	return 1;
}

/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to clear incoming messages. */
int mqtt3_db_message_reconnect_reset(struct mosquitto *context)
{
	mosquitto_client_msg *msg;
	mosquitto_client_msg *prev = NULL;

	msg = context->msgs;
	while(msg){
		if(msg->direction == mosq_md_out){
			if(msg->state != ms_queued){
				switch(msg->qos){
					case 0:
						msg->state = ms_publish_qos0;
						break;
					case 1:
						msg->state = ms_publish_qos1;
						break;
					case 2:
						msg->state = ms_publish_qos2;
						break;
				}
			}
		}else{
			/* Client must resend any partially completed messages. */
			msg->store->ref_count--;
			if(prev){
				prev->next = msg->next;
				_mosquitto_free(msg);
				msg = prev;
			}else{
				context->msgs = msg->next;
				_mosquitto_free(msg);
				msg = context->msgs;
			}
		}
		prev = msg;
		if(msg) msg = msg->next;
	}
	/* Messages received when the client was disconnected are put
	 * in the ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 * This only sets a single message up to be published, but once
	 * it is sent the full max_inflight amount of messages will be
	 * queued up for sending.
	 */
	if(context->msgs){
		if(context->msgs->state == ms_queued){
			switch(context->msgs->qos){
				case 0:
					context->msgs->state = ms_publish_qos0;
					break;
				case 1:
					context->msgs->state = ms_publish_qos1;
					break;
				case 2:
					context->msgs->state = ms_publish_qos2;
					break;
			}
		}
	}

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_message_timeout_check(mosquitto_db *db, unsigned int timeout)
{
	int i;
	time_t threshold = time(NULL) - timeout;
	enum mqtt3_msg_state new_state = ms_invalid;
	struct mosquitto *context;
	mosquitto_client_msg *msg;

	
	for(i=0; i<db->context_count; i++){
		context = db->contexts[i];
		if(!context) continue;

		msg = context->msgs;
		while(msg){
			if(msg->timestamp < threshold && msg->state != ms_queued){
				switch(msg->state){
					case ms_wait_for_puback:
						new_state = ms_publish_qos1;
						break;
					case ms_wait_for_pubrec:
						new_state = ms_publish_qos2;
						break;
					case ms_wait_for_pubrel:
						new_state = ms_send_pubrec;
						break;
					case ms_wait_for_pubcomp:
						new_state = ms_resend_pubrel;
						break;
					default:
						break;
				}
				if(new_state != ms_invalid){
					msg->timestamp = time(NULL);
					msg->state = new_state;
					msg->dup = true;
				}
			}
			msg = msg->next;
		}
	}

	return MOSQ_ERR_SUCCESS;
}

int mqtt3_db_message_release(mosquitto_db *db, struct mosquitto *context, uint16_t mid, enum mosquitto_msg_direction dir)
{
	mosquitto_client_msg *tail, *last = NULL;
	int qos;
	int retain;
	char *topic;
	char *source_id;

	if(!context) return MOSQ_ERR_INVAL;

	tail = context->msgs;
	while(tail){
		if(tail->mid == mid && tail->direction == dir){
			qos = tail->store->msg.qos;
			topic = tail->store->msg.topic;
			retain = tail->retain;
			source_id = tail->store->source_id;

			if(!mqtt3_db_messages_queue(db, source_id, topic, qos, retain, tail->store)){
				tail->store->ref_count--;
				if(last){
					last->next = tail->next;
				}else{
					context->msgs = tail->next;
				}
				_mosquitto_free(tail);
				return MOSQ_ERR_SUCCESS;
			}else{
				return 1;
			}
		}
		last = tail;
		tail = tail->next;
	}
	return 1;
}

int mqtt3_db_message_write(struct mosquitto *context)
{
	int rc;
	mosquitto_client_msg *tail, *last = NULL;
	uint16_t mid;
	int retries;
	int retain;
	const char *topic;
	int qos;
	uint32_t payloadlen;
	const void *payload;

	if(!context || context->sock == -1
			|| (context->state == mosq_cs_connected && !context->id)){
		return MOSQ_ERR_INVAL;
	}

	tail = context->msgs;
	while(tail){
		if(tail->state != ms_queued){
			mid = tail->mid;
			retries = tail->dup;
			retain = tail->retain;
			topic = tail->store->msg.topic;
			qos = tail->qos;
			payloadlen = tail->store->msg.payloadlen;
			payload = tail->store->msg.payload;

			switch(tail->state){
				case ms_publish_qos0:
					rc = _mosquitto_send_publish(context, mid, topic, payloadlen, payload, qos, retain, retries);
					if(!rc){
						if(last){
							last->next = tail->next;
							tail->store->ref_count--;
							_mosquitto_free(tail);
							tail = last->next;
						}else{
							context->msgs = tail->next;
							tail->store->ref_count--;
							_mosquitto_free(tail);
							tail = context->msgs;
						}
					}else{
						return rc;
					}
					break;

				case ms_publish_qos1:
					rc = _mosquitto_send_publish(context, mid, topic, payloadlen, payload, qos, retain, retries);
					if(!rc){
						tail->timestamp = time(NULL);
						tail->dup = 1; /* Any retry attempts are a duplicate. */
						tail->state = ms_wait_for_puback;
					}else{
						return rc;
					}
					last = tail;
					tail = tail->next;
					break;

				case ms_publish_qos2:
					rc = _mosquitto_send_publish(context, mid, topic, payloadlen, payload, qos, retain, retries);
					if(!rc){
						tail->timestamp = time(NULL);
						tail->dup = 1; /* Any retry attempts are a duplicate. */
						tail->state = ms_wait_for_pubrec;
					}else{
						return rc;
					}
					last = tail;
					tail = tail->next;
					break;
				
				case ms_send_pubrec:
					rc = _mosquitto_send_pubrec(context, mid);
					if(!rc){
						tail->state = ms_wait_for_pubrel;
					}else{
						return rc;
					}
					last = tail;
					tail = tail->next;
					break;

				case ms_resend_pubrel:
					rc = _mosquitto_send_pubrel(context, mid, true);
					if(!rc){
						tail->state = ms_wait_for_pubcomp;
					}else{
						return rc;
					}
					last = tail;
					tail = tail->next;
					break;

				case ms_resend_pubcomp:
					rc = _mosquitto_send_pubcomp(context, mid);
					if(!rc){
						tail->state = ms_wait_for_pubrel;
					}else{
						return rc;
					}
					last = tail;
					tail = tail->next;
					break;

				default:
					last = tail;
					tail = tail->next;
					break;
			}
		}else{
			last = tail;
			tail = tail->next;
		}
	}

	return MOSQ_ERR_SUCCESS;
}

void mqtt3_db_store_clean(mosquitto_db *db)
{
	/* FIXME - this may not be necessary if checks are made when messages are removed. */
	struct mosquitto_msg_store *tail, *last = NULL;
	int i;
	assert(db);

	tail = db->msg_store;
	while(tail){
		if(tail->ref_count == 0){
			if(tail->source_id) _mosquitto_free(tail->source_id);
			if(tail->dest_ids){
				for(i=0; i<tail->dest_id_count; i++){
					if(tail->dest_ids[i]) _mosquitto_free(tail->dest_ids[i]);
				}
				_mosquitto_free(tail->dest_ids);
			}
			if(tail->msg.topic) _mosquitto_free(tail->msg.topic);
			if(tail->msg.payload) _mosquitto_free(tail->msg.payload);
			if(last){
				last->next = tail->next;
				_mosquitto_free(tail);
				tail = last->next;
			}else{
				db->msg_store = tail->next;
				_mosquitto_free(tail);
				tail = db->msg_store;
			}
			db->msg_store_count--;
		}else{
			last = tail;
			tail = tail->next;
		}
	}
}

/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
void mqtt3_db_sys_update(mosquitto_db *db, int interval, time_t start_time)
{
	static time_t last_update = 0;
	time_t now = time(NULL);
	time_t uptime;
	char buf[100];
	int value;
	int inactive;
	int active;
#ifndef WIN32
	unsigned long value_ul;
#endif

	static int msg_store_count = -1;
	static int client_count = -1;
	static int client_max = -1;
	static int inactive_count = -1;
	static int active_count = -1;
#ifdef REAL_WITH_MEMORY_TRACKING
	static unsigned long current_heap = -1;
	static unsigned long max_heap = -1;
#endif
	static unsigned long msgs_received = -1;
	static unsigned long msgs_sent = -1;
	static unsigned long pub_msgs_received = -1;
	static unsigned long pub_msgs_sent = -1;
	static unsigned int msgsps_received = -1;
	static unsigned int msgsps_sent = -1;
	static unsigned long long bytes_received = -1;
	static unsigned long long bytes_sent = -1;
	static unsigned long long pub_bytes_received = -1;
	static unsigned long long pub_bytes_sent = -1;
	static unsigned int bytesps_received = -1;
	static unsigned int bytesps_sent = -1;
	static int subscription_count = -1;
	static int retained_count = -1;

	if(interval && now - interval > last_update){
		uptime = now - start_time;
		snprintf(buf, 100, "%d seconds", (int)uptime);
		mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/uptime", 2, strlen(buf), buf, 1);

		if(db->msg_store_count != msg_store_count){
			msg_store_count = db->msg_store_count;
			snprintf(buf, 100, "%d", msg_store_count);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/messages/stored", 2, strlen(buf), buf, 1);
		}

		if(db->subscription_count != subscription_count){
			subscription_count = db->subscription_count;
			snprintf(buf, 100, "%d", subscription_count);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/subscriptions/count", 2, strlen(buf), buf, 1);
		}

		if(db->retained_count != retained_count){
			retained_count = db->retained_count;
			snprintf(buf, 100, "%d", retained_count);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/retained messages/count", 2, strlen(buf), buf, 1);
		}

		if(!mqtt3_db_client_count(db, &value, &inactive)){
			if(client_count != value){
				client_count = value;
				snprintf(buf, 100, "%d", client_count);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/clients/total", 2, strlen(buf), buf, 1);
			}
			if(inactive_count != inactive){
				inactive_count = inactive;
				snprintf(buf, 100, "%d", inactive_count);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/clients/inactive", 2, strlen(buf), buf, 1);
			}
			active = client_count - inactive;
			if(active_count != active){
				active_count = active;
				snprintf(buf, 100, "%d", active_count);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/clients/active", 2, strlen(buf), buf, 1);
			}
			if(value > client_max){
				client_max = value;
				snprintf(buf, 100, "%d", client_max);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/clients/maximum", 2, strlen(buf), buf, 1);
			}
		}

#ifdef REAL_WITH_MEMORY_TRACKING
		value_ul = _mosquitto_memory_used();
		if(current_heap != value_ul){
			current_heap = value_ul;
			snprintf(buf, 100, "%lu bytes", current_heap);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/heap/current size", 2, strlen(buf), buf, 1);
		}
		value_ul =_mosquitto_max_memory_used();
		if(max_heap != value_ul){
			max_heap = value_ul;
			snprintf(buf, 100, "%lu bytes", max_heap);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/heap/maximum size", 2, strlen(buf), buf, 1);
		}
#endif

		if(msgs_received != g_msgs_received){
			msgs_received = g_msgs_received;
			snprintf(buf, 100, "%lu", msgs_received);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/messages/received", 2, strlen(buf), buf, 1);
		}
		
		if(msgs_sent != g_msgs_sent){
			msgs_sent = g_msgs_sent;
			snprintf(buf, 100, "%lu", msgs_sent);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/messages/sent", 2, strlen(buf), buf, 1);
		}

		if(pub_msgs_received != g_pub_msgs_received){
			pub_msgs_received = g_pub_msgs_received;
			snprintf(buf, 100, "%lu", pub_msgs_received);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/publish/messages/received", 2, strlen(buf), buf, 1);
		}
		
		if(pub_msgs_sent != g_pub_msgs_sent){
			pub_msgs_sent = g_pub_msgs_sent;
			snprintf(buf, 100, "%lu", pub_msgs_sent);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/publish/messages/sent", 2, strlen(buf), buf, 1);
		}

		if(bytes_received != g_bytes_received){
			bytes_received = g_bytes_received;
			snprintf(buf, 100, "%llu", bytes_received);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/bytes/received", 2, strlen(buf), buf, 1);
		}
		
		if(bytes_sent != g_bytes_sent){
			bytes_sent = g_bytes_sent;
			snprintf(buf, 100, "%llu", bytes_sent);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/bytes/sent", 2, strlen(buf), buf, 1);
		}
		
		if(pub_bytes_received != g_pub_bytes_received){
			pub_bytes_received = g_pub_bytes_received;
			snprintf(buf, 100, "%llu", pub_bytes_received);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/publish/bytes/received", 2, strlen(buf), buf, 1);
		}

		if(pub_bytes_sent != g_pub_bytes_sent){
			pub_bytes_sent = g_pub_bytes_sent;
			snprintf(buf, 100, "%llu", pub_bytes_sent);
			mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/publish/bytes/sent", 2, strlen(buf), buf, 1);
		}

		if(uptime){
			value = msgs_received/uptime;
			if(msgsps_received != value){
				msgsps_received = value;
				snprintf(buf, 100, "%u", msgsps_received);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/messages/per second/received", 2, strlen(buf), buf, 1);
			}

			value = msgs_sent/uptime;
			if(msgsps_sent != value){
				msgsps_sent = value;
				snprintf(buf, 100, "%u", msgsps_sent);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/messages/per second/sent", 2, strlen(buf), buf, 1);
			}

			value = bytes_received/uptime;
			if(bytesps_received != value){
				bytesps_received = value;
				snprintf(buf, 100, "%u", bytesps_received);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/bytes/per second/received", 2, strlen(buf), buf, 1);
			}

			value = bytes_sent/uptime;
			if(bytesps_sent != value){
				bytesps_sent = value;
				snprintf(buf, 100, "%u", bytesps_sent);
				mqtt3_db_messages_easy_queue(db, NULL, "$SYS/broker/bytes/per second/sent", 2, strlen(buf), buf, 1);
			}
		}

		last_update = time(NULL);
	}
}

void mqtt3_db_limits_set(int inflight, int queued)
{
	max_inflight = inflight;
	max_queued = queued;
}

void mqtt3_db_vacuum(void)
{
	/* FIXME - reimplement? */
}

