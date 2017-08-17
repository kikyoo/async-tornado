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
        import random
        pong = self.hash_call('user', str(random.random()), ping)
        self.write(pong.pong)

class WorldHandler(BaseHandler):
    def do_POST(self):
        self.write('post handler')


class App(Application):
    def __init__(self, **config):
        handlers = [
            (r'/hello', HelloHandler),
            (r'/world', WorldHandler),
        ]
        settings = config['route']
        Application.__init__(self, handlers, **settings)

def start_http(idx, **config):
    global application
    io_loop = tornado.ioloop.IOLoop.instance()
    application = App(**config)
    http_server = tornado.httpserver.HTTPServer(application, io_loop = io_loop)
    http_server.listen(config['listen_port'][idx])
    io_loop.start()


def stop_http():
    tornado.ioloop.IOLoop.instance().stop()
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
