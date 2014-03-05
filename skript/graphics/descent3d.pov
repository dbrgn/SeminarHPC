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
        area_light <0, 2,-1.4>, <-1.4, 2, 0>, 5, 5
}
light_source { <-5, 0.5,-3> color <0.5,0.5,0.5> }

sky_sphere {
	pigment {
		color <1, 1, 1>
	}
}

sphere {
	<0, 0,0>, 0.05
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
	}
	pigment {
		color <1.0, 1.0, 1.0>
	}
}

sphere {
	<0, 0,0>, 0.1
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
	}
	pigment {
		color <1.0, 1.0, 1.0>
	}
}

sphere {
	<0, 0,0>, 0.15
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
	}
	pigment {
		color <1.0, 1.0, 1.0>
	}
}

sphere {
	<0,0,0>, 0.17
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
		prism {
			linear_spline
			0, 1, 4,
			<0,0>,<0,-1>,<1,-1>,<1,0.6666>
			inverse
		}
	}
	pigment {
		color <1.0, 0.9, 0.9>
	}
}

sphere {
	<0,0,0>, 0.2
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
		prism {
			linear_spline
			0, 1, 4,
			<0,0>,<0,-1>,<1,-1>,<1,0.6666>
			inverse
		}
	}
	pigment {
		color <1.0, 1.0, 1.0>
	}
}

sphere {
	<0,0,0>, 0.22672
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
		prism {
			linear_spline
			0, 1, 4,
			<0,0>,<0,-1>,<1,-1>,<1,0.6666>
			inverse
		}
	}
	pigment {
		color <1.0, 0.9, 0.9>
	}
}

sphere {
	<0,0,0>, 0.25
	scale <2, 1, 2.5>
	clipped_by {
		box { <0,-1,0>, <1,1,-1> inverse }
		prism {
			linear_spline
			0, 1, 4,
			<0,0>,<0,-1>,<1,-1>,<1,0.6666>
			inverse
		}
	}
	pigment {
		color <1.0, 1.0, 1.0>
	}
}

sphere {
	<0, 0, 0>, 0.01
	pigment {
		color <1, 0, 0>
	}
}

cylinder {
	<0,0,0>, <0.3, 0,0>, 0.007
	pigment {
		color <1, 0, 0>
	}
}

sphere {
	<0.3, 0, 0>, 0.01
	pigment {
		color <1, 0, 0>
	}
}

cylinder {
	<0.3,0,0>, <0.3, 0,0.2>, 0.007
	pigment {
		color <1, 0, 0>
	}
}

sphere {
	<0.3, 0, 0.2>, 0.01
	pigment {
		color <1, 0, 0>
	}
}

cylinder {
	<0.3,0.15,0.2>, <0.3, 0,0.2>, 0.007
	pigment {
		color <1, 0, 0>
	}
}

sphere {
	<0.3, 0.15,0.2>, 0.01
	pigment {
		color <1, 0, 0>
	}
}

cylinder {
	<0,0,0>, <0.55,0,0>, 0.004
	pigment {
		color <0,0,1>
	}
}

cone {
	<0.55,0,0>, 0.01, <0.60,0,0>, 0
	pigment {
		color <0,0,1>
	}
}

cylinder {
	<0,0,0>, <0,0.3,0>,0.004
	pigment {
		color <0,0,1>
	}
}

cone {
	<0,0.3,0>, 0.01, <0,0.35,0>, 0
	pigment {
		color <0,0,1>
	}
}

cylinder {
	<0,0,0>, <0,0,-0.67>,0.004
	pigment {
		color <0,0,1>
	}
}

cone {
	<0,0,-0.67>, 0.01, <0,0,-0.73>, 0
	pigment {
		color <0,0,1>
	}
}
