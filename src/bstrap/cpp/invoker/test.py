import kikyoo
conf={'timeout':10, 'queue_size':10000, 'user':['localhost:9090', 'localhost:9091'], 'xxx':['localhost:9092']}
inv=kikyoo.Invoker(conf)
