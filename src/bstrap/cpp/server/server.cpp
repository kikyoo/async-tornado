#include <unistd.h>

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>

#include <iostream>

#include "server.hpp"  
#include "cpython.hpp"


namespace kikyoo {

void RouteServiceHandler::ping(std::string& _return, const std::string& ping) {
  _return.assign("pong\n");
}

void RouteServiceHandler::call(Rslt& _return, const Msg& msg) {
  if (MsgType::H_CALL & msg.type) {
    _return.status = (RsltType::type)m_pobj_->hash_call(_return.name,
        _return.msg,
        msg.key,
        msg.name,
        msg.msg);
  } else {
    _return.status = (RsltType::type)m_pobj_->module_call(_return.name,
        _return.msg,
        msg.name,
        msg.msg);
  }
  _return.status? false: _return.__isset.name = _return.__isset.msg = true;
}

void RouteServiceHandler::message(const Msg& msg) {
  if (MsgType::H_MSG & msg.type) {
    m_pobj_->hash_msg(msg.key, msg.name, msg.msg);
  } else {
    m_pobj_->module_msg(msg.name, msg.msg);
  }
}

RouteServiceCloneFactory::RouteServiceCloneFactory(const std::string& cpy_path,
  const std::string& srv_path,
  const std::string& conf_file) {

  Py_Initialize(); 

  m_pobj_ = boost::make_shared<CPython>();
  if (!m_pobj_->import(cpy_path, srv_path, conf_file)) {
    abort();
  }
}

RouteServiceCloneFactory::~RouteServiceCloneFactory() {
  Py_Finalize();
}

}

