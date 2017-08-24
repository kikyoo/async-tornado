import callback
import tornado
import tornado.web

class Application(tornado.web.Application):
    def __init__(self, handlers=None, default_host="", transforms=None, **settings):
        tornado.web.Application.__init__(self, handlers, default_host, transforms, **settings)
        self.invoker = callback.Invoker(**settings['route'])

    def stop(self):
        self.invoker.stop()

