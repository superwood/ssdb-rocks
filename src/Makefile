include ../build_config.mk
SRC_PATH=./src
TEST_PATH=./test

CC_EXEC=ssdb-server

output : all
	cp $(CC_EXEC) ../ 

all : $(CC_EXEC)

clean :
	rm -f ${CC_EXEC} *.o *.exe
	rm util/*.o

CXXFLAGS=$(CFLAGS)
FILTER_FILE=proc_control.cpp proc_hash.cpp proc_kv.cpp proc_queue.cpp proc_zset.cpp link_redis.cpp

SOURCES_ALL=$(wildcard *.cpp)
SOURCES=$(filter-out $(FILTER_FILE), $(SOURCES_ALL))

OBJS=$(patsubst %.cpp,%.o,$(SOURCES))


UTIL_FILTER_FILE=util/fde_epoll.cpp util/fde_select.cpp util/test_sorted_set.cpp
UTIL_SOURCES_ALL=$(wildcard util/*.cpp)
UTIL_SOURCES=$(filter-out $(UTIL_FILTER_FILE), $(UTIL_SOURCES_ALL) )
UTIL_OBJS=$(patsubst %.cpp,%.o, $(UTIL_SOURCES))

$(CC_EXEC) : $(OBJS) $(UTIL_OBJS)
	$(CXX) $(CFLAGS) -o $@ $^ $(CLIBS)  
