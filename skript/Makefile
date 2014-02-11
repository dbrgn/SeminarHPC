#
# Makefile zum Skript ueber High Performance Computing
#
# (c) 2013 Prof Dr Andreas Mueller, Hochschule Rapperswil
#

TEXFILES = vorwort.tex einleitung.tex maschinen.tex sprachen.tex \
	daten.tex algorithm.tex  \
	sample/sample.tex

skript.pdf:	skript.tex $(TEXFILES) skript.ind 
	pdflatex skript.tex
	bibtex skript

skript.ind:	skript.tex $(TEXFILES)
	touch skript.ind
	pdflatex skript.tex
	makeindex skript.idx

#	references.bib
