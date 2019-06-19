source	=$(wildcard *.cpp)
object	=$(patsubst %.cpp, %.o, $(source))
target	= server
CFLAGS	= -std=c++11 -I/usr/local/mycpp/include -Wall -Winline -g
LDFLAGS	= -lrt -luuid -lpthread -lssl -lodbc -ljsoncpp -L /usr/local/mycpp -lmycpp

all: $(object)
	g++ $(CFLAGS) $(object) -o $(target) $(LDFLAGS) $(LIBS)
	-rm core* -rvf

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-rm -rvf core* $(object) $(target)
