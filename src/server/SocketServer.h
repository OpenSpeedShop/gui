#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

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

