all:
	g++ -fpermissive -w bj.cpp -o bj
debug:
	g++ -fpermissive -w -g bj.cpp -o debug
clean:
	rm *~ bj
