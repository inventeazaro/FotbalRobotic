
// Based on interface defined in Ethernet's Client.h

#ifndef __WIFLY_CLIENT_H__
#define __WIFLY_CLIENT_H__

#include "Client.h"
#include "IPAddress.h"

#include "ParsedStream.h"

#include "WiFlyDevice.h"

class WiFlyClient : public Client {
 public:
  WiFlyClient();


  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek(); 
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();

  uint8_t *_ip;
  uint16_t _port;

  const char *_domain;
private:
  WiFlyDevice& _WiFly;
  boolean _connect();

  bool isOpen;

  ParsedStream stream;


  // TODO: Work out why alternate instantiation code in
  //       Server.available() doesn't work and thus requires this:
  friend class WiFlyServer;
};

#endif
