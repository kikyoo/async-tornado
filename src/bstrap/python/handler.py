import async
import application

import tornado
import tornado.web

class BaseHandler(tornado.web.RequestHandler):
    def __init__(self, application, request, **kwargs):
        tornado.web.RequestHandler.__init__(self, application, request, **kwargs)
        self.invoker = application.invoker

        self.module_call = self.invoker.module_call
        self.module_msg  = self.invoker.module_msg
        self.hash_call   = self.invoker.hash_call
        self.hash_msg    = self.invoker.hash_msg

    @async.async
    def post(self):
        self.do_POST()
        self.finish()

    @async.async
    def get(self):
        self.do_GET()
        self.finish()


    def do_GET(self):
        raise NotImplementedError()

    def do_POST(self):
        raise NotImplementedError()

