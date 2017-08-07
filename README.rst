x-tornado
=========

Thrift(C++) for network communication, tornado as web server, while greenlet makes (async) rpc look like function call.


GET, POST
---------

Simple example:

.. code-block:: python
    
    route.proto
    
    message Ping {
      required int64 id = 1;
      required string ping = 2;
    }
    
    message Pong {
      required int64 id = 1;
      required string pong = 2;
    }


    user/server.py

    class Server(CPython):
        def __init__(self, *args):
            pass
    
        def recv_module_call_Ping(self, ping):
            pong = pb.Pong()
            pong.id = ping.id + 1
            pong.pong = 'pong!'
            return pong
    
        def recv_hash_call_Ping(self, key, ping):
            pong = pb.Pong()
            pong.id = ping.id + 1
            pong.pong = 'pong!!'
            return pong

        def recv_module_msg_Ping(self, ping):
            pass
    
        def recv_hash_msg_Ping(self, key, ping):
            pass


    http/server.py

    class HelloHandler(BaseHandler):
        def do_GET(self):
            ping = pb.Ping()
            ping.id = 1
            ping.ping = 'ping'
            pong = self.hash_call('user', 'key', ping)
            
            self.write(pong.pong)
    
    class WorldHandler(BaseHandler):
        def do_POST(self):
            self.write('post handler')
    

ToDo
---------
This is only an Alpha version, many things need to do.

* multi rpc-server support
* log system
* performance opt. && stability

