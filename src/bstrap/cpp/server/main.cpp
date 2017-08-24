#include <unistd.h>
#include <iostream>
#include <Python.h>
#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>

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

#include "server.hpp"

using namespace ::apache::thrift;  
using namespace ::apache::thrift::server;  
using namespace ::apache::thrift::protocol;  
using namespace ::apache::thrift::transport;  
using namespace ::apache::thrift::concurrency;  

using namespace kikyoo;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  int port = 9090;
  bool daemon = false;
  std::string cpy_path;
  std::string srv_path;
  std::string pylib_path;
  std::string conf_file;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h",      "print help message")
    //("daemon",      "run as system daemon")
    ("port",        po::value<int>(&port)->default_value(port), "port number to listen")
    ("cpy_path",    po::value<std::string>(&cpy_path)->default_value(cpy_path), "path to python module cpython")
    ("srv_path",    po::value<std::string>(&srv_path)->default_value(srv_path), "path to python module server")
    ("pylib_path",  po::value<std::string>(&pylib_path)->default_value(srv_path), "path to python module pylib")
    ("conf_file",   po::value<std::string>(&conf_file)->default_value(conf_file), "config file for python module server");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return -1;
  }
  if (cpy_path.empty() || srv_path.empty() || conf_file.empty()) {
    std::cout << desc << std::endl;
    return -1;
  }

  //if (vm.count("daemon")) {
  //  std::cout << "run in daemon" << std::endl;
  //  ::daemon(1, 1);
  //}

  auto factory = boost::make_shared<RouteServiceCloneFactory>(port, cpy_path, srv_path, pylib_path, conf_file);
  TThreadedServer server(
    boost::make_shared<RouteServiceProcessorFactory>(factory),
    boost::make_shared<TServerSocket>(port),
    boost::make_shared<TBufferedTransportFactory>(),
    boost::make_shared<TBinaryProtocolFactory>());

  std::cout << "Starting the server..." << std::endl;
  server.serve();
  std::cout << "Done." << std::endl;

  return 0;
}
