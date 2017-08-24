namespace cpp kikyoo

enum MsgType {
  H_CALL = 0x1,
  H_MSG  = 0x2,
  M_CALL = 0x4,
  M_MSG  = 0x8,
  M_STOP = 0x10
}

struct Msg {
  1: MsgType  type,
  2: string   name,
  3: string   msg,
  4: optional string  key
}

enum RsltType {
  STATUS_OK   = 0x0,
  MSG_FORMAT  = 0x1,
  RPC_TIMEOUT = 0x2,
  NO_METHOD   = 0x4,
  M_STOP      = 0x10
}

struct Rslt {
  1: RsltType status,
  2: optional string name,
  3: optional string msg
}

service RouteService {
  Rslt   call(1: Msg msg),
  void   message(1: Msg msg)
}

