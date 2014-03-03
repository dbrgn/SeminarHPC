verbatimtex
\documentclass{book}
\usepackage{times}
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{amsfonts}
\usepackage{txfonts}
\begin{document}
etex;
beginfig(1)

pickup pencircle scaled 1pt;

numeric a; a = 140;
numeric b; b =  50;

drawarrow (-a - 10,   0)--(a + 10,  0);
drawarrow (   0,-b - 10)--(  0,b + 10);

label.urt(btex $y$ etex, (0, b + 10));
label.urt(btex $x$ etex, (a + 10,0));

pickup pencircle scaled 0.1pt;

def ellipse(expr x, y) =
	numeric c;
	c = sqrt((x/a)*(x/a) + (y/b)*(y/b));
	draw fullcircle xscaled (2*a*c) yscaled (2*b*c);
enddef;

def sqr(expr x) =
	(x * x)
enddef;

def cube(expr x) =
	(x * sqr(x))
enddef;

def descent =
	numeric	e;
	e := b / a;
	numeric c, d;
	c := sqr(x1 * e * e) + sqr(y1);
	d := sqr(x1 * e * e * e) + sqr(y1);
	numeric l;
	l := c / d;
	x2 := x1 - x1 * sqr(e) * l;
	y2 := y1 - y1 * l;
	pickup pencircle scaled 3pt;
	draw z1 withcolor (1.0,0.0,0.0);
	pickup pencircle scaled 1pt;
	draw z1--z2 withcolor(1.0,0.0,0.0);
	pickup pencircle scaled 3pt;
	draw z2 withcolor (1.0,0.0,0.0);
	x1 := x2;
	y1 := y2;
enddef;

pickup pencircle scaled 3pt;

z1 = (110,30);
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
label.urt(btex $x_1$ etex, z1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
label.lrt(btex $x_2$ etex, z1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
label.urt(btex $x_3$ etex, z1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
label.lrt(btex $x_4$ etex, z1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
label.urt(btex $x_5$ etex, z1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;
pickup pencircle scaled 0.1pt;
ellipse(x1, y1);
descent;

endfig;

end
