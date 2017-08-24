import os
import sys
import signal

bin_path = os.path.dirname(os.path.realpath(__file__))
pro_path = os.path.split(bin_path)[0]
src_path = os.path.join(pro_path, 'src')
conf_path = os.path.join(pro_path, 'conf')
pylib_path = os.path.join(pro_path, 'pylib')
cpplib_path = os.path.join(pro_path, 'cpplib')

sys.path.extend([pylib_path, src_path])


def setup_log(**conf):
    import logging
    import bstrap
    import bstrap.python
    from bstrap.python import Logger as logger
  
    logger = logger.instance(**conf)
    logging.debug = logger.debug
    logging.info  = logger.info
    logging.warn  = logger.warn
    logging.error = logger.error

import daemon
class TDaemon(daemon.Daemon):
    def __init__(self, pid, loop, **conf):
        daemon.Daemon.__init__(self, pid)
        self.conf = conf
        self.loop = loop
  
    def restart(self, *args, **kwargs):
        self.stop(**self.conf)
        self.start(**self.conf)
  
    def run(self, *args, **kwargs):
        self.loop(**self.conf)

pids = {} 
def stop_http(signo, frame):
    for pid in pids:
        os.kill(pid, signal.SIGTERM)

def start_http(**conf):
    for port in conf['listen_port']:
        pid = os.fork()
        if pid != 0:
            pids[pid] = port
        else:
            log = conf['log']
            log['suffix'] = str(port) 
            setup_log(**log)

            import http
            import http.server as server
            server.start_http(port, **conf)

    signal.signal(signal.SIGTERM, stop_http)

    while pids:
        try:
            pid, status = os.wait()
            port = pids.pop(pid)
            if status > 0 and status != 256:
                pid = os.fork()
                if pid != 0:
                    pids[pid] = port
                else:
                    log = conf['log']
                    log['suffix'] = str(port) 
                    setup_log(**log)
                    
                    import http
                    import http.server as server
                    signal.signal(signal.SIGTERM, server.stop_http)
                    server.start_http(port, **conf)
        except: pass


if __name__ == '__main__':
    if 'LD_LIBRARY_PATH' not in os.environ or -1 == os.environ['LD_LIBRARY_PATH'].find(os.path.join(cpplib_path, 'thrift/lib')):
        ld_path ='%s:%s:%s' %(os.path.join(cpplib_path, 'boost/lib'), os.path.join(cpplib_path, 'openssl/lib'), os.path.join(cpplib_path, 'thrift/lib'))
        os.environ['LD_LIBRARY_PATH'] = ld_path
        os.execvp('python', ('python', sys.argv[0], sys.argv[1]))

    conf = {}
    execfile(os.path.join(conf_path, 'http.conf'), conf)
    pid = conf['log']['dir'] + os.sep + 'http.pid'
    if True == conf.get('daemon'):
        conf['log']['STREAM_LEVEL'] = 'NONE' #neccesary?
        daemon = TDaemon(pid, start_http, **conf)
        getattr(daemon, sys.argv[1])(**conf) 
    else:
        start_http(**conf)

