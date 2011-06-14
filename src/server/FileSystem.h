#include <iostream>
#include <string>
#include <vector>

#include "rapidxml-1.13/rapidxml.hpp"

#define DEBUG

using namespace std;
using namespace rapidxml;

class FileSystem {
public:
  FileSystem() {}
  FileSystem(FileSystem &other) {}
  ~FileSystem() {}
  
  xml_node<> *dirStat(const string &path, memory_pool<> *memoryPool);
  xml_node<> *fileStat(const string &path, const string &file, memory_pool<> *memoryPool);
  xml_node<> *catFile(const string &path, memory_pool<> *memoryPool);

protected:
  int dirList(const string &path, vector<string> &list);
};

