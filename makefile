all:
	g++ crypto.cpp -c -o crypto.o -Wall -Winline -g -std=c++11
	g++ net.cpp -c -o net.o -Wall -Winline -g -std=c++11
	g++ threadpool.cpp -c -o threadpool.o -Wall -Winline -g -std=c++11
	g++ datetime.cpp -c -o datetime.o -Wall -Winline -g -std=c++11
	g++ epoll.cpp -c -o epoll.o -Wall -Winline -g -std=c++11
	ar rcs libmycpp.a *.o

	gcc crypto.cpp net.cpp threadpool.cpp datetime.cpp epoll.cpp -fPIC -shared -std=c++11 -o libmycpp.so

.PHONY:clean
clean:
	-rm -rvf *.o libmycpp.a libmycpp.so
