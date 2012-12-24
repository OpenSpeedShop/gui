#include "Direct.h"

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#include "OpenSpeedShopCLI.h"

using namespace std;
using namespace rapidxml;

string Direct::execute(const string &command)
{
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
                serverResponse->append_attribute(responseDocument.allocate_attribute("version", "DirectConnection_0.1.dev"));
            } else if(commandText == "exit") {
                //Ignore, there's nothing to do
            }

            responseNode->append_node(serverResponse);

        } else if(commandType == "OpenSpeedShopCLI") {
            commandText += '\n';
            xml_node<> *cliResponse = this->_cli->execute(commandText, &responseDocument);
            responseNode->append_node(cliResponse);
        }

        // Send the result back to the client
        cout << "responseDocument \"\n" << responseDocument << "\n\"" << endl;
        ostringstream responseString;
        responseString << responseDocument;

        return responseString.str();

    } catch(...) {
        cerr << __FILE__ << ":" << __LINE__ << "\tError caught" << endl;
    }

    return NULL;
}

Direct::Direct() :
    _cli(new OpenSpeedShopCLI())
{
}

Direct::~Direct()
{
    if(_cli) {
        delete _cli;
    }
}

