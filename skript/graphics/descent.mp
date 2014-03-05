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

beginfig(2)

label.urt(btex $x_1$ etex, (50,50));
pickup pencircle scaled 0.1pt;
draw fullcircle scaled (sqrt(2) * 100);
pickup pencircle scaled 3pt;
draw(50,50) withcolor (1.0,0.0,0.0);
pickup pencircle scaled 1pt;
label.rt(btex $x_2$ etex, (50,0));
draw (50,50)--(50,0) withcolor (1.0,0.0,0.0);
pickup pencircle scaled 3pt;
draw (50,0) withcolor (1.0,0.0,0.0);
pickup pencircle scaled 0.1pt;
draw fullcircle scaled 100pt;
pickup pencircle scaled 1pt;
draw (0,0)--(50,0) withcolor (1.0,0.0,0.0);
pickup pencircle scaled 3pt;
draw (0,0) withcolor(1.0,0.0,0.0);
label.lft(btex $x_3$ etex, (0,0));

pickup pencircle scaled 1pt;
drawarrow (-80,-80)--(80,-80); label.urt(btex $x$ etex, (80,-80));
drawarrow (-80,-80)--(-80,80); label.urt(btex $y$ etex, (-80,80));

endfig;

beginfig(3)

pickup pencircle scaled 1pt;

numeric e;
e := 50;

drawarrow (-10,0)--(3 * e, 0); label.urt(btex $x$ etex, (3*e,0));
drawarrow (0,-3 * e)--(0,10); label.urt(btex $y$ etex, (0,10));

z1 = (2 * e, 0);
z2 = ((9./7.) * e, -(10/7) * e);
z3 = (2 * e, -2 * e);


pickup pencircle scaled 0.1pt;
numeric l;
l := 4 * 2 / sqrt(3);
draw fullcircle xscaled (l * sqrt(0.5) * e) yscaled (l * sqrt(1.5) * e) rotated 45 shifted (z3);
l := 1.508;
draw fullcircle xscaled (l * sqrt(0.5) * e) yscaled (l * sqrt(1.5) * e) rotated 45 shifted (z3);

z0 = (0,0);
z4 = (4 * e, 0);
z5 = (0, -4 * e);
z6 = (4 * e, -4 * e);

draw (-0.05)[z0,z6]--(1.05)[z0,z6];
draw (-0.05)[z4,z5]--(1.05)[z4,z5];


label.urt(btex $x_1$ etex, z1);
label.lft(btex $x_2$ etex, z2);
label.rt(btex $x_3$ etex, z3);

pickup pencircle scaled 3pt;
draw z1 withcolor(1.0,0.0,0.0);
pickup pencircle scaled 1pt;
draw z1--z2 withcolor(1.0,0.0,0.0);
pickup pencircle scaled 3pt;
draw z2 withcolor(1.0,0.0,0.0);
pickup pencircle scaled 1pt;
draw z2--z3 withcolor(1.0,0.0,0.0);
pickup pencircle scaled 3pt;
draw z3 withcolor(1.0,0.0,0.0);

clip currentpicture to (-20,20)--(3 * e + 20,20)--(3 * e + 20, -3 * e - 20)--(-20,-3 * e - 20)--cycle;

endfig;

end
