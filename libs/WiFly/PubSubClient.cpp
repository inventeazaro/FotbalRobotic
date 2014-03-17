/*
 * PubSubClient.cpp - A simple client for MQTT.
 *  Nicholas O'Leary
 *  http://knolleary.net
 */

#include "PubSubClient.h"
#include <string.h>

PubSubClient::PubSubClient(Client& client) {
	this->_client = &client;
}

PubSubClient::PubSubClient(uint8_t *ip, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), Client& client) {
	this->_client = &client;
	this->callback = callback;
	this->ip = ip;
	this->port = port;
}

PubSubClient::PubSubClient(char* domain, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), Client& client){
	_client = &client;
	this->callback = callback;
	this->domain = domain;
	this->port = port;
}

boolean PubSubClient::connect(char *id) {
	return connect(id,0,0,0,0);
}

boolean PubSubClient::connect(char *id, char* willTopic, uint8_t willQos, uint8_t willRetain, char* willMessage) {
	if (!connected()) {
		int result = 0;
		
		if (domain != NULL) {// connect using domain of server
			result = _client->connect(this->domain, this->port);
		} else {// connect using ip of server
			result = _client->connect(this->ip, this->port);
		}
		
		if (result) {
			nextMsgId = 1;
			//set MSB,LSB,MSG,PROT version
			uint8_t d[9] = {0x00,0x06,'M','Q','I','s','d','p',MQTTPROTOCOLVERSION};
			uint8_t length = 0;
			unsigned int j;
			for (j = 0;j<9;j++) {
				buffer[length++] = d[j];
			}
			// set connect flags
			if (willTopic) {
				buffer[length++] = 0x06|(willQos<<3)|(willRetain<<5);
			} else {
				buffer[length++] = 0x02;
			}
			// set MSB,LSB of Keep Alive timer
			buffer[length++] = ((MQTT_KEEPALIVE) >> 8);
			buffer[length++] = ((MQTT_KEEPALIVE) & 0xff);
			//set payload client id
			length = writeString(id,buffer,length);
			//set payload client if will topic
			if (willTopic) {
				length = writeString(willTopic,buffer,length);
				length = writeString(willMessage,buffer,length);
			}
			// send CONNECT msg to serial
			write(MQTTCONNECT,buffer,length);//fixed header,var header, length of var header
			lastOutActivity = millis();
			lastInActivity = millis();
			// check availability of client for 15mins
			while (!_client->available()) {
				unsigned long t= millis();
				if (t-lastInActivity > MQTT_KEEPALIVE*1000) {
					_client->stop();
					return false;
				}
			}
			// receive packet from MQTT server
			uint16_t len = readPacket();
			
			if (len == 4 && buffer[3] == 0) {//4 bytes length(2 fix header + 2(compression + connect respons))
				// buffer[3] == 0 means connection accepted
				lastInActivity = millis();
				pingOutstanding = false;
				return true;
			}
		}
		_client->stop();
	}
	return false;
}

uint8_t PubSubClient::readByte() {
	while(!_client->available()) {}
	return _client->read();
}

uint16_t PubSubClient::readPacket() {
	uint16_t len = 0;
	buffer[len++] = readByte();//reading the fixed header
	uint8_t multiplier = 1;
	uint16_t length = 0;
	uint8_t digit = 0;
	do {// decoding the remaining length from variable length encoding scheme to decimal
		digit = readByte();
		buffer[len++] = digit;
		length += (digit & 127) * multiplier;
		multiplier *= 128;
	} while ((digit & 128) != 0);
	
	for (uint16_t i = 0;i<length;i++)
	{//reading the variable header and payload if exists
		if (len < MQTT_MAX_PACKET_SIZE) {
			buffer[len++] = readByte();
		} else {
			readByte();
			len = 0; // This will cause the packet to be ignored.
		}
	}
	
	return len;
}

