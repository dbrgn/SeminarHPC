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

fill (-30,-30)--(190,-30)--(190, 95)--(-30, 95)--cycle withcolor(0.9,0.9,1.0);
fill ( 25,105)--(135,105)--(135,135)--( 25,135)--cycle withcolor(1.0,0.9,0.9);

pickup pencircle scaled 0.5pt;

draw (-10,  0)--(170,  0);
draw (-10, 40)--(170, 40);
draw (-10, 80)--(170, 80);
draw (-10,120)--(170,120);
draw (-10,160)--(170,160);
draw (  0,-10)--(  0,170);
draw ( 40,-10)--( 40,170);
draw ( 80,-10)--( 80,170);
draw (120,-10)--(120,170);
draw (160,-10)--(160,170);

pickup pencircle scaled 5pt;

draw (  0,  0); draw ( 40,  0); draw ( 80,  0); draw (120,  0); draw (160,  0);
draw (  0, 40); draw ( 40, 40); draw ( 80, 40); draw (120, 40); draw (160, 40);
draw (  0, 80); draw ( 40, 80); draw ( 80, 80); draw (120, 80); draw (160, 80);
draw (  0,120);                                                 draw (160,120);
draw (  0,160); draw ( 40,160); draw ( 80,160); draw (120,160); draw (160,160);

draw ( 40, 120) withcolor (1.0,0.0,0.0);
draw ( 80, 120) withcolor (1.0,0.0,0.0);
draw (120, 120) withcolor (1.0,0.0,0.0);

draw ( 80, 100);

pickup pencircle scaled 3pt;

draw (  0,  0) withcolor(1.0,1.0,1.0);
draw ( 40,  0) withcolor(1.0,1.0,1.0);
draw ( 80,  0) withcolor(1.0,1.0,1.0);
draw (120,  0) withcolor(1.0,1.0,1.0);
draw (160,  0) withcolor(1.0,1.0,1.0);

draw (  0, 40) withcolor(1.0,1.0,1.0);
draw ( 40, 40) withcolor(1.0,1.0,1.0);
draw ( 80, 40) withcolor(1.0,1.0,1.0);
draw (120, 40) withcolor(1.0,1.0,1.0);
draw (160, 40) withcolor(1.0,1.0,1.0);

draw (  0, 80) withcolor(1.0,1.0,1.0);
draw ( 40, 80) withcolor(1.0,1.0,1.0);
draw ( 80, 80) withcolor(1.0,1.0,1.0);
draw (120, 80) withcolor(1.0,1.0,1.0);
draw (160, 80) withcolor(1.0,1.0,1.0);

draw (  0,120) withcolor(1.0,1.0,1.0);
draw ( 40,120) withcolor(1.0,1.0,1.0);
draw ( 80,120) withcolor(1.0,1.0,1.0);
draw (120,120) withcolor(1.0,1.0,1.0);
draw (160,120) withcolor(1.0,1.0,1.0);

draw (  0,160) withcolor(1.0,1.0,1.0);
draw ( 40,160) withcolor(1.0,1.0,1.0);
draw ( 80,160) withcolor(1.0,1.0,1.0);
draw (120,160) withcolor(1.0,1.0,1.0);
draw (160,160) withcolor(1.0,1.0,1.0);

pickup pencircle scaled 1pt;
draw (-28,  0); draw (180,  0);
draw (-24,  0); draw (184,  0);
draw (-20,  0); draw (188,  0);
draw (-28, 40); draw (180, 40);
draw (-24, 40); draw (184, 40);
draw (-20, 40); draw (188, 40);
draw (-28, 80); draw (180, 80);
draw (-24, 80); draw (184, 80);
draw (-20, 80); draw (188, 80);
draw (-28,120); draw (180,120);
draw (-24,120); draw (184,120);
draw (-20,120); draw (188,120);
draw (-28,160); draw (180,160);
draw (-24,160); draw (184,160);
draw (-20,160); draw (188,160);

