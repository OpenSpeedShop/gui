#ifndef DIRECT_H
#define DIRECT_H

#include <string>

class OpenSpeedShopCLI;

class Direct {
public:
  Direct();
  ~Direct();

  std::string execute(const std::string &command);

private:
  OpenSpeedShopCLI *_cli;

};

#endif // DIRECT_H

