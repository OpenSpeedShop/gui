#ifndef OPENSPEEDSHOPCLI_H
#define OPENSPEEDSHOPCLI_H

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <dlfcn.h>

#include <SS_Input_Manager.hxx>

#include "rapidxml-1.13/rapidxml.hpp"
#include "ServerException.h"

/* Initializing the Open|SpeedShop CLI */
void Openss_Basic_Initialization();
void Openss_Basic_Termination();
extern void pcli_load_messages(void);
extern void pcli_load_scripting_messages(void);

/* Command parsing with the Open|SpeedShop CLI */
extern FILE *yyin;
extern int yyparse(void);
extern ParseResult *p_parse_result;
extern InputLineObject *Current_ILO;
extern CommandObject *Current_CO;

//bool Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, bool need_cli, bool need_gui, OpenSpeedShop_Start_Modes oss_start_mode);

class OpenSpeedShopCLI {
public:
  OpenSpeedShopCLI() { m_windowID = initializeOSS(); }
  ~OpenSpeedShopCLI() { terminateOSS(m_windowID); }

  std::string execute(std::string command) { return execute(m_windowID, command.c_str()); }
  std::string execute(char *command) { return execute(m_windowID, command); }

protected:
  std::string processCommandResults(std::list<CommandResult *> commandResultList);
  InputLineObject *getInputLineObject(int windowID, const char *command);
  std::string execute(int windowID, const char *command);
  int initializeOSS();
  void terminateOSS(int windowID);
  std::string scrubXML(std::string xml, int pos = 0);


  int m_windowID;
  static int m_windowCount;


};

#endif
