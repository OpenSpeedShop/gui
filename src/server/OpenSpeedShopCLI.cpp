#include "OpenSpeedShopCLI.h"

std::string &OpenSpeedShopCLI::trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

OpenSpeedShopCLI::OpenSpeedShopCLI()
{
  m_windowID = initializeOSS();
}

OpenSpeedShopCLI::~OpenSpeedShopCLI()
{
  terminateOSS(m_windowID);
}

std::list<rapidxml::xml_node<> *> OpenSpeedShopCLI::processCommandResults(
    std::list<CommandResult *> commandResultList,
    rapidxml::memory_pool<> *memoryPool )
{
  std::list<rapidxml::xml_node<> *> commandResults;

  std::list<CommandResult *>::iterator commandResultListIterator;
  for(commandResultListIterator = commandResultList.begin();
    commandResultListIterator != commandResultList.end();
    commandResultListIterator++) {

    CommandResult *commandResult = *commandResultListIterator;
    rapidxml::xml_node<> *commandResultNode;
    
    std::string nodeName(typeid(*commandResult).name());
    int start = nodeName.find("CommandResult_") + 14;
    char *nodeNameString = memoryPool->allocate_string(nodeName.substr(start).c_str());
    commandResultNode = memoryPool->allocate_node(rapidxml::node_element, nodeNameString);

    if(typeid(*commandResult) == typeid(CommandResult_Columns)) {
      // Get a list of the column nodes
      std::list<CommandResult *> columns;
      ((CommandResult_Columns *)commandResult)->Value(columns);
      std::list<rapidxml::xml_node<> *> childNodes = processCommandResults(columns, memoryPool);

      // Add them to this node as children
      bool killThisRow = true;
      std::list<rapidxml::xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        rapidxml::xml_node<> *child = *childNodesListIterator;
  
        // We need to test if this is a disparate CallStackEntry row
        std::string childName(child->name());
        if(childName.compare("String") != 0 && childName.compare("CallStackEntry") != 0) {
          // Anything but a String or CallStackEntry cancels the kill
          killThisRow = false;
        } else if(childName.compare("String") == 0) {
          // A string with a non-empty value will also cancel the kill
          std::string childValue(child->first_attribute("value")->value());
          if(!childValue.empty()) {
            killThisRow = false;
          }
        }
        
        commandResultNode->append_node(child);
      }

      // If we iterated over the children, and found this row to be a what we're testing for, move on to the next CommandResult
      if(killThisRow) { continue; }

    } else if(typeid(*commandResult) == typeid(CommandResult_Headers)) {
      // Get a list of header nodes
      std::list<CommandResult *> headers;
      ((CommandResult_Headers *)commandResult)->Value(headers);
      std::list<rapidxml::xml_node<> *> childNodes = processCommandResults(headers, memoryPool);

      // Add them to this node as children
      std::list<rapidxml::xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        commandResultNode->append_node(*childNodesListIterator);
      }

    } else if(typeid(*commandResult) == typeid(CommandResult_Function)) {
      CommandResult_Function *function = (CommandResult_Function *)commandResult;
      char *attributeValue = memoryPool->allocate_string(function->getName().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("value", attributeValue));

      /* NOTE: This may be the wrong way of going about it
       * See plugins/panels/StatsPanel/StatsPanel.cxx:11927 for more
       * information if we run into problems doing it this way. */
      std::set<Statement> statements = function->getDefinitions();

      if(!statements.empty()) {
        Statement statement = *statements.begin();  // There should only be one!

        attributeValue = memoryPool->allocate_string(statement.getPath().c_str());
        commandResultNode->append_attribute(memoryPool->allocate_attribute("path", attributeValue));

        std::ostringstream stringStream;
        stringStream << statement.getLine();
        attributeValue = memoryPool->allocate_string(stringStream.str().c_str());
        commandResultNode->append_attribute(memoryPool->allocate_attribute("line", attributeValue));
      }

    } else if(typeid(*commandResult) == typeid(CommandResult_Statement)) {
      CommandResult_Statement *statement = (CommandResult_Statement *)commandResult;

      char *attributeValue = memoryPool->allocate_string(statement->getPath().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("path", attributeValue));

      std::ostringstream stringStream;
      stringStream << statement->getLine();
      attributeValue = memoryPool->allocate_string(stringStream.str().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("line", attributeValue));

    } else if(typeid(*commandResult) == typeid(CommandResult_CallStackEntry)) {
      // Get a list of the call stack entries
      CommandResult_CallStackEntry *callStackEntry = (CommandResult_CallStackEntry *)commandResult;
      std::vector<CommandResult *> *callStackEntryVector = callStackEntry->Value();
      std::list<CommandResult *> callStackEntryList(callStackEntryVector->rbegin(), callStackEntryVector->rend());
      std::list<rapidxml::xml_node<> *> childNodes = processCommandResults(callStackEntryList, memoryPool);

      /* //NOTE: It looks like the CallStackEntries aren't created as expected.  We get both a row-based hierarchy, and a 
                 "building" stack object.  We're looking for only the "full-stack" entry, and then removing the previous 
                 partial-stack rows, based on the count.  See the CommandResult_Columns elseif for more details on the 
                 culling process.
       */

      // Add them to this node as children
      std::list<rapidxml::xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        commandResultNode->append_node(*childNodesListIterator);
      }
      
    } else {
      std::string value = commandResult->Form();
      char *attributeValue = memoryPool->allocate_string(trim(value).c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("value", attributeValue));

    }

    commandResults.push_back(commandResultNode);
  }

  return commandResults;
}

