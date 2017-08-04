import route_pb2 as pb

class CPython:
    def hash_call(self, key, name, msg):
        status, name_, msg_ = 0, '', ''
        try:
            cls = pb._sym_db.GetSymbol(name)()
            cls.ParseFromString(msg)
            result = getattr(self, 'recv_hash_call_%s' %cls.DESCRIPTOR.name)(key, cls)
            msg_   = result.SerializeToString()
            name_  = result.DESCRIPTOR.full_name
        except AttributeError,e:
            status = pb.NO_METHOD
        except:
            status = pb.MSG_FORMAT
        return status, name_, msg_

    def module_call(self, name, msg):
        status, name_, msg_ = 0, '', ''
        try:
            cls = pb._sym_db.GetSymbol(name)()
            cls.ParseFromString(msg)
            result = getattr(self, 'recv_module_call_%s' %cls.DESCRIPTOR.name)(cls)
            msg_   = result.SerializeToString()
            name_  = result.DESCRIPTOR.full_name
        except AttributeError:
            status = pb.NO_METHOD
        except:
            status = pb.MSG_FORMAT
        return status, name_, msg_

    def hash_msg(self, key, name, msg):
        cls = pb._sym_db.GetSymbol(name)()
        cls.ParseFromString(msg)
        getattr(self, 'recv_hash_msg_%s' %cls.DESCRIPTOR.name)(key, cls)

    def module_msg(self, name, msg):
        cls = pb._sym_db.GetSymbol(name)()
        cls.ParseFromString(msg)
        getattr(self, 'recv_module_msg_%s' %cls.DESCRIPTOR.name)(cls)

