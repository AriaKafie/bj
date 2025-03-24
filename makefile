all:
	g++ *cpp -o bj
debug:
	g++ -g *cpp -o debug
clean:
	rm *~ bj
