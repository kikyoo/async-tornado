.PHONY: all clean

all:
	../../../cpplib/thrift/bin/thrift -out common/gen-cpp/ -gen cpp common/route.thrift
	$(RM) common/gen-cpp/RouteService_server.skeleton.cpp
	make -C server/
	make -C invoker/

clean:
	make clean -C server/
	make clean -C invoker/
	$(RM) common/gen-cpp/*
