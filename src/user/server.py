import route_pb2 as pb
from cpython import CPython

class Server(CPython):
    def __init__(self, *args):
        #load cnf file
        #do sth.
        print args
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