draw (  0, -20); draw (  0,180);
draw (  0, -24); draw (  0,184);
draw (  0, -28); draw (  0,188);
draw ( 40, -20); draw ( 40,180);
draw ( 40, -24); draw ( 40,184);
draw ( 40, -28); draw ( 40,188);
draw ( 80, -20); draw ( 80,180);
draw ( 80, -24); draw ( 80,184);
draw ( 80, -28); draw ( 80,188);
draw (120, -20); draw (120,180);
draw (120, -24); draw (120,184);
draw (120, -28); draw (120,188);
draw (160, -20); draw (160,180);
draw (160, -24); draw (160,184);
draw (160, -28); draw (160,188);

label.urt(btex $u_{ij}$ etex,    ( 80, 80));
label.urt(btex $u_{i,j+1}$ etex, (120, 80));
label.urt(btex $u_{i,j-1}$ etex, ( 40, 80));

label.urt(btex $u_{i-1,j}$ etex, ( 80, 40));
label.urt(btex $u_{i-1,j-1}$ etex, ( 40, 40));
label.urt(btex $u_{i-1,j+1}$ etex, (120, 40));

label.urt(btex $u_{i+1,j}$ etex, ( 80,120)) withcolor (1.0,0.0,0.0);
label.urt(btex $u_{i+1,j-1}$ etex, ( 40,120)) withcolor (1.0,0.0,0.0);
label.urt(btex $u_{i+1,j+1}$ etex, (120,120)) withcolor (1.0,0.0,0.0);

label.bot(btex $jh_x$ etex,     ( 80, -45));
label.bot(btex $(j+2)h_x$ etex, (160, -45));
label.bot(btex $(j+1)h_x$ etex, (120, -45));
label.bot(btex $(j-1)h_x$ etex, ( 40, -45));
label.bot(btex $(j-2)h_x$ etex, (  0, -45));

label.lft(btex $ih_t$ etex,     (-45,  80));
label.lft(btex $(i-2)h_t$ etex, (-45,   0));
label.lft(btex $(i-1)h_t$ etex, (-45,  40));
label.lft(btex $(i+1)h_t$ etex, (-45, 120));
label.lft(btex $(i+2)h_t$ etex, (-45, 160));

label.rt(btex $(i+\frac12,j)$ etex, (80,100));

pickup pencircle scaled 1pt;

drawarrow (-35,-35)--(190,-35);
drawarrow (-35,-35)--(-35,180);

label.urt(btex $x$ etex, (190,-35));
label.urt(btex $t$ etex, (-35,180));

endfig;

beginfig(2)

fill (20,60)--(60,60)--(60,20)--(100,20)--(100,60)--(140,60)--(140,100)--(100,100)--(100,140)--(60,140)--(60,100)--(20,100)--cycle withcolor(0.9,0.9,1.0);

pickup pencircle scaled 0.5pt;

draw (-10,  0)--(170,  0);
draw (-10, 40)--(170, 40);
draw (-10, 80)--(170, 80);
draw (-10,120)--(170,120);
draw (-10,160)--(170,160);
draw (  0,-10)--(  0,170);
draw ( 40,-10)--( 40,170);
draw ( 80,-10)--( 80,170);
draw (120,-10)--(120,170);
draw (160,-10)--(160,170);

pickup pencircle scaled 5pt;

draw (  0,  0); draw ( 40,  0); draw ( 80,  0); draw (120,  0); draw (160,  0);
draw (  0, 40); draw ( 40, 40); draw ( 80, 40); draw (120, 40); draw (160, 40);
draw (  0, 80); draw ( 40, 80); draw ( 80, 80); draw (120, 80); draw (160, 80);
draw (  0,120); draw ( 40,120); draw ( 80,120); draw (120,120); draw (160,120);
draw (  0,160); draw ( 40,160); draw ( 80,160); draw (120,160); draw (160,160);

pickup pencircle scaled 3pt;

draw (  0,  0) withcolor(1.0,1.0,1.0);
draw ( 40,  0) withcolor(1.0,1.0,1.0);
draw ( 80,  0) withcolor(1.0,1.0,1.0);
draw (120,  0) withcolor(1.0,1.0,1.0);
draw (160,  0) withcolor(1.0,1.0,1.0);

draw (  0, 40) withcolor(1.0,1.0,1.0);
draw ( 40, 40) withcolor(1.0,1.0,1.0);
draw ( 80, 40) withcolor(1.0,1.0,1.0);
draw (120, 40) withcolor(1.0,1.0,1.0);
draw (160, 40) withcolor(1.0,1.0,1.0);

