#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#include "OpenSpeedShopCLI.h"
#include "FileSystem.h"
#include "Socket.h"

class SocketServer {

public:
  SocketServer(int port = 2048, const char *address = NULL);
  ~SocketServer();

protected:
  OpenSpeedShopCLI _cli;
  FileSystem _fileSystem;
  Socket _socket;

};


#endif // SOCKETSERVER_H

