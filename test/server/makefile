source	=$(wildcard *.cpp)
object	=$(patsubst %.cpp, %.o, $(source))
target	= server
CFLAGS	= -std=c++11 -I/root/mycpplib/ -Wall -Winline -g
LDFLAGS	= -lrt -luuid -lpthread -lssl -lodbc -ljsoncpp -L /root/mycpplib -lmycpp

all: $(object)
	g++ $(CFLAGS) $(object) -o $(target) $(LDFLAGS) $(LIBS)
	echo "" > nohup.out
	-rm core* -rvf

%.o: %.cpp
	g++ -c $< -o $@ $(CFLAGS)

.PHONY:clean
clean:
	-rm -rvf core* $(object) $(target)
	-echo "" > nohup.out
