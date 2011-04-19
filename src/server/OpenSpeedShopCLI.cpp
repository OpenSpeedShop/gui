#include "OpenSpeedShopCLI.h"

std::string OpenSpeedShopCLI::scrubXML(std::string xml, int start)
{
  size_t pos = std::string::npos;

  if((pos = xml.find('\"', start)) != std::string::npos) {
    xml.replace(pos, 1, "&quot;");
  } else if((pos = xml.find('\'', start)) != std::string::npos) {
    xml.replace(pos, 1, "&apos;");
  } else if((pos = xml.find('&', start)) != std::string::npos) {
    xml.replace(pos, 1, "&amp;");
  } else if((pos = xml.find('<', start)) != std::string::npos) {
    xml.replace(pos, 1, "&lt;");
  } else if((pos = xml.find('>', start)) != std::string::npos) {
    xml.replace(pos, 1, "&gt;");
  } else if((pos = xml.find('\n', start)) != std::string::npos) {
    xml.replace(pos, 1, "<![CDATA[<br/>]]>");
  }

  if(pos != std::string::npos) {
    //TODO: increment by the size of the replacement string, instead of just the minimum
    xml = scrubXML(xml, pos + 3);
  }

  return xml;
}

std::string OpenSpeedShopCLI::processCommandResults(std::list<CommandResult *> commandResultList)
{
  std::ostringstream retval;

  std::list<CommandResult *>::iterator commandResultListIterator;
  for(commandResultListIterator = commandResultList.begin(); 
    commandResultListIterator != commandResultList.end(); 
    commandResultListIterator++) {

    CommandResult *commandResult = *commandResultListIterator;

    retval << "<" << typeid(*commandResult).name();

    //TODO: We need to scrub the values for use in XML DOM (i.e. quotes, ampersands, etc.)

    if(typeid(*commandResult) == typeid(CommandResult_Columns)) {
      retval << ">";
      std::list<CommandResult *> columns;
      ((CommandResult_Columns *)commandResult)->Value(columns);
      retval << processCommandResults(columns);
      retval << "</" << typeid(*commandResult).name() << ">" << std::endl;
    } else if(typeid(*commandResult) == typeid(CommandResult_Headers)) {
      retval << ">";
      std::list<CommandResult *> headers;
      ((CommandResult_Headers *)commandResult)->Value(headers);
      retval << processCommandResults(headers);
      retval << "</" << typeid(*commandResult).name() << ">" << std::endl;
    } else if(typeid(*commandResult) == typeid(CommandResult_Function)) {
      CommandResult_Function *function = (CommandResult_Function *)commandResult;
      retval << " name=\"" << scrubXML(function->getName()) << "\"";

      /* NOTE: This may be the wrong way of going about it
       * See plugins/panels/StatsPanel/StatsPanel.cxx:11927 for more 
       * information if we run into problems doing it this way. */
      std::set<Statement> statements = function->getDefinitions();

      if(!statements.empty()) {
        Statement statement = *statements.begin();  // There should only be one!
        retval << " path=\"" << scrubXML(statement.getPath()) << "\"";
        retval << " line=\"" << statement.getLine() << "\"";
      }

      retval << " />" << std::endl;
    } else if(typeid(*commandResult) == typeid(CommandResult_Statement)) {
      CommandResult_Statement *statement = (CommandResult_Statement *)commandResult;
      retval << " path=\"" << scrubXML(statement->getPath()) << "\"";
      retval << " line=\"" << statement->getLine() << "\"";
      retval << " />" << std::endl;
    } else if(typeid(*commandResult) == typeid(CommandResult_CallStackEntry)) {
      CommandResult_CallStackEntry *callStackEntry = (CommandResult_CallStackEntry *)commandResult;
      std::vector<CommandResult *> *callStackEntryVector = callStackEntry->Value();
      std::list<CommandResult *> callStackEntryList(callStackEntryVector->rbegin(), callStackEntryVector->rend());
      retval << ">";
      retval << processCommandResults(callStackEntryList);
      retval << "</" << typeid(*commandResult).name() << ">" << std::endl;
    } else {
      retval << " value=\"" << scrubXML(commandResult->Form()) << "\"";
      retval << " />" << std::endl;
    }

  }
  
  return retval.str();
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

std::string OpenSpeedShopCLI::execute(int windowID, const char *command)
{
  std::ostringstream retval;

  try {
    InputLineObject *inputLineObject = getInputLineObject(windowID, command);

    retval << "<Response>" << std::endl;

    /* It appears that there is always only one CommandObject returned, even though
     * it is in a list.  We'll process it properly, just in case: bits are cheap */
    std::list<CommandObject *> commandObjectList = inputLineObject->CmdObj_List();
    std::list<CommandObject *>::iterator commandObjectListIterator;
    for(commandObjectListIterator = commandObjectList.begin(); 
      commandObjectListIterator != commandObjectList.end(); 
      commandObjectListIterator++) {

      retval << "<CommandObject>" << std::endl;
      retval << processCommandResults((*commandObjectListIterator)->Result_List());
      retval << "</CommandObject>" << std::endl;

    }

    retval << "</Response>" << std::endl;

    if(inputLineObject) {
      delete inputLineObject;
      Current_ILO = inputLineObject = NULL;
    }

  } catch (std::exception &error) {
    std::ostringstream retval;
    retval << "<exception>" << error.what() << "</exception>" << std::endl;
    return retval.str();
  } catch (std::string error) {
    return std::string("<error>" + error + "</error>");
  } catch(...) {
    return std::string("<exception>unknown</exception>");
  } 

  return retval.str();
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


