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

      bool exitNow = false;

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

          std::cerr << __FILE__ << ":" << __LINE__ << "\tRecieved commandText: \"" << commandText << "\""
                                                   << " commandType: \"" << commandType << "\""
                                                   << " commandID: \"" << commandID << "\"" 
                                                   << std::endl;

          // Build the response document
          rapidxml::xml_document<> responseDocument;
          rapidxml::xml_node<> *responseNode = responseDocument.allocate_node(rapidxml::node_element, "Response");
          responseNode->append_attribute(responseDocument.allocate_attribute("commandID", commandID));
          responseDocument.append_node(responseNode);

          // Deal with an OpenSpeedShopCLI command
          if(commandType == "Server") {
            rapidxml::xml_node<> *serverResponse =
                    responseDocument.allocate_node(rapidxml::node_element, responseDocument.allocate_string("ServerResponse"));

            if(commandText == "version") {
              serverResponse->append_attribute(responseDocument.allocate_attribute("version", "0.1.dev"));
            } else if(commandText == "exit") {
              exitNow = true;
            }

            responseNode->append_node(serverResponse);

          } else if(commandType == "OpenSpeedShopCLI") {
            commandText += '\n';
            rapidxml::xml_node<> *cliResponse = _cli.execute(commandText, &responseDocument);
            responseNode->append_node(cliResponse);

          }


          // Send the result back to the client
          std::cout << "responseDocument " << responseDocument << std::endl;
          std::ostringstream responseString;
          responseString << responseDocument;
          clientConnection.send(responseString.str());

          if(exitNow) break;

        } catch(...) {
          std::cerr << __FILE__ << ":" << __LINE__ << "\tError caught" << std::endl;
        }
      }

      std::cerr << __FILE__ << ":" << __LINE__ << "\tClosing connection" << std::endl;
      clientConnection.close();

      std::cerr << __FILE__ << ":" << __LINE__ << "\tExiting forked child" << std::endl;
      exit(0);
    }
  }
  _socket.close();
}

SocketServer::~SocketServer()
{
}