boolean PubSubClient::loop() {
	if (connected()) {
		unsigned long t = millis();
		if ((t - lastInActivity > MQTT_KEEPALIVE*1000) || (t - lastOutActivity > MQTT_KEEPALIVE*1000)) {
			if (pingOutstanding) {
				_client->stop();
				return false;
			} else {// PINGREQ fixed header (2 bytes --- pingreq msg type + 0 for remaining length)
				_client->write(MQTTPINGREQ);
				_client->write((uint8_t)0);
				lastOutActivity = t;
				lastInActivity = t;
				pingOutstanding = true;
			}
		}
		if (_client->available()) {
			uint16_t len = readPacket();
			if (len > 0) {
				lastInActivity = t;
				uint8_t type = buffer[0]&0xF0;
				if (type == MQTTPUBLISH) {
					if (callback) {
						//variable header's topic's length
						uint16_t tl = (buffer[2]<<8)+buffer[3];
						char topic[tl+1];
						for (uint16_t i=0;i<tl;i++) {// get each char of the string
							topic[i] = buffer[4+i];//4+i cuz 2bytes for fixed header and 2 bytes for string length
						}
						topic[tl] = 0;
						// ignore msgID - only support QoS 0 subs
						//variable header's payload's length
						uint16_t pl = len-tl-4; 
						uint8_t payload[pl];
						for (uint16_t i = 0; i<pl; i++) {
							payload[i] = buffer[4+i+tl];
						}
						callback(topic, payload, pl);
					}
				} else if (type == MQTTPINGREQ) { // respond to PINGREQ requested from server with PINGRESP
					_client->write(MQTTPINGRESP);
					_client->write((uint8_t)0);
				} else if (type == MQTTPINGRESP) {// handle PINGRESP send from server as answear from client's PINGREQ
					pingOutstanding = false;
				}
			}
		}
		return true;
	}
	return false;
}

boolean PubSubClient::publish(char* topic, char* payload) {
	return publish(topic,(uint8_t*)payload,strlen(payload),false);
}

boolean PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength) {
	return publish(topic, payload, plength, false);
}

boolean PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength, boolean retained) {
	if (connected()) {
		uint16_t length = writeString(topic,buffer,false);// false for pos = 0
		uint16_t i;
		for (i=0;i<plength;i++) {// store to buffer variable header+payload
			buffer[length++] = payload[i];
		}
		uint8_t header = MQTTPUBLISH; // set publish header value
		if (retained) {
			header |= 1;
		}
		return write(header,buffer,length);// send publish msg to server
	}
	return false;
}

boolean PubSubClient::write(uint8_t header, uint8_t* buf, uint16_t length) {
	uint8_t lenBuf[4];
	uint8_t llen = 0;
	uint8_t digit;
	uint8_t pos = 0;
	uint8_t rc;
	uint8_t len = length;
	do { //encoding from decimal len to variable length encoding scheme
		digit = len % 128;
		len = len / 128;
		if (len > 0) {
			digit |= 0x80;
		}
		lenBuf[pos++] = digit;
		llen++;
	} while(len>0);
	
	rc = _client->write(header);			//1st byte fixed header(Msg type)
	rc += _client->write(lenBuf,llen);	//2nd byte fixed header(Remaining length value-4byte long)
	rc += _client->write(buf,length);	//variable header+payload
	lastOutActivity = millis();
	return (rc == 1+llen+length);
}


boolean PubSubClient::subscribe(char* topic) {
	if (connected()) {
		uint16_t length = 2;
		nextMsgId++;
		if (nextMsgId == 0) {
			nextMsgId = 1;
		}
		buffer[0] = nextMsgId >> 8;
		buffer[1] = nextMsgId - (buffer[0]<<8);
		length = writeString(topic, buffer,length);
		buffer[length++] = 0; // Only do QoS 0 subs
		return write(MQTTSUBSCRIBE|MQTTQOS1,buffer,length);
	}
	return false;
}

void PubSubClient::disconnect() {
	_client->write(MQTTDISCONNECT);
	_client->write((uint8_t)0);
	_client->stop();
	lastInActivity = millis();
	lastOutActivity = millis();
}

uint16_t PubSubClient::writeString(char* string, uint8_t* buf, uint16_t pos) {// put string to buf at place pos
	char* idp = string;
	uint16_t i = 0;
	pos += 2;
	while (*idp) {
		buf[pos++] = *idp++;
		i++;
	}
	buf[pos-i-2] = 0; // string MSB length 
	buf[pos-i-1] = i; // string LSB length
	return pos;
}


boolean PubSubClient::connected() {
	int rc = (int)_client->connected();
	if (!rc) _client->stop();
	return rc;
}