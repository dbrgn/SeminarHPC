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
RADONFILES = radon/RadonBack.bib radon/RadonBack.tex			\
	radon/sections/Einleitung.tex radon/sections/Erweiterung.tex	\
	radon/sections/Implementation.tex radon/sections/Loesungsansatz.tex \
	radon/sections/Problemstellung.tex radon/sections/Resultate.tex	\
	radon/sections/Rueckprojektion.tex
HEATFILES = heat/heat.tex
JULIAFILES = julia/julia.tex julia/julia.bib
MAPREDUCEFILES = mapreduce/mapreduce.tex mapreduce/map.bib
POTENTIALFILES = potential/potential.tex
GREENFILES = green/green.tex green/quellen.bib
STOSSWELLENFILES = stosswellen/stosswellen.tex
KUGELFILES = kugel/Kugelsternhaufen.tex kugel/kugel.bib

TEXFILES = vorwort.tex einleitung.tex maschinen.tex sprachen.tex \
	daten.tex algorithm.tex \
	$(RADONFILES) $(HEATFILES) $(JULIAFILES) $(MAPREDUCEFILES) \
	$(GREENFILES) $(STOSSWELLENFILES)

BLXFILES = skript1-blx.bbl skript2-blx.bbl skript3-blx.bbl skript4-blx.bbl \
	skript5-blx.bbl skript6-blx.bbl skript7-blx.bbl skript8-blx.bbl \
	skript9-blx.bbl skript10-blx.bbl skript11-blx.bbl

IMAGES = graphics/cgresults.pdf

skript.pdf:	skript.tex $(TEXFILES) skript.ind $(IMAGES) $(BLXFILES)
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

skript4-blx.bbl:	skript4-blx.aux
	bibtex skript4-blx

skript5-blx.bbl:	skript5-blx.aux
	bibtex skript5-blx

skript6-blx.bbl:	skript6-blx.aux
	bibtex skript6-blx

skript7-blx.bbl:	skript7-blx.aux
	bibtex skript7-blx

skript8-blx.bbl:	skript8-blx.aux
	bibtex skript8-blx

skript9-blx.bbl:	skript9-blx.aux
	bibtex skript9-blx

skript10-blx.bbl:	skript10-blx.aux
	bibtex skript10-blx

skript11-blx.bbl:	skript11-blx.aux
	bibtex skript11-blx

# standalone compilation of a single article
standalone.pdf:	standalone.tex $(TEXFILES)
	pdflatex standalone.tex
	bibtex standalone
	pdflatex standalone.tex
