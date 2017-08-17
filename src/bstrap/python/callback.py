import kikyoo
import tornado
import greenlet
import tornado.ioloop
from functools import partial
from exception import *
import route_pb2 as pb


class Invoker(object):
    class Callback(object):
        def __init__(self, grt):
            self.grt = grt
        
        def cb(self, status, name, msg):
            tornado.ioloop.IOLoop.instance().add_callback(partial(self.grt.switch, status, name, msg))

    def __init__(self, **conf):
        self.sdb = pb._sym_db
        self.invoker = kikyoo.Invoker(conf)

    def __parse_ret(self, status, name, msg):
        if pb.STATUS_OK == status:
            cls = self.sdb.GetSymbol(name)()
            cls.ParseFromString(msg)
            return cls
            
        elif pb.MSG_FORMAT == status:
            raise MsgFormat('pb msg format error, name: %s' %name)
        elif pb.RPC_TIMEOUT == status:
            raise RPCTimeout('rpc call timeout, name: %s' %name)
        elif pb.NO_METHOD == status:
            raise NoMethod('server has no compatible method, name: %s' %name)
        else:
            raise

    def hash_call(self, server, key, msg):
        msg_ = msg.SerializeToString()
        name = msg.DESCRIPTOR.full_name
        grt = greenlet.getcurrent()
        cb = Invoker.Callback(grt)
        self.invoker.hash_call(cb, server, str(key), name, msg_)
        status, name, msg = grt.parent.switch()
        return self.__parse_ret(status, name, msg)
        
    def module_call(self, server, msg):
        msg_ = msg.SerializeToString()
        name = msg.DESCRIPTOR.full_name
        grt = greenlet.getcurrent()
        cb = Invoker.Callback(grt)
        self.invoker.module_call(cb, server, name, msg_)
        status, name, msg = grt.parent.switch()
        return self.__parse_ret(status, name, msg)

    def hash_msg(self, server, key, msg):
        msg_ = msg.SerializeToString()
        name = msg.DESCRIPTOR.full_name
        self.invoker.hash_msg(server, str(key), name, msg_)

    def module_msg(self, server, msg):
        msg_ = msg.SerializeToString()
        name = msg.DESCRIPTOR.full_name
        self.invoker.module_msg(server, name, msg_)

    
