Dserver:Dserver.o sock.o epoll.o http.o rio.o http_error.o http_out_filter.o thread_pool.o
	g++ -Wall -std=c++11 -o Dserver sock.o Dserver.o epoll.o http.o rio.o http_error.o http_out_filter.o thread_pool.o \
	-g -lpthread
Dserver.o:Dserver.cpp debug.h sock.h
	g++ -Wall -std=c++11 Dserver.cpp -c -g -lpthread
sock.o:sock.cpp sock.h debug.h
	g++ -Wall -std=c++11 sock.cpp -c -g -lpthread
epoll.o:epoll.cpp debug.h
	g++ -Wall -std=c++11 epoll.cpp -c -g -lpthread
http.o:http.cpp rio.h http_data_structure.h http_error.h http_out_filter.h
	g++ -Wall -std=c++11 http.cpp -c -g -lpthread
rio.o:rio.cpp
	g++ -Wall -std=c++11 rio.cpp -c -g -lpthread
http_error.o:http_error.cpp http_data_structure.h debug.h
	g++ -Wall -std=c++11 http_error.cpp -c -g -lpthread
http_out_filter.o:http_out_filter.cpp http_data_structure.h
	g++ -Wall -std=c++11 http_out_filter.cpp -c -g -lpthread
thread_pool.o:thread_pool.cpp debug.h
	g++ -Wall -std=c++11 thread_pool.cpp -c -g -lpthread
clean:
	rm -rf Dserver.o sock.o epoll.o http.o rio.o http_error.o http_out_filter.o thread_pool.o

# -std=c++11 there is no space