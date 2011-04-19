#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>

class Socket
{
public:
  Socket();
  ~Socket();

  bool start(int port);
  bool accept(Socket &clientConnection);
  bool send(std::string str);
  int recv(std::string &str);
  bool close();
  
  const Socket& operator <<(const std::string &str) const;
  const Socket& operator >>(std::string &str) const;

protected:
  int _socketDescriptor;

};

#endif // SOCKET_H
