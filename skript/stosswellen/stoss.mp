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

fill (25,75)--(75,75)--(75,25)--(125,25)--(125,75)--(175,75)--(175,125)--(125,125)--(125,175)--(75,175)--(75,125)--(25,125)--cycle withcolor(0.9,0.9,1.0);

fill
(85,135)--(85,165)--(115,165)--(115,135)--cycle withcolor(1.0,0.9,0.9);
pickup pencircle scaled 0.5pt;

draw (-10,  0)--(210,  0);
draw (-10, 50)--(210, 50);
draw (-10,100)--(210,100);
draw (-10,150)--(210,150);
draw (-10,200)--(210,200);
draw (  0,-10)--(  0,210);
draw ( 50,-10)--( 50,210);
draw (100,-10)--(100,210);
draw (150,-10)--(150,210);
draw (200,-10)--(200,210);

pickup pencircle scaled 5pt;

draw (  0,  0); draw ( 50,  0); draw ( 100,  0); draw (150,  0); draw (200,  0);
draw (  0, 50); draw ( 50, 50); draw ( 100, 50); draw (150, 50); draw (200, 50);
draw (  0, 100); draw ( 50, 100); draw ( 100, 100); draw (150, 100); draw (200, 100);
draw (  0,150); draw ( 50,150);
draw ( 100,150) withcolor (1.0,0.0,0.0);
draw (150,150); draw (200,150);
draw (  0,200); draw ( 50,200); draw ( 100,200); draw (150,200); draw (200,200);

pickup pencircle scaled 3pt;

draw (  0,  0) withcolor(1.0,1.0,1.0);
draw ( 50,  0) withcolor(1.0,1.0,1.0);
draw ( 100,  0) withcolor(1.0,1.0,1.0);
draw (150,  0) withcolor(1.0,1.0,1.0);
draw (200,  0) withcolor(1.0,1.0,1.0);

draw (  0, 50) withcolor(1.0,1.0,1.0);
draw ( 50, 50) withcolor(1.0,1.0,1.0);
draw ( 100, 50) withcolor(1.0,1.0,1.0);
draw (150, 50) withcolor(1.0,1.0,1.0);
draw (200, 50) withcolor(1.0,1.0,1.0);

draw (  0, 100) withcolor(1.0,1.0,1.0);
draw ( 50, 100) withcolor(1.0,1.0,1.0);
draw ( 100, 100) withcolor(1.0,1.0,1.0);
draw (150, 100) withcolor(1.0,1.0,1.0);
draw (200, 100) withcolor(1.0,1.0,1.0);

draw (  0,150) withcolor(1.0,1.0,1.0);
draw ( 50,150) withcolor(1.0,1.0,1.0);
draw ( 100,150) withcolor(1.0,1.0,1.0);
draw (150,150) withcolor(1.0,1.0,1.0);
draw (200,150) withcolor(1.0,1.0,1.0);

draw (  0,200) withcolor(1.0,1.0,1.0);
draw ( 50,200) withcolor(1.0,1.0,1.0);
draw ( 100,200) withcolor(1.0,1.0,1.0);
draw (150,200) withcolor(1.0,1.0,1.0);
draw (200,200) withcolor(1.0,1.0,1.0);

pickup pencircle scaled 1pt;
draw (-28,   0); draw (220,   0);
draw (-24,   0); draw (224,   0);
draw (-20,   0); draw (228,   0);

draw (-28,  50); draw (220,  50);
draw (-24,  50); draw (224,  50);
draw (-20,  50); draw (228,  50);

draw (-28,  100); draw (220,  100);
draw (-24,  100); draw (224,  100);
draw (-20,  100); draw (228,  100);

draw (-28, 150); draw (220, 150);
draw (-24, 150); draw (224, 150);
draw (-20, 150); draw (228, 150);

draw (-28, 200); draw (220, 200);
draw (-24, 200); draw (224, 200);
draw (-20, 200); draw (228, 200);

draw (  0, -20); draw (  0, 220);
draw (  0, -24); draw (  0, 224);
draw (  0, -28); draw (  0, 228);

draw ( 50, -20); draw ( 50, 220);
draw ( 50, -24); draw ( 50, 224);
draw ( 50, -28); draw ( 50, 228);

draw ( 100, -20); draw ( 100, 220);
draw ( 100, -24); draw ( 100, 224);
draw ( 100, -28); draw ( 100, 228);

