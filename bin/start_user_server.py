import os
import sys
import time
import signal
import subprocess

bin_path = os.path.dirname(os.path.realpath(__file__))
pro_path = os.path.split(bin_path)[0]
src_path = os.path.join(pro_path, 'src')
conf_path = os.path.join(pro_path, 'conf')
pylib_path = os.path.join(pro_path, 'pylib')
cpplib_path = os.path.join(pro_path, 'cpplib')

sys.path.extend([pylib_path, src_path])
srv_path = os.path.join(src_path, 'bstrap/cpp/server/server')

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
def stop_user(signo, frame):
    for pid in pids.keys():
        pid.terminate()
        pids.pop(pid)

def start_user(**conf):
    for port in conf['listen_port']:
        option = '--port=%s --conf_file=%s --cpy_path=%s --srv_path=%s --pylib_path=%s' %(port, os.path.join(conf_path, 'user.conf'), os.path.join(src_path, 'bstrap/python'), os.path.join(src_path, 'user'), pylib_path)
        args = []
        args.append(srv_path)
        args.extend(option.split())
        pids[subprocess.Popen(args)] = port

    signal.signal(signal.SIGINT,  stop_user)
    signal.signal(signal.SIGTERM, stop_user)

    while pids:
        time.sleep(1)
        for pid in pids:
            if None != pid.poll():
                port = pids[pid]
                option = '--port=%s --conf_file=%s --cpy_path=%s --srv_path=%s --pylib_path=%s' %(port, os.path.join(conf_path, 'user.conf'), os.path.join(src_path, 'bstrap/python'), os.path.join(src_path, 'user'), pylib_path)
                args = []
                args.append(srv_path)
                args.extend(option.split())
                pids[subprocess.Popen(args)] = port


if __name__ == '__main__':
    if 'LD_LIBRARY_PATH' not in os.environ or -1 == os.environ['LD_LIBRARY_PATH'].find(os.path.join(cpplib_path, 'thrift/lib')):
        ld_path ='%s:%s:%s' %(os.path.join(cpplib_path, 'boost/lib'), os.path.join(cpplib_path, 'openssl/lib'), os.path.join(cpplib_path, 'thrift/lib'))
        os.environ['LD_LIBRARY_PATH'] = ld_path
        os.execvp('python', ('python', sys.argv[0], sys.argv[1]))

    conf = {}
    execfile(os.path.join(conf_path, 'user.conf'), conf)
    pid = conf['log']['dir'] + os.sep + 'user.pid'
    if True == conf.get('daemon'):
        conf['log']['STREAM_LEVEL'] = 'NONE' #neccesary?
        daemon = TDaemon(pid, start_user, **conf)
        getattr(daemon, sys.argv[1])(**conf) 
    else:
        start_user(**conf)

