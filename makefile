all:
	g++ crypto.cpp -c -o crypto.o -Wall -Winline -g
	g++ net.cpp -c -o net.o -Wall -Winline -g
	g++ threadpool.cpp -c -o threadpool.o -Wall -Winline -g
	g++ datetime.cpp -c -o datetime.o -Wall -Winline -g
	g++ epoll.cpp -c -o epoll.o -Wall -Winline -g
	ar rcs librui.a *.o

	gcc crypto.cpp net.cpp threadpool.cpp datetime.cpp epoll.cpp -fPIC -shared -o libmycpp.so

.PHONY:clean
clean:
	-rm -rvf *.o libmycpp.a libmycpp.so