draw (  0, 80) withcolor(1.0,1.0,1.0);
draw ( 40, 80) withcolor(1.0,1.0,1.0);
draw ( 80, 80) withcolor(1.0,1.0,1.0);
draw (120, 80) withcolor(1.0,1.0,1.0);
draw (160, 80) withcolor(1.0,1.0,1.0);

draw (  0,120) withcolor(1.0,1.0,1.0);
draw ( 40,120) withcolor(1.0,1.0,1.0);
draw ( 80,120) withcolor(1.0,1.0,1.0);
draw (120,120) withcolor(1.0,1.0,1.0);
draw (160,120) withcolor(1.0,1.0,1.0);

draw (  0,160) withcolor(1.0,1.0,1.0);
draw ( 40,160) withcolor(1.0,1.0,1.0);
draw ( 80,160) withcolor(1.0,1.0,1.0);
draw (120,160) withcolor(1.0,1.0,1.0);
draw (160,160) withcolor(1.0,1.0,1.0);

pickup pencircle scaled 1pt;
draw (-28,   0); draw (180,   0);
draw (-24,   0); draw (184,   0);
draw (-20,   0); draw (188,   0);

draw (-28,  40); draw (180,  40);
draw (-24,  40); draw (184,  40);
draw (-20,  40); draw (188,  40);

draw (-28,  80); draw (180,  80);
draw (-24,  80); draw (184,  80);
draw (-20,  80); draw (188,  80);

draw (-28, 120); draw (180, 120);
draw (-24, 120); draw (184, 120);
draw (-20, 120); draw (188, 120);

draw (-28, 160); draw (180, 160);
draw (-24, 160); draw (184, 160);
draw (-20, 160); draw (188, 160);

draw (  0, -20); draw (  0, 180);
draw (  0, -24); draw (  0, 184);
draw (  0, -28); draw (  0, 188);

draw ( 40, -20); draw ( 40, 180);
draw ( 40, -24); draw ( 40, 184);
draw ( 40, -28); draw ( 40, 188);

draw ( 80, -20); draw ( 80, 180);
draw ( 80, -24); draw ( 80, 184);
draw ( 80, -28); draw ( 80, 188);

draw (120, -20); draw (120, 180);
draw (120, -24); draw (120, 184);
draw (120, -28); draw (120, 188);

draw (160, -20); draw (160, 180);
draw (160, -24); draw (160, 184);
draw (160, -28); draw (160, 188);

label.urt(btex $u_{ijk}$ etex,    ( 80, 80));
label.urt(btex $u_{ij+1,k}$ etex, (120, 80));
label.urt(btex $u_{ij-1,k}$ etex, ( 40, 80));
label.urt(btex $u_{ij,k-1}$ etex, ( 80, 40));
label.urt(btex $u_{ij,k+1}$ etex, ( 80,120));

label.bot(btex $jh$ etex,     ( 80, -45));
label.bot(btex $(j+2)h$ etex, (160, -45));
label.bot(btex $(j+1)h$ etex, (120, -45));
label.bot(btex $(j-1)h$ etex, ( 40, -45));
label.bot(btex $(j-2)h$ etex, (  0, -45));

label.lft(btex $kh$ etex,     (-45,  80));
label.lft(btex $(k-2)h$ etex, (-45,   0));
label.lft(btex $(k-1)h$ etex, (-45,  40));
label.lft(btex $(k+1)h$ etex, (-45, 120));
label.lft(btex $(k+2)h$ etex, (-45, 160));

pickup pencircle scaled 1pt;

drawarrow (-35,-35)--(180,-35);
drawarrow (-35,-35)--(-35,180);

label.urt(btex $x$ etex, (180,-35));
label.urt(btex $y$ etex, (-35,180));

endfig;

beginfig(3)

pickup pencircle scaled 1pt;

