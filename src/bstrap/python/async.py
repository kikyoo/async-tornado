import tornado
import tornado.web
import greenlet
from functools import wraps

def async(method):
    @tornado.web.asynchronous
    @wraps(method)
    def wrapper(self, *args, **kwargs):
        def greenlet_func():
            method(self, *args, **kwargs)

        gr = greenlet.greenlet(greenlet_func)
        gr.switch()

    return wrapper

