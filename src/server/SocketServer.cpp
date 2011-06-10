#include "SocketServer.h"

using namespace std;
using namespace rapidxml;

SocketServer::SocketServer()
{
  if(!_socket.start(2048)) {
    cerr << __FILE__ << ":" << __LINE__ << "\tError code returned from SocketServer.start: " << endl;
    return;
  }

  for(;;) {
    Socket clientConnection;
	  _socket.accept(clientConnection);

    pid_t childPID = fork();
    if(childPID < 0) {
      cerr << __FILE__ << ":" << __LINE__ << "\tForking error: " << childPID << endl;
      return;
    } else if(childPID > 0) {
      cerr << __FILE__ << ":" << __LINE__ << "\tChild process spawned: " << childPID << endl;
    } else {
      sleep(1); //DEBUG:

      bool exitNow = false;

      string command;
      while(clientConnection.recv(command)) {
        try {
          // Parse the command
          xml_document<> commandDocument;
          commandDocument.parse<0>(commandDocument.allocate_string(command.c_str()));

          xml_node<> *commandNode = commandDocument.first_node("Command");
          string commandText(commandNode->value());
          string commandType(commandNode->first_attribute("type")->value());
          char *commandID(commandNode->first_attribute("id")->value());

          cerr << __FILE__ << ":" << __LINE__ << "\tRecieved commandText: \"" << commandText << "\""
                                              << " commandType: \""           << commandType << "\""
                                              << " commandID: \""             << commandID   << "\"" 
                                              << endl;

          // Build the response document
          xml_document<> responseDocument;
          xml_node<> *responseNode = responseDocument.allocate_node(node_element, "Response");
          responseNode->append_attribute(responseDocument.allocate_attribute("commandID", commandID));
          responseDocument.append_node(responseNode);

          // Deal with a socket server command
          if(commandType == "Server") {
            xml_node<> *serverResponse = responseDocument.allocate_node(node_element, "ServerResponse");

            if(commandText == "version") {
              serverResponse->append_attribute(responseDocument.allocate_attribute("version", "0.1.dev"));
            } else if(commandText == "exit") {
              exitNow = true;
            }

            responseNode->append_node(serverResponse);

          // Deal with FileSystem requests
          } else if(commandType == "FileSystem") {
            xml_node<> *fileSystemResponse = responseDocument.allocate_node(node_element, "FileSystem");
            fileSystemResponse->append_node( _fileSystem.dirStat(commandText, &responseDocument) );
            responseNode->append_node(fileSystemResponse);

          // Deal with an OpenSpeedShopCLI command
          } else if(commandType == "OpenSpeedShopCLI") {
            commandText += '\n';
            xml_node<> *cliResponse = _cli.execute(commandText, &responseDocument);
            responseNode->append_node(cliResponse);

          }


          // Send the result back to the client
          cout << "responseDocument \"\n" << responseDocument << "\n\"" << endl;
          ostringstream responseString;
          responseString << responseDocument;
          clientConnection.send(responseString.str());

          if(exitNow) break;

        } catch(...) {
          cerr << __FILE__ << ":" << __LINE__ << "\tError caught" << endl;
        }
      }

      cerr << __FILE__ << ":" << __LINE__ << "\tClosing connection" << endl;
      clientConnection.close();

      cerr << __FILE__ << ":" << __LINE__ << "\tExiting forked child" << endl;
      exit(0);
    }
  }
  _socket.close();
}

SocketServer::~SocketServer()
{
}

