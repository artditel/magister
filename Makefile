tex: gentex
	mkdir -p genfiles
	pdflatex main.tex -output-directory=geniles
gentex: $(wildcard data/*json)
	./totex.sh
compile:
	cd methods && make all
analyze:
	./analyze
clean:
	find data | grep tex | xargs rm
	cd methods && make clean
	rm -rf genfiles
	rm -f *aux *log *out *toc
