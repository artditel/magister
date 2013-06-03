tex:
	./totex.sh
compile:
	cd methods && make all
analyze:
	./analyze
clean:
	find data -name *tex -delete
	cd methods && make clean