draw (150, -20); draw (150, 220);
draw (150, -24); draw (150, 224);
draw (150, -28); draw (150, 228);

draw (200, -20); draw (200, 220);
draw (200, -24); draw (200, 224);
draw (200, -28); draw (200, 228);

label.urt(btex $u(x_{n},t_{m})$ etex,    ( 100, 100));
label.urt(btex $u(x_{n\!+\!1},t_{m})$ etex, (150, 100));
label.urt(btex $u(x_{n\!-\!1},t_{m})$ etex, ( 50, 100));
label.urt(btex $u(x_{n},t_{m\!-\!1})$ etex, ( 100, 50));
label.urt(btex $u(x_{n},t_{m\!+\!1})$ etex, ( 100,150)) withcolor (1.0,0.0,0.0);

label.bot(btex $x_{n}$ etex,     ( 100, -45));
label.bot(btex $x_{n+2}$ etex, (200, -45));
label.bot(btex $x_{n+1}$ etex, (150, -45));
label.bot(btex $x_{n-1}$ etex, ( 50, -45));
label.bot(btex $x_{n-2}$ etex, (  0, -45));

label.lft(btex $t_{m}$ etex,     (-45,  100));
label.lft(btex $t_{m-2}$ etex, (-45,   0));
label.lft(btex $t_{m-1}$ etex, (-45,  50));
label.lft(btex $t_{m+1}$ etex, (-45, 150));
label.lft(btex $t_{m+2}$ etex, (-45, 200));

pickup pencircle scaled 1pt;

drawarrow (-35,-35)--(220,-35);
drawarrow (-35,-35)--(-35,220);

label.urt(btex $x$ etex, (220,-35));
label.urt(btex $t$ etex, (-35,220));

endfig;

beginfig(2)

fill (25,25)--(175,25)--(175,175)--(150,175)--(25,50)--cycle withcolor(0.9,0.9,1.0);

pickup pencircle scaled 0.5pt;

draw (-10,  0)--(210,  0);
draw (-10, 50)--(210, 50);
draw (-10,100)--(210,100);
draw (-10,150)--(210,150);
draw (-10,200)--(210,200);
draw (  0,-10)--(  0,210);
draw ( 50,-10)--( 50,210);
draw (100,-10)--(100,210);
draw (150,-10)--(150,210);
draw (200,-10)--(200,210);

pickup pencircle scaled 5pt;

draw (  0,  0); draw ( 50,  0); draw ( 100,  0); draw (150,  0); draw (200,  0);
draw (  0, 50); draw ( 50, 50); draw ( 100, 50); draw (150, 50); draw (200, 50);
draw (  0, 100); draw ( 50, 100); draw ( 100, 100); draw (150, 100); draw (200, 100);
draw (  0,150); draw ( 50,150); draw ( 100,150); draw (150,150); draw (200,150);
draw (  0,200); draw ( 50,200); draw ( 100,200); draw (150,200); draw (200,200);

pickup pencircle scaled 3pt;

draw (  0,  0) withcolor(1.0,1.0,1.0);
draw ( 50,  0) withcolor(1.0,1.0,1.0);
draw ( 100,  0) withcolor(1.0,1.0,1.0);
draw (150,  0) withcolor(1.0,1.0,1.0);
draw (200,  0) withcolor(1.0,1.0,1.0);

draw (  0, 50) withcolor(1.0,1.0,1.0);
draw ( 50, 50) withcolor(1.0,1.0,1.0);
draw ( 100, 50) withcolor(1.0,1.0,1.0);
draw (150, 50) withcolor(1.0,1.0,1.0);
draw (200, 50) withcolor(1.0,1.0,1.0);

draw (  0, 100) withcolor(1.0,1.0,1.0);
draw ( 50, 100) withcolor(1.0,1.0,1.0);
draw ( 100, 100) withcolor(1.0,1.0,1.0);
draw (150, 100) withcolor(1.0,1.0,1.0);
draw (200, 100) withcolor(1.0,1.0,1.0);

draw (  0,150) withcolor(1.0,1.0,1.0);
draw ( 50,150) withcolor(1.0,1.0,1.0);
draw ( 100,150) withcolor(1.0,1.0,1.0);
draw (150,150) withcolor(1.0,1.0,1.0);
draw (200,150) withcolor(1.0,1.0,1.0);

