#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "OpenSpeedShopCLI.h"
#include "Socket.h"

class SocketServer {

public:
  SocketServer();
  ~SocketServer();

protected:
  OpenSpeedShopCLI _cli;
  Socket _socket;

};


#endif // SOCKETSERVER_H

