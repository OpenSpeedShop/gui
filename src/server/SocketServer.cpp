#include "SocketServer.h"

SocketServer::SocketServer()
{
  // std::cout << _cli.execute("list -v views\n") << std::endl;
  // std::cout << _cli.execute("help\n") << std::endl;
  
  if(!_socket.start(2048)) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\tError code returned from SocketServer.start: " << std::endl;
	return;
  }
  
  for(;;) {
    Socket clientConnection;
	_socket.accept(clientConnection);

    pid_t childPID = fork();
    if(childPID < 0) {
      std::cerr << __FILE__ << ":" << __LINE__ << "\tForking error: " << childPID << std::endl;
      return;
    } else if(childPID > 0) {
      std::cerr << __FILE__ << ":" << __LINE__ << "\tChild process spawned: " << childPID << std::endl;
    } else {
      //TODO: Parse commands, and do our bidding!

	  sleep(1);  //DEBUG: Delay the output to cerr/cout a bit for debugging purposes

	  std::string recieved;
	  while(clientConnection.recv(recieved)) {
        std::cout << __FILE__ << ":" << __LINE__ << "\tRecieved: '" << recieved << "'" << std::endl;
        std::string cliResponse = _cli.execute(recieved);
        std::cout << __FILE__ << ":" << __LINE__ << "\tSending: '" << cliResponse << "'" << std::endl;
        clientConnection.send(cliResponse);
      }

	  clientConnection.close();
      exit(0);
    }
     sleep(1);
  }

  _socket.close();

}

SocketServer::~SocketServer()
{
}

