#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "RouteService.h"  

using namespace ::apache::thrift;  
using namespace ::apache::thrift::protocol;  
using namespace ::apache::thrift::transport;  
using namespace ::apache::thrift::concurrency;  

namespace kikyoo {
class CPython;
typedef boost::shared_ptr<CPython> CPythonPtr;

class RouteServiceHandler : virtual public RouteServiceIf {
public:
  RouteServiceHandler(CPythonPtr pobj)
  : m_pobj_(pobj) {
  }

  void ping(std::string& _return, const std::string& ping);

  void call(Rslt& _return, const Msg& msg);

  void message(const Msg& msg);

private:
  CPythonPtr m_pobj_;
};

class RouteServiceCloneFactory : virtual public RouteServiceIfFactory {
public:
  RouteServiceCloneFactory(const std::string& cpy_path,
    const std::string& srv_path,
    const std::string& pylib_path,
    const std::string& conf_file);

  ~RouteServiceCloneFactory();

  virtual RouteServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) {
    std::cout << "new handler" << std::endl;
    return new RouteServiceHandler(m_pobj_);
  }
  virtual void releaseHandler(RouteServiceIf* handler) {
    std::cout << "del handler" << std::endl;
    delete handler;
  }
private:
  CPythonPtr m_pobj_;
};
}

#endif

