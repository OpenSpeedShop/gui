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

class OpenSpeedShopCLI {
public:
  OpenSpeedShopCLI();
  ~OpenSpeedShopCLI();

  rapidxml::xml_node<> *execute(std::string command, rapidxml::memory_pool<> *memoryPool);

protected:
  int initializeOSS();
  void terminateOSS(int windowID);

  InputLineObject *getInputLineObject(int windowID, const char *command);
  std::list<rapidxml::xml_node<> *> processCommandResults(
        std::list<CommandResult *> commandResultList,
        rapidxml::memory_pool<> *memoryPool,
        const std::string &parentName = std::string() );

  int m_windowID;
  std::vector<rapidxml::xml_node<> *> _headerNodes;
  static int m_windowCount;

  inline std::string &trim(std::string &s);

};

#endif
