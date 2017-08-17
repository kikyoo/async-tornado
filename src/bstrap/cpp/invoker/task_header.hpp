#ifndef HEADER_H_
#define HEADER_H_

#include <string>
#include <Python.h>
#include <boost/shared_ptr.hpp>

#include "route_types.h"
#include "RouteService.h"  
#include "event_buffer.hpp"
#include "fifo_queue.hpp"
using namespace apache::thrift::concurrency;

namespace kikyoo {

struct Rpc {
  PyObject*     pobj;
  std::string   server;
  std::string   key;
  std::string   name;
  std::string   request;
  MsgType::type type;
  int64_t       birth_time;
  int64_t       hash_key;

  Rpc()
    : pobj(NULL)
    , hash_key(0)
    , birth_time(0)
    , type(MsgType::M_MSG){}
};
typedef boost::shared_ptr<Rpc> RpcPtr;

std::ostream& operator << (std::ostream &os, const RpcPtr rpc);

struct Callback {
  PyObject*      pobj;
  std::string    name;
  std::string    response;
  RsltType::type type;
  Callback()
    : pobj(NULL)
    , type(RsltType::RPC_TIMEOUT) {}
};
typedef boost::shared_ptr<Callback> CallbackPtr;
std::ostream& operator << (std::ostream &os, const CallbackPtr cb);

struct Node {
  enum State{
    UNINITIALIZED,
    CONNECTED,
    CORRUPTED
  };
  int seq_id;
  std::string server;
  std::string host;
  int port;
  int state;
  int64_t reconnect_times;
  int64_t last_reconnect_time;
  boost::shared_ptr<RouteServiceClient> client_ptr;

  Node()
    : seq_id(0) 
    , port(0)
    , state(UNINITIALIZED)
    , reconnect_times(0)
    , last_reconnect_time(0) {}
};
typedef boost::shared_ptr<Node> NodePtr;
std::ostream& operator << (std::ostream &os, const NodePtr node);

//typedef some useful FifoQueue
typedef boost::shared_ptr<EventBuffer<NodePtr>> EventListType;
typedef boost::shared_ptr<FifoQueue<RpcPtr>> RequestQueueType;//single reader, single writer
typedef boost::shared_ptr<FifoQueue<CallbackPtr, NullMutex, Mutex>> CbQueueType;//single reader, mul writer

typedef boost::shared_ptr<std::vector<RequestQueueType>> InvokerQueueType;
}
#endif

