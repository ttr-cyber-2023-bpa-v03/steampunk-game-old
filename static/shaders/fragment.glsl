precision highp float;

void main() {
	const vec2 squarePos = vec2(100.0, 100.0);
	const vec2 squareSize = vec2(100.0, 100.0);

	const vec2 lightPos = vec2(250.0, 250.0);
	const float lightRadius = 100.0;
	const float lightDamping = 0.4;

	const vec2 lightSquarePos = vec2(200.0, 200.0);
	const vec2 lightSquareSize = vec2(100.0, 100.0);

	// check if in light square (1px border inside)
	if (gl_FragCoord.x > lightSquarePos.x && gl_FragCoord.x < lightSquarePos.x + 1.0) {
		if (gl_FragCoord.y > lightSquarePos.y && gl_FragCoord.y < lightSquarePos.y + lightSquareSize.y) {
			gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
		return;
	}

	// im lazy so i calculate to put square here
	// dont ever branch in shaders unless you have to
	if (gl_FragCoord.x < squarePos.x || gl_FragCoord.x > squarePos.x + squareSize.x || gl_FragCoord.y < squarePos.y || gl_FragCoord.y > squarePos.y + squareSize.y) {
		// continnue shading of background
		vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
		
		// calculate distance to light
		float dist = distance(gl_FragCoord.xy, lightPos);
		
		// apply damping with sine
		dist = dist - lightDamping * cos(dist);

		// calculate light
		float light = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);
		color = color + vec4(light, light, light, 1.0);



		gl_FragColor = color;
		return;
	}

    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
