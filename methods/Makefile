all: predict proof eig
predict: predict.cpp *.h
	g++-4.6 -O2 -g -std=c++0x -lpthread -pedantic \
		-o predict predict.cpp
proof: proof.cpp *.h
	g++-4.6 -O2 -g -std=c++0x -lpthread -pedantic \
		-o proof proof.cpp
eig: eig.cpp *.h
	g++-4.6 -O2 -g -std=c++0x -lpthread -pedantic \
		-I /usr/include/arpack++/ /usr/lib/libarpack++.so \
		-o eig eig.cpp
clean:
	rm -f predict proof eig
