import os
import sys

bin_path = os.path.dirname(os.path.realpath(__file__))
pro_path = os.path.split(bin_path)[0]
src_path = os.path.join(pro_path, 'src')
conf_path = os.path.join(pro_path, 'conf')
pylib_path = os.path.join(pro_path, 'pylib')

sys.path.extend([pylib_path, src_path])

config = {}
execfile(os.path.join(conf_path, "http.conf"), config)
import http
import http.server as server
server.start_http(0, **config)