draw (  0,200) withcolor(1.0,1.0,1.0);
draw ( 50,200) withcolor(1.0,1.0,1.0);
draw ( 100,200) withcolor(1.0,1.0,1.0);
draw (150,200) withcolor(1.0,1.0,1.0);
draw (200,200) withcolor(1.0,1.0,1.0);


pickup pencircle scaled 1pt;

drawarrow (50,50)--(100,100) withcolor (1.0,0.0,0.0);
drawarrow (100,50)--(100,100) withcolor (1.0,0.0,0.0);

drawarrow (100,50)--(150,100) withcolor (1.0,0.0,0.0);
drawarrow (150,50)--(150,100) withcolor (1.0,0.0,0.0);

drawarrow (100,100)--(150,150) withcolor (1.0,0.0,0.0);
drawarrow (150,100)--(150,150) withcolor (1.0,0.0,0.0);

drawarrow (50,50)--(75,100) withcolor (0.0,0.0,1.0);
drawarrow (100,50)--(75,100) withcolor (0.0,0.0,1.0);

pickup pencircle scaled 3pt;
draw ( 75,100);


pickup pencircle scaled 1pt;
draw (-28,   0); draw (220,   0);
draw (-24,   0); draw (224,   0);
draw (-20,   0); draw (228,   0);

draw (-28,  50); draw (220,  50);
draw (-24,  50); draw (224,  50);
draw (-20,  50); draw (228,  50);

draw (-28,  100); draw (220,  100);
draw (-24,  100); draw (224,  100);
draw (-20,  100); draw (228,  100);

draw (-28, 150); draw (220, 150);
draw (-24, 150); draw (224, 150);
draw (-20, 150); draw (228, 150);

draw (-28, 200); draw (220, 200);
draw (-24, 200); draw (224, 200);
draw (-20, 200); draw (228, 200);

draw (  0, -20); draw (  0, 220);
draw (  0, -24); draw (  0, 224);
draw (  0, -28); draw (  0, 228);

draw ( 50, -20); draw ( 50, 220);
draw ( 50, -24); draw ( 50, 224);
draw ( 50, -28); draw ( 50, 228);

draw ( 100, -20); draw ( 100, 220);
draw ( 100, -24); draw ( 100, 224);
draw ( 100, -28); draw ( 100, 228);

draw (150, -20); draw (150, 220);
draw (150, -24); draw (150, 224);
draw (150, -28); draw (150, 228);

draw (200, -20); draw (200, 220);
draw (200, -24); draw (200, 224);
draw (200, -28); draw (200, 228);

label.urt(btex $u(x_{n},t_{m})$ etex,    ( 100, 100));
label.urt(btex $u(x_{n\!+\!1},t_{m})$ etex, (150, 100));
label.urt(btex $u(x_{n},t_{m\!-\!1})$ etex, ( 100, 50));
label.urt(btex $u(x_{n\!-\!1},t_{m\!-\!1})$ etex, ( 50, 50));
label.urt(btex $u(x_{n\!+\!1},t_{m\!-\!1})$ etex, ( 150, 50));
label.urt(btex $u(x_{n\!+\!1},t_{m\!+\!1})$ etex, ( 150,150));

label.urt(btex $u(x_{n\!-\!\frac{1}{2}},t_{m})$ etex, ( 55,100));

label.bot(btex $x_{n}$ etex,   (100, -45));
label.bot(btex $x_{n+2}$ etex, (200, -45));
label.bot(btex $x_{n+1}$ etex, (150, -45));
label.bot(btex $x_{n-1}$ etex, ( 50, -45));
label.bot(btex $x_{n-2}$ etex, (  0, -45));

label.lft(btex $t_{m}$ etex,     (-45,  100));
label.lft(btex $t_{m-2}$ etex, (-45,   0));
label.lft(btex $t_{m-1}$ etex, (-45,  50));
label.lft(btex $t_{m+1}$ etex, (-45, 150));
label.lft(btex $t_{m+2}$ etex, (-45, 200));




pickup pencircle scaled 1pt;

drawarrow (-35,-35)--(220,-35);
drawarrow (-35,-35)--(-35,220);

label.urt(btex $x$ etex, (220,-35));
label.urt(btex $t$ etex, (-35,220));

endfig;

end
