#include "SocketServer.h"

SocketServer::SocketServer()
{
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
      sleep(1); //DEBUG:

      std::string command;
      while(clientConnection.recv(command)) {
        try {
          // Parse the command
          rapidxml::xml_document<> commandDocument;
          commandDocument.parse<0>(commandDocument.allocate_string(command.c_str()));

          rapidxml::xml_node<> *commandNode = commandDocument.first_node("Command");
          std::string commandText(commandNode->value());
          std::string commandType(commandNode->first_attribute("type")->value());
          char *commandID(commandNode->first_attribute("id")->value());

          // Deal with an OpenSpeedShopCLI command
          if(commandType == "OpenSpeedShopCLI") {
            rapidxml::xml_document<> responseDocument;
            rapidxml::xml_node<> *responseNode = responseDocument.allocate_node(rapidxml::node_element, "Response");
            responseNode->append_attribute(responseDocument.allocate_attribute("commandID", commandID));
            responseDocument.append_node(responseNode);

            commandText += '\n';
            std::string cliResponse = _cli.execute(commandText);
            responseNode->value(responseDocument.allocate_string(cliResponse.c_str()));
            std::cout << "responseDocument " << responseDocument << std::endl;

            std::ostringstream responseString;
            responseString << responseDocument;
            clientConnection.send(responseString.str());
          }
        } catch(...) {
          std::cerr << "Error caught" << std::endl;
        }
      }
      clientConnection.close();
      exit(0);
    }
//    sleep(1);
  }
  _socket.close();
}

SocketServer::~SocketServer()
{
}

