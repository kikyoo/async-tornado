import os
import logging
import traceback

import tornado
import tornado.options
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
        try:
            a=100/0
        except Exception,e:
            self.set_status(500)
            log_str = str(self.request)
            log_str += ", ({body='" + self.request.body + "'})"
            log_str += os.linesep + traceback.format_exc()
            logging.error(log_str)


class App(Application):
    def log_func(self, handler):
        if handler.get_status() < 400:
            log_method = logging.info
        elif handler.get_status() < 500:
            log_method = logging.warning
        else:
            log_method = logging.error
        request_time = 1000.0 * handler.request.request_time()
        log_method("%d %s %.2fms", handler.get_status(),
                  handler._request_summary(), request_time)

    def __init__(self, **config):
        handlers = [
            (r'/hello', HelloHandler),
            (r'/world', WorldHandler),
        ]
        settings = {
            'log_function': self.log_func,
            'route': config['route']
        }
        Application.__init__(self, handlers, **settings)

def start_http(port, **config):
    global application
    io_loop = tornado.ioloop.IOLoop.instance()
    application = App(**config)
    http_server = tornado.httpserver.HTTPServer(application, io_loop = io_loop)
    http_server.listen(port)
    io_loop.start()


def stop_http(signal, frame):
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
