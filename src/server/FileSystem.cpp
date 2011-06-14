#include "FileSystem.h"

#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;
using namespace rapidxml;

xml_node<> *FileSystem::fileStat(const string &path, const string &file, memory_pool<> *memoryPool)
{
  xml_node<> *fileNode = memoryPool->allocate_node(node_element, memoryPool->allocate_string("File"));
  string fileName = file;

  /* Build the file path */
  string filePath(path);
  if(filePath[filePath.size()-1] != '/') {
    filePath.append("/");
  }
  filePath.append(fileName);

  /* Attempt to stat the file */  
  struct stat statBuff;
  if (stat(filePath.c_str(), &statBuff) < 0) {
    cerr << __FILE__ << ":" << __LINE__ << "\tCouldn't stat file: " << filePath << endl;
  } else {
    /* If it's a directory, append a slash as an indicator */
    if ((statBuff.st_mode & S_IFMT) == S_IFDIR) {
      fileName.append("/");
    }
  }

  char *attributeValue = memoryPool->allocate_string(fileName.c_str());
  fileNode->append_attribute(memoryPool->allocate_attribute("name", attributeValue));

  return fileNode;
}

int FileSystem::dirList(const string &dirPath, vector<string> &list)
{
  DIR *directory;
  struct dirent *direntBuffer;

  directory = opendir(dirPath.c_str());
  if(!directory) {
    cerr << __FILE__ << ":" << __LINE__ << "\tCouldn't open directory: " << dirPath << endl;
    return errno;
  }

  while(direntBuffer = readdir(directory)) {
    list.push_back(string(direntBuffer->d_name));
  }

  closedir(directory);

  return 0;
}

xml_node<> *FileSystem::dirStat(const string &path, memory_pool<> *memoryPool)
{
  string dirPath(path);
  if(dirPath[dirPath.size() - 1] != '/') {
    dirPath.append("/");
  }

  xml_node<> *dirNode = memoryPool->allocate_node(node_element, "Dir");
  dirNode->append_attribute(memoryPool->allocate_attribute("path", memoryPool->allocate_string(dirPath.c_str())));

  if(path.empty()) {
    return dirNode;
  }

  vector<string> list = vector<string>();
  if(dirList(dirPath, list) == 0) {

    for(unsigned int i=0; i < list.size(); i++) {
      xml_node<> *fileNode = fileStat(dirPath, list[i], memoryPool);
      dirNode->append_node(fileNode);
    }
  }

  return dirNode;
}

xml_node<> *FileSystem::catFile(const string &path, memory_pool<> *memoryPool)
{
  xml_node<> *fileContentNode = memoryPool->allocate_node(node_element, "FileContent");
  fileContentNode->append_attribute(memoryPool->allocate_attribute("path", memoryPool->allocate_string(path.c_str())));

  stringstream fileContent;

  //TODO: Detect and deal with very large files!
  
  ifstream file;
  file.open(path.c_str(), ifstream::in);
  for(;;) {
    char c = (char)file.get();
    if(!file.good()) break;
    fileContent << c;
  }
  file.close();

  fileContentNode->value(memoryPool->allocate_string(fileContent.str().c_str()));
  
  return fileContentNode;
}
