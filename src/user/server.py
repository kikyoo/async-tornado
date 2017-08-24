import logging
import traceback
import route_pb2 as pb
from cpython import CPython
#import redis

class Server(CPython):
    def __init__(self, *args):
        #load config file
        #do sth. like self.redis=redis.Redis('localhost')
        print args
        conf = {}
        execfile(args[1], conf)
        self._setup_log(args[0], **conf)

    def recv_module_call_Ping(self, ping):
        pong = pb.Pong()
        pong.id = ping.id
        pong.pong = 'pong!'
        return pong

    def recv_hash_call_Ping(self, key, ping):
        logging.info('%s' %ping)
        pong = pb.Pong()
        pong.id = ping.id
        pong.pong = 'pong!!'
        return pong

    def recv_module_msg_Ping(self, ping):
        pass

    def recv_hash_msg_Ping(self, key, ping):
        pass

    def _setup_log(self, port, **conf):
        import logger
        conf['log']['suffix'] = str(port)
        logger = logger.Logger.instance(**conf['log'])
        logging.debug = logger.debug
        logging.info  = logger.info
        logging.warn  = logger.warn
        logging.error = logger.error