InputLineObject *OpenSpeedShopCLI::getInputLineObject(int windowID, const char *command)
{
  static int sequence = 0;

  InputLineObject *inputLineObject = new InputLineObject(windowID, std::string(command));
  inputLineObject->SetSeq(++sequence);
  inputLineObject->SetStatus(ILO_IN_PARSER);

  ParseResult *parseResult = new ParseResult();
  p_parse_result = parseResult;     //GLOBAL: YACC methods require this

  if(!(yyin = tmpfile())) {         //GLOBAL: YACC methods require this
    throw ServerException("Failed to open temporary file");
  }

  fprintf(yyin, command);
  rewind(yyin);

  if(yyparse()) {
    fclose(yyin);                   // Can't do anything about errors here
    throw ServerException("Failed to parse with YACC");
  }

  if(fclose(yyin)) {
    throw ServerException("Failed to close temporary file");
  }

  if(parseResult->syntaxError()) {
    throw ServerException("Syntax error");
  }

  Current_ILO = inputLineObject;    //GLOBAL: CommandObject constructor requires this
  CommandObject *commandObject = new CommandObject(parseResult, true);

  if (parseResult->getCommandType() == CMD_HEAD_ERROR) {
    throw ServerException("Error in CommandObject");
  }

  SS_Execute_Cmd(commandObject);

  while(!inputLineObject->Semantics_Complete()) {
    //TODO: Add methods for timeout and canceling the operation

    if(inputLineObject->What() == ILO_ERROR) {
      if(inputLineObject) inputLineObject->Set_Results_Used();
      throw ServerException("Error in inputLineObject processing");
    }

    sleep(1);
  }

  return commandObject->Clip();
}


rapidxml::xml_node<> *OpenSpeedShopCLI::execute(std::string command, rapidxml::memory_pool<> *memoryPool)
{
  rapidxml::xml_node<> *cliResponseNode = memoryPool->allocate_node(rapidxml::node_element, "OpenSpeedShopCLI");

  try {
    InputLineObject *inputLineObject = getInputLineObject(m_windowID, command.c_str());

    /* It appears that there is always only one CommandObject returned, even though
     * it is in a list.  We'll process it properly, just in case: bits are cheap */
    std::list<CommandObject *> commandObjectList = inputLineObject->CmdObj_List();
    std::list<CommandObject *>::iterator commandObjectListIterator;
    for(commandObjectListIterator = commandObjectList.begin();
      commandObjectListIterator != commandObjectList.end();
      commandObjectListIterator++) {

      rapidxml::xml_node<> *commandObjectNode =
              memoryPool->allocate_node(rapidxml::node_element, memoryPool->allocate_string("CommandObject"));

      std::list<rapidxml::xml_node<> *> childNodes =
              processCommandResults((*commandObjectListIterator)->Result_List(), memoryPool);

      std::list<rapidxml::xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        commandObjectNode->append_node(*childNodesListIterator);
      }

      cliResponseNode->append_node(commandObjectNode);
    }

    if(inputLineObject) {
      delete inputLineObject;
      Current_ILO = inputLineObject = NULL;
    }

  } catch (std::exception &error) {
    char *errorWhat = memoryPool->allocate_string(error.what());
    cliResponseNode->append_node(memoryPool->allocate_node(rapidxml::node_element, "Exception", errorWhat));
  } catch (std::string error) {
    char *errorWhat = memoryPool->allocate_string(error.c_str());
    cliResponseNode->append_node(memoryPool->allocate_node(rapidxml::node_element, "Exception", errorWhat));
  } catch(...) {
    cliResponseNode->append_node(memoryPool->allocate_node(rapidxml::node_element, "Exception", "Unknown"));
  }

  return cliResponseNode;
}


// Keeps count of the "windows" that we have open (used in initialize and terminate)
int OpenSpeedShopCLI::m_windowCount = 0;

int OpenSpeedShopCLI::initializeOSS()
{
  if(!m_windowCount++) {
    Openss_Basic_Initialization();
  }

  pid_t pid = getpid();
  char hostName[256];
  gethostname(&*hostName,255);
  int windowID = Embedded_Window("SocketServerInterface", &*hostName, pid, 0, false);

  pcli_load_scripting_messages();

  return windowID;
}

void OpenSpeedShopCLI::terminateOSS(int windowID)
{
  /* For some reason this is already called when main() returns. Though everywhere I look in
   * Open|SpeedShop's code, these are called.  Maybe this only applies for Embedded_Windows? */

//  Window_Termination(windowID);  // Probably not necessary I think all of the windows are destroyed in Commander_Termination()

  if(!--m_windowCount) {
//    Openss_Basic_Termination();
  }

}


