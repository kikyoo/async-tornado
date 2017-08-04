import tornado
import tornado.web
import tornado.ioloop
import tornado.httpserver

import bstrap
import bstrap.python
import bstrap.python.route_pb2 as pb
from bstrap.python import Application, BaseHandler

application = None

class HelloHandler(BaseHandler):
    def do_GET(self):
        ping = pb.Ping()
        ping.id = 1
        ping.ping = 'ping'
        self.module_msg('user', ping)
        ping.id = 2
        self.hash_msg('user', 'key1', ping)
        ping.id = 3
        pong1 = self.module_call('user', ping)
        ping.id = 4
        pong2 = self.hash_call('user', 'key2', ping)
        self.write(pong1.pong + '\t' + pong2.pong)

class WorldHandler(BaseHandler):
    def do_POST(self):
        self.write('post handler')


class App(Application):
    def __init__(self, **config):
        handlers = [
            (r'/hello', HelloHandler),
            (r'/world', WorldHandler),
        ]
        settings = {}
        Application.__init__(self, handlers, **settings)

def start_http(idx, **config):
    global application
    io_loop = tornado.ioloop.IOLoop.instance()
    application = App(**config)
    http_server = tornado.httpserver.HTTPServer(application, io_loop = io_loop)
    http_server.listen(config['listen_port'][idx])
    io_loop.start()


def stop_http():
    application.stop()


if __name__ == '__main__':
    config = {
        'listen_port': [8888,],
        'server': {
            'timeout': 10,
            'user': ['localhost:9090',]
        }
    }

    start_http(0, config)
