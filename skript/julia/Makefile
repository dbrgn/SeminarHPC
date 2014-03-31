#
# Makefile for the julia article
#
# (c) 2014 Prof Dr Andreas Mueller, Hochschule Rapperswil
#

mono = a.png b.png c.png d.png e.png f.png g.png h.png
monolow = a-low.png b-low.png c-low.png d-low.png e-low.png \
	f-low.png g-low.png h-low.png
color = c-a.png c-b.png c-c.png c-d.png c-e.png c-f.png c-g.png c-h.png
colorlow = c-a-low.png c-b-low.png c-c-low.png c-d-low.png c-e-low.png \
	c-f-low.png c-g-low.png c-h-low.png
julia = j-a.png j-b.png j-c.png j-d.png j-e.png j-f.png j-g.png j-h.png
julialow = j-a-low.png j-b-low.png j-c-low.png j-d-low.png j-e-low.png \
	j-f-low.png j-g-low.png j-h-low.png

all:	$(monolow) $(colorlow) $(julialow)

scale = 100%

$(monolow):	$(mono)
	for f in $(mono); \
	do \
		convert $${f} -scale $(scale) `basename $${f} .png`-low.png; \
	done

$(colorlow):	$(color)
	for f in $(color); \
	do \
		convert $${f} -scale $(scale) `basename $${f} .png`-low.png; \
	done

$(julialow):	$(julia)
	for f in $(julia); \
	do \
		convert $${f} -scale $(scale) `basename $${f} .png`-low.png; \
	done
