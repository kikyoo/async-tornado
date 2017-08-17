#include "task_header.hpp"

namespace kikyoo {

std::ostream& operator << (std::ostream &os, const RpcPtr rpc) {
  os << "RpcPtr {";
  os << "pobj=" << rpc->pobj << ", ";
  os << "server=" << rpc->server << ", ";
  os << "key=" << rpc->key << ", ";
  os << "name=" << rpc->name << ", ";
  os << "request=" << rpc->request << ", ";
  os << "type=" << rpc->type << ", ";
  os << "birth_time=" << rpc->birth_time << ", ";
  os << "hash_key=" << rpc->hash_key;
  os << "}";
  return os;
}

std::ostream& operator << (std::ostream &os, const CallbackPtr cb) {
  os << "CallbackPtr {";
  os << "pobj=" << cb->pobj << ", ";
  os << "name=" << cb->name << ", ";
  os << "response=" << cb->response << ", ";
  os << "type=" << cb->type;
  os << "}";
  return os;
}

std::ostream& operator << (std::ostream &os, const NodePtr node) {
  os << "NodePtr {";
  os << "seq_id=" << node->seq_id << ", "; 
  os << "server=" << node->server << ", "; 
  os << "host=" << node->host << ", "; 
  os << "port=" << node->port << ", "; 
  os << "state=" << node->state << ", "; 
  os << "reconnect_times=" << node->reconnect_times << ", "; 
  os << "last_reconnect_time=" << node->last_reconnect_time << ", "; 
  os << "client_ptr=" << node->client_ptr; 
  os << "}";
  return os;
}

}

