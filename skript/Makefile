#
# Makefile zum Skript ueber High Performance Computing
#
# (c) 2013 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
SAMPLEFILES = sample/sample.tex sample/sample.bib

TEXFILES = vorwort.tex einleitung.tex maschinen.tex sprachen.tex \
	daten.tex algorithm.tex $(SAMPLEFILES)


skript.pdf:	skript.tex $(TEXFILES) skript.ind  skript.bbl
	pdflatex skript.tex
	bibtex skript

skript.ind:	skript.tex $(TEXFILES)
	touch skript.ind
	pdflatex skript.tex
	makeindex skript.idx

#	references.bib
skript.bbl:		skript.aux
	bibtex skript
