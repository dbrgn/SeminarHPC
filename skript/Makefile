#
# Makefile zum Skript ueber High Performance Computing
#
# (c) 2013 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
all:	subdirs skript.pdf

subdirs:
	for d in graphics julia heat; \
	do \
		cd $${d}; make; cd ..; \
	done


SAMPLEFILES = sample/sample.tex sample/sample.bib
HEATFILES = heat/heat.tex
JULIAFILES = julia/julia.tex julia/julia.bib

TEXFILES = vorwort.tex einleitung.tex maschinen.tex sprachen.tex \
	daten.tex algorithm.tex $(SAMPLEFILES) $(HEATFILES) $(JULIAFILES)

BLXFILES = skript1-blx.bbl skript2-blx.bbl skript3-blx.bbl

IMAGES = graphics/cgresults.pdf

skript.pdf:	skript.tex $(TEXFILES) skript.ind $(BLXFILES) $(IMAGES)
	pdflatex skript.tex
	bibtex skript

skript.ind:	skript.tex $(TEXFILES)
	touch skript.ind
	pdflatex skript.tex
	makeindex skript.idx

# References

skript.bbl:		skript.aux
	bibtex skript

skript1-blx.bbl:	skript1-blx.aux
	bibtex skript1-blx

skript2-blx.bbl:	skript2-blx.aux
	bibtex skript2-blx

skript3-blx.bbl:	skript3-blx.aux
	bibtex skript3-blx

# standalone compilation of a single article
standalone.pdf:	standalone.tex $(TEXFILES)
	pdflatex standalone.tex
	bibtex standalone
	pdflatex standalone.tex
