import route_pb2 as pb
from cpython import CPython
#import redis

class Server(CPython):
    def __init__(self, *args):
        #load config file
        #do sth.
        '''print args
        config = {}
        execfile(args[0], config)
        self.redis=redis.Redis('localhost')'''
        pass

    def recv_module_call_Ping(self, ping):
        pong = pb.Pong()
        pong.id = ping.id
        pong.pong = 'pong!'
        return pong

    def recv_hash_call_Ping(self, key, ping):
        pong = pb.Pong()
        pong.id = ping.id
        pong.pong = 'pong!!'
        return pong

    def recv_module_msg_Ping(self, ping):
        pass

    def recv_hash_msg_Ping(self, key, ping):
        pass

