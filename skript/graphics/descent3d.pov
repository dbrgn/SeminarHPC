#include "textures.inc"
#include "colors.inc"

camera {
	location <1.7, 1.2,-1.5>
	look_at <-0.01, 0, -0.01>
	right 16/9 * x * 0.29
	up y * 0.26
}

light_source {
        <5, 8, -7> color White
        area_light <0, 2,-1.4>, <-1.4, 2, 0>, 10, 10
}
light_source { <-5, 0.5,-3> color White }

sky_sphere {
	pigment {
		color White
	}
}

/* points of descent path */
#declare x4 = <0, 0, 0>;
#declare x3 = <0.3, 0, 0>;
#declare x2 = <0.3, 0, 0.2>;
#declare x1 = <0.3, 0.15, 0.2>;

/* ellipsoids */
#declare a = 2;
#declare b = 1;
#declare c = 2.5;

#macro sA(U, V)
	((U.x * V.x) / (a * a) + (U.y * V.y) / (b * b) + (U.z * V.z) / (c * c))
#end

#macro normA(U)
	sqrt(sA(U, U))
#end

#macro clippedellipsoid(X, col)
#local r = normA(X);
sphere {
	<0, 0,0>, r
	scale <a, b, c>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
	}
	pigment {
		color col
	}
}
#end

#macro clippedellipsoid2(X, col)
#local r = normA(X);
sphere {
	<0,0,0>, r
	scale <a, b, c>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
		prism {
			linear_spline
			0, 1, 4,
			<0,0>,<0,-1>,<1,-1>,<1,0.6666>
			inverse
		}
	}
	pigment { color col }
}
#end

clippedellipsoid(<0, 0.05, 0>, White)
clippedellipsoid(<0, 0.1, 0>, White)

#declare levelsurface = <1, 1, 0>;
#declare levelsurface = Yellow;

clippedellipsoid(x3, levelsurface)
clippedellipsoid2(x2, levelsurface)
clippedellipsoid2(x1, levelsurface)

clippedellipsoid2(<0, 0.2, 0>, White)
clippedellipsoid2(<0, 0.25, 0>, White)

/* descent path */
#macro redpoint(where)
#local center = <where.x, where.y, where.z>;
sphere { center, 0.01
	pigment { color Red }
}
#end

redpoint(x4)
redpoint(x3)
redpoint(x2)
redpoint(x1)

#macro draw(U, V)
#local startpoint = <U.x, U.y, U.z>;
#local endpoint = <V.x, V.y, V.z>;
cylinder {
	startpoint, endpoint, 0.007
	pigment {
		color Red
	}
}
#end

draw(x1, x2)
draw(x2, x3)
draw(x3, x4)

/* coordinate axes */
#macro axis(dir)
#local endpoint = <dir.x, dir.y, dir.z>;
cylinder {
	<0,0,0>, endpoint, 0.004
	pigment {
		color Blue
	}
}
cone {
	endpoint, 0.01, endpoint + 0.05 * vnormalize(endpoint), 0
	pigment {
		color Blue
	}
}
#end

axis(<0.55, 0, 0>)
axis(<0, 0.3, 0>)
axis(<0, 0, -0.67>)

