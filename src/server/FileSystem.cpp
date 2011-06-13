#include "FileSystem.h"

#include <sstream>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;
using namespace rapidxml;

xml_node<> *FileSystem::fileStat(const string &path, const string &file, memory_pool<> *memoryPool)
{
  xml_node<> *fileNode = memoryPool->allocate_node(node_element, memoryPool->allocate_string("File"));

  string filePath(path);
  if(filePath[filePath.size()-1] != '/') {
    filePath.append("/");
  }
  filePath.append(file);
  
  char *attributeValue = memoryPool->allocate_string(file.c_str());
  fileNode->append_attribute(memoryPool->allocate_attribute("name", attributeValue));
  attributeValue = memoryPool->allocate_string(path.c_str());
  fileNode->append_attribute(memoryPool->allocate_attribute("path", attributeValue));

  struct stat statBuff;
  if (stat(filePath.c_str(), &statBuff) < 0) {
    cerr << __FILE__ << __LINE__ << "Couldn't stat file: " << filePath << endl;
    return fileNode;
  }

  if ((statBuff.st_mode & S_IFMT) == S_IFREG) attributeValue = memoryPool->allocate_string("file");
  else if ((statBuff.st_mode & S_IFMT) == S_IFDIR) attributeValue = memoryPool->allocate_string("directory");
  else if ((statBuff.st_mode & S_IFMT) == S_IFLNK) attributeValue = memoryPool->allocate_string("symbolic");
  else  attributeValue = memoryPool->allocate_string("other");
  fileNode->append_attribute(memoryPool->allocate_attribute("type", attributeValue));

  
//  char buffer[64];
//  stringstream stringStream;
//  string str;
//
//  int permissions = 0;
//  if(statBuff.st_mode & S_IRUSR) permissions |= 0x400;
//  if(statBuff.st_mode & S_IWUSR) permissions |= 0x200;
//  if(statBuff.st_mode & S_IXUSR) permissions |= 0x100;
//  if(statBuff.st_mode & S_IRGRP) permissions |= 0x040;
//  if(statBuff.st_mode & S_IWGRP) permissions |= 0x020;
//  if(statBuff.st_mode & S_IXGRP) permissions |= 0x010;
//  if(statBuff.st_mode & S_IROTH) permissions |= 0x004;
//  if(statBuff.st_mode & S_IWOTH) permissions |= 0x002;
//  if(statBuff.st_mode & S_IXOTH) permissions |= 0x001;
//  stringStream << permissions;
//  stringStream >> str;
//  attributeValue = memoryPool->allocate_string(str.c_str());
//  fileNode->append_attribute(memoryPool->allocate_attribute("permissions", attributeValue));
  
//  stringStream << statBuff.st_nlink;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("links", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_uid;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("userId", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_gid;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("groupId", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_size;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("size", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_atime;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("accessTime", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_mtime;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("modifiedTime", memoryPool->allocate_string(str.c_str())));
//
//  stringStream << statBuff.st_ctime? statBuff.st_ctime: statBuff.st_mtime;
//  stringStream >> str;
//  fileNode->append_attribute(memoryPool->allocate_attribute("createdTime", memoryPool->allocate_string(str.c_str())));

//  statBuff.st_dev      device
//  statBuff.st_ino      inode
//  statBuff.st_rdev     deviceType
//  statBuff.st_blksize  blockSize

  return fileNode;
}

int FileSystem::dirList(const string &dirPath, vector<string> &list)
{
  DIR *directory;
  struct dirent *direntBuffer;

  directory = opendir(dirPath.c_str());
  if(!directory) {
    cerr << "Couldn't open directory: " << dirPath << endl;
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

