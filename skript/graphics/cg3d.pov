#include "colors.inc"

camera {
	location <1.2, 1.2,-4.5>
	look_at <0, 0, 0>
	right 16/9 * x * 0.12 * part
	up y * 0.12 * part
}

light_source {
        <5, 8, -7> color White
        area_light <0, 2,-1.4>, <-1.4, 2, 0>, 10, 10
}
light_source { <-5, 1.5, -3> color White }
light_source { <1.2, 1.2,-4.5> color White }

sky_sphere {
	pigment {
		color <1, 1, 1>
	}
}

/* coordinate axes */
#macro axis(from, dir)
	cylinder { from, from + dir, 0.004 * linethickness
		pigment { color Blue }
	}
	cone { from + dir, 0.01 * linethickness, from + dir + vnormalize(dir) * 0.05, 0
		pigment { color Blue }
	}
#end

axis(<0,0,0>, <0.45, 0, 0>)
axis(<0,0,0>, <0, 0.25, 0>)
axis(<0,0,0>, <0, 0, -0.57>)

#declare a = 2;
#declare b = 1;
#declare c = 2.5;

#macro grad(where)
	<2 * where.x / (a * a), 2 * where.y / (b * b), 2 * where.z / (c * c)>
#end

#declare x1 = <0.3, 0.08, -0.3>;

#macro redpoint(where)
	sphere { <where.x, where.y, where.z>, 0.01 * linethickness
		pigment { color Red }
	}
#end

#macro gradvec(where)
#local start = <where.x, where.y, where.z>;
#local dir = 0.1 * vnormalize(grad(where));
	cylinder { start, start + dir, 0.005 * linethickness
		pigment { color Green }
	}
	cone { start + dir, 0.007 * linethickness, start + 1.2 * dir, 0
		pigment { color Green }
	}
#end

redpoint(x1)
gradvec(x1)

#macro sA(U, V)
	((U.x * V.x) / (a * a) + (U.y * V.y) / (b * b) + (U.z * V.z) / (c * c))
#end
#macro normA(U)
	sqrt(sA(U, U))
#end

#declare r1 = grad(x1);
#declare v1 = r1 / normA(r1);
#declare x2 = x1 - sA(v1, x1) * v1;

#declare r2 = grad(x2);
#declare v2 = r2 - sA(r2, v1) * v1;
#declare v2 = v2 / normA(v2);
#declare x3 = x2 - sA(v2, x2) * v2;

#declare r3 = grad(x3);
#declare v3 = r3 - sA(r3, v2) * v2;
#declare v3 = v3 / normA(v3);
#declare x4 = x3 - sA(v3, x3) * v3;

#macro draw(U, V)
	cylinder { <U.x, U.y, U.z>, <V.x, V.y, V.z>, 0.007 * linethickness
		pigment { color Red }
	}
#end

draw(x1, x2)
draw(x2, x3)
draw(x3, x4)

redpoint(x2)
gradvec(x2)

redpoint(x3)
gradvec(x3)

redpoint(x4)

#declare n1 = vnormalize(-vcross(x1, y));
#declare n2 = vnormalize(vcross(x1, x2));
#declare n3 = vnormalize(vcross(x2, x3));
#declare n4 = vnormalize(vcross(x3, y));

#declare cutout = union {
	plane { n1, 0 }
	plane { n2, 0 }
	plane { n3, 0 }
	plane { n4, 0 }
};

#macro ellipsoid(where)
#local w = <where.x, where.y, where.z>;
#local r = normA(w);
	sphere {
		<0, 0, 0>, r
		scale <a, b, c>
		clipped_by { cutout }
		pigment { color White }
	}
#end

ellipsoid(x1)
ellipsoid(x2)
ellipsoid(x3)

#declare m2 = vnormalize(vcross(v1, v2));
#declare m3 = vnormalize(vcross(v2, v3));

#declare m2offset = vdot(m2, x2);
#declare m3offset = vdot(m3, x3);

#macro	planedisk(where, n)
#local	center = <where.x, where.y, where.z>;
#local	norm = <n.x, n.y, n.z>;
#local	d = vdot(norm, center);
	intersection {
		sphere { center, 0.05 }
		plane { norm, d + 0.001 }
		plane { -norm, -d + 0.001 }
		pigment { color <1, 1, 0> }
	}
#end

planedisk(x2, m2)
planedisk(x3, m3)

#macro thinline(dir)
#local endpoint = normA(x1) * dir / normA(dir);
	cylinder {
		<0, 0, 0>, endpoint, 0.001
		pigment { color Yellow }
	}
#end

//thinline(x1)
//thinline(x2)
//thinline(x3)
