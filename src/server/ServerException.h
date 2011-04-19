#ifndef SERVEREXCEPTION_H
#define SERVEREXCEPTION_H

#include <exception>
#include <string>

class ServerException : public std::exception {
public:
  ServerException(const char *message) throw() : m_Message(message) {}
  ~ServerException() throw() {}
  virtual const char *what() const throw() { return m_Message.c_str(); }
private:
  std::string m_Message;
};

#endif
