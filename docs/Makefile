FILE=doc

all: $(FILE).tex
	pdflatex $<;
	pdflatex $<

.PHONY: clean
clean:
	rm -f *.aux *.out *.log *.dvi *.ps *.pdf *.toc
