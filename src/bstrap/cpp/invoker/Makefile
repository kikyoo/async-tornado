.PHONY:all clean

TARGET=../../python/kikyoo.so
all:$(TARGET)

CXXFLAGS=-std=c++11 -g -O3 -w -fPIC
LDFLAGS=-fPIC -shared -g -O3 -L../../../../cpplib/thrift/lib -L../../../../cpplib/boost/lib -L../../../../cpplib/openssl/lib -L../../../../cpplib/libevent/lib
INCLUDES=-I/usr/include/python2.7  -I../../../../cpplib/boost/include/ -I../../../../cpplib/thrift/include/ -I../common/gen-cpp/ 
LIBS=-lthrift -lpthread -lpython2.7 -lboost_python

CXXFLAGS += $(INCLUDES)

THR_DIR=../common/gen-cpp
THR_SRC=$(wildcard $(THR_DIR)/*.cpp)  
MAN_SRC=$(wildcard *.cpp)  
SRC=$(THR_SRC) $(MAN_SRC)
OBJ=$(patsubst %.cpp, %.o, $(notdir $(SRC))) 

$(TARGET):$(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: $(THR_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	$(RM) $(OBJ) $(TARGET)
