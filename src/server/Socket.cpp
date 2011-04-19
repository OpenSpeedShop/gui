
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

#include "Socket.h"

Socket::Socket()
{
  _socketDescriptor = -1;
}

Socket::~Socket()
{
}

bool Socket::start(int port)
{
  int status;

  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;  //TODO: set this properly
  serverAddress.sin_port = htons(port);

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tCreating socket" << std::endl;
  _socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(_socketDescriptor < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while opening socket: " << _socketDescriptor << std::endl;
    return false;
  }

  int on = 1;
  status = setsockopt(_socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
  if(status < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError setting socket options: " << status << std::endl;
    return false;
  }


  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBinding socket" << std::endl;
  status = bind(_socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
  if(status < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError binding socket to hostname: " << status << std::endl;
    return false;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBeginning socket listen" << std::endl;
  status = listen(_socketDescriptor, 5);
  if(status < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError listening socket: " << status << std::endl;
    return false;
  }

  return true;
}

bool Socket::accept(Socket &clientConnection)
{
  struct sockaddr_in clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBeginning connection accept" << std::endl;
  clientConnection._socketDescriptor = ::accept(_socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressSize);
  if(clientConnection._socketDescriptor < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while accepting connection: " 
	                                         << clientConnection._socketDescriptor << std::endl;
    return false;
  }
  
  return true;
}

bool Socket::close()
{
  ::close(_socketDescriptor);
  return true;
}

bool Socket::send(std::string str)
{
  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tSending data" << std::endl;
  return (::send(_socketDescriptor, str.c_str(), str.size(), MSG_NOSIGNAL) >= 0);
}

int Socket::recv(std::string &str)
{
  int retval = 0;
  str = "";

  int bufferSize = 64*1024;
  char buffer[bufferSize + 1];
  memset(buffer, 0, bufferSize + 1);

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tReceiving data" << std::endl;
  retval = ::recv(_socketDescriptor, buffer, bufferSize, 0);
  if(retval < 0) {
    std::cerr << "Error while recieving" << std::endl;    // More than likely an immediate disconnect
    return 0;
  }

  str = buffer;
  return retval;
}


