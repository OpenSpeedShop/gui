#include <string>
#include "SocketServer.h"

void help(const char *command)
{
  std::cout << "\nUsage: " << command << " [options]\n";
  std::cout << "Options:\n";
  std::cout << "  -p #           Port number\n";
  std::cout << "  -a #.#.#.#     IP address\n\n";
}

int main(int argc, char **argv)
{
  int port = 2048;
  char *address = NULL;

  for(int i = 1; i < argc; ++i) {
    if(strcmp(argv[i], "-p") == 0) {
      port = atoi(argv[++i]);

    } else if(strcmp(argv[i], "-a") == 0) {
      address = argv[++i];

    } else if(strcmp(argv[i], "-h") == 0) {
      help(argv[0]);
      exit(0);

    } else {
      std::cerr << "Error parsing parameters\n";
      help(argv[0]);
      exit(1);

    }
  }

  SocketServer _server(port, address);

  return 0;
}