fill (  1,  1)--( 79,  1)--( 79, 79)--(  1, 79)--cycle withcolor(0.9,0.9,0.9);
fill ( 81,  1)--(159,  1)--(159, 79)--( 81, 79)--cycle withcolor(0.9,0.9,0.9);
fill (  1, 81)--( 79, 81)--( 79,159)--(  1,159)--cycle withcolor(0.9,0.9,0.9);
fill ( 81, 81)--(159, 81)--(159,159)--( 81,159)--cycle withcolor(0.9,0.9,0.9);
fill (  1,201)--( 79,201)--( 79,279)--(  1,279)--cycle withcolor(0.9,0.9,0.9);
fill ( 81,201)--(159,201)--(159,279)--( 81,279)--cycle withcolor(0.9,0.9,0.9);

fill (201,  1)--(279,  1)--(279, 79)--(201, 79)--cycle withcolor(0.9,0.9,0.9);
fill (201, 81)--(279, 81)--(279,159)--(201,159)--cycle withcolor(0.9,0.9,0.9);
fill (201,201)--(279,201)--(279,279)--(201,279)--cycle withcolor(0.9,0.9,0.9);

draw (176, 40);
draw (180, 40);
draw (184, 40);

draw (176,120);
draw (180,120);
draw (184,120);

draw (176,240);
draw (180,240);
draw (184,240);

draw ( 40, 176);
draw ( 40, 180);
draw ( 40, 184);

draw (120, 176);
draw (120, 180);
draw (120, 184);

draw (240, 176);
draw (240, 180);
draw (240, 184);

label(btex $r = 0$ etex, ( 40, 40));
label(btex $r = 1$ etex, (120, 40));
label(btex $r = n_x - 1$ etex, (240, 40));

label(btex $r = n_x$ etex, (40, 120));
label(btex $r = n_x + 1$ etex, (120, 120));
label(btex $r = 2n_x - 1$ etex, (240, 120));

label(btex $r = (n_y - 1)n_x$ etex, ( 40, 240));
label(btex $r = (n_y - 1)n_x + 1$ etex, (120, 240));
label(btex $r = n_xn_y - 1$ etex, (240, 240));

pickup pencircle scaled 0.2pt;

fill ( 81, 81)--(159, 81)--(159, 84)--( 81, 84)--cycle withcolor (1.0,0.9,0.9);
fill ( 81, 76)--(159, 76)--(159, 79)--( 81, 79)--cycle withcolor (1.0,0.9,0.9);

fill ( 81,159)--(159,159)--(159,156)--( 81,156)--cycle withcolor (1.0,0.9,0.9);
fill ( 81,161)--(159,161)--(159,164)--( 81,164)--cycle withcolor (1.0,0.9,0.9);

fill ( 81, 81)--( 84, 81)--( 84,159)--( 81,159)--cycle withcolor (1.0,0.9,0.9);
fill ( 79, 81)--( 76, 81)--( 76,159)--( 79,159)--cycle withcolor (1.0,0.9,0.9);

fill (159, 81)--(156, 81)--(156,159)--(159,159)--cycle withcolor (1.0,0.9,0.9);
fill (161, 81)--(164, 81)--(164,159)--(161,159)--cycle withcolor (1.0,0.9,0.9);

draw ( 81, 81)--(159, 81)--(159, 84)--( 81, 84)--cycle;
draw ( 81, 76)--(159, 76)--(159, 79)--( 81, 79)--cycle;

draw ( 81,159)--(159,159)--(159,156)--( 81,156)--cycle;
draw ( 81,161)--(159,161)--(159,164)--( 81,164)--cycle;

draw ( 81, 81)--( 84, 81)--( 84,159)--( 81,159)--cycle;
draw ( 79, 81)--( 76, 81)--( 76,159)--( 79,159)--cycle;

draw (159, 81)--(156, 81)--(156,159)--(159,159)--cycle;
draw (161, 81)--(164, 81)--(164,159)--(161,159)--cycle;

pickup pencircle scaled 0.5pt;

draw (0,0)--(280,0)--(280,280)--(0,280)--cycle;

pickup pencircle scaled 1pt;

drawarrow (-10,-10)--(290,-10);
drawarrow (-10,-10)--(-10,290);
label.urt(btex $x$ etex, (290,-10));
label.urt(btex $y$ etex, (-10,290));

label.top (btex $w$ etex, (140,280));
label.rt(btex $h$ etex, (280,140));

endfig;

end
