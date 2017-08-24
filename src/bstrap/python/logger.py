#coding=utf-8
'''
Created on 2017年5月10日

@author: cynical 
'''
import os
import sys
import time
import logging
import logging.handlers


class Singleton(type):  
  def __init__(cls, name, bases, dict):  
    super(Singleton, cls).__init__(name, bases, dict)  
    cls.__instance = None  
    
  def __call__(cls, *args, **kw):  
    if cls.__instance is None:
      cls.__instance = super(Singleton, cls).__call__(*args, **kw)  
    return cls.__instance 

class EncodingFormatter(logging.Formatter):
  def __init__(self, fmt, datefmt=None, encoding=None):
    logging.Formatter.__init__(self, fmt, datefmt)
    self.__encoding = encoding

  def format(self, record):
    result = logging.Formatter.format(self, record)
    if isinstance(result, unicode):
      result = result.encode(self.__encoding)
    return result

class RotatingFileHandler(logging.handlers.BaseRotatingHandler):
  def __init__(self, log_path, pattern, mode='a', encoding=None, delay=0):
    self.__log_path = log_path
    self.__pattern = pattern
    logging.handlers.BaseRotatingHandler.__init__(self, log_path + os.sep + pattern(), mode, encoding, delay)

  def doRollover(self):
    self.baseFilename = self.__log_path + os.sep + self.__pattern()
    if self.stream:
      self.stream.close()
      self.stream = None
    if not self.delay:
      self.stream = self._open()

  def shouldRollover(self, record):
    return self.baseFilename != self.__log_path + os.sep + self.__pattern()

class Logger(object):
  __metaclass__ = Singleton

  logger = logging.getLogger('root') 
  logger.setLevel(logging.DEBUG)

  def __init__(self, **conf):
    self.__init_logger(**conf)

  def __init_logger(self, **conf):
    _levelNames = set(['DEBUG', 'INFO', 'WARN', 'ERROR'])
    if conf.get('STREAM_LEVEL') in _levelNames:
      self.__add_stdout_handler(conf['STREAM_LEVEL'], conf['format'])
    if conf.get('FILE_LEVEL') in _levelNames:
      self.__add_file_handler(conf['FILE_LEVEL'], conf['format'], conf['dir'], conf['name'], conf['suffix'])
    if conf.get('SMTP_LEVEL') in _levelNames: 
      self.__add_smtp_handler(conf['SMTP_LEVEL'], conf['format'], **conf['mail'])

  def __add_stdout_handler(self, level, format):
    sh = logging.StreamHandler(sys.stdout)
    sh.setLevel(level)
    sh.setFormatter(EncodingFormatter(format, "%y%m%d %H:%M:%S", encoding = 'utf-8'))
    Logger.logger.addHandler(sh)

  def __add_file_handler(self, level, format, log_dir, name, suffix):
    if not os.path.exists(log_dir):
      os.makedirs(log_dir)
    name_pattern = lambda: name + time.strftime('_%Y-%m-%d_') + str(suffix) + '.log'
    fh = RotatingFileHandler(log_dir, name_pattern)
    fh.setLevel(level)
    fh.setFormatter(EncodingFormatter(format, "%y%m%d %H:%M:%S", encoding = 'utf-8'))
    Logger.logger.addHandler(fh)

  def __add_smtp_handler(self, level, format, **mail):
    smtp = logging.handlers.SMTPHandler(mailhost=mail['mailhost'],
                       credentials=mail['credentials'],
                       fromaddr=mail['fromaddr'],
                       toaddrs=mail['toaddrs'],
                       subject=mail['subject'])
    smtp.setLevel(level)
    smtp.setFormatter(EncodingFormatter(format, "%y%m%d %H:%M:%S", encoding = 'utf-8'))
    Logger.logger.addHandler(smtp)

  @staticmethod
  def instance(**conf):
    logger = Logger(**conf).logger
    return logger


if __name__ == '__main__':
  conf = {
    'dir' : '/home/wangxiu/program/async-tornado/log',
    'name': 'async-tornado',
    'suffix': '8888', #{PORT}
    'format': '[%(levelname)1.1s %(asctime)s %(name)s %(module)s:%(lineno)d] %(message)s',
    'STREAM_LEVEL'  : 'DEBUG', #only when daemon=False
    'FILE_LEVEL'    : 'INFO',
    'SMTP_LEVEL'    : 'NONE',
  }
  logger = Logger.instance(**conf)
  for i in xrange(10):
    logger.debug('debug!')
    logger.info('info!')
    logger.warn('warn!')
    logger.error('error!')
    time.sleep(1)
