import os
import sys

bin_path = os.path.dirname(os.path.realpath(__file__))
pro_path = os.path.split(bin_path)[0]
src_path = os.path.join(pro_path, 'src')
conf_path = os.path.join(pro_path, 'conf')
pylib_path = os.path.join(pro_path, 'pylib')
cpplib_path = os.path.join(pro_path, 'cpplib')

srv_path = os.path.join(src_path, 'bstrap/cpp/server/server')

sys.path.extend([pylib_path, src_path])

config = {}
execfile(os.path.join(conf_path, "user.conf"), config)

ld_path ='%s:%s:%s' %(os.path.join(cpplib_path, 'boost/lib'), os.path.join(cpplib_path, 'openssl/lib'), os.path.join(cpplib_path, 'thrift/lib'))
os.environ['LD_LIBRARY_PATH'] = ld_path

option = '--port=%s --conf_file=%s --cpy_path=%s --srv_path=%s' %(config['listen_port'][0], os.path.join(conf_path, 'user.conf'), os.path.join(src_path, 'bstrap/python'), os.path.join(src_path, 'user'))
print option

os.system(srv_path + ' ' + option)

