precision mediump float;

uniform sampler2D shadowMap;

void main() {
    const vec2  lightPos      = vec2(375.0, 325.0);
    const float lightDamping  = 1.0;
    const float lightRadius   = 4.0;
    const float lightSpread   = 0.6;
    const vec3  lightColor    = vec3(0.0, 1.0, 0.0);

    float l2pDist = length(gl_FragCoord.xy - lightPos);

    float attenuation = lightRadius / ((lightDamping * l2pDist) + (0.01 * l2pDist) * l2pDist);
    attenuation = pow(attenuation, lightSpread);

    // Convert fragment position to light's view space
    vec4 fragCoordLightSpace = vec4(gl_FragCoord.xy / vec2(800.0, 600.0), texture2D(shadowMap, gl_FragCoord.xy / vec2(800.0, 600.0)).r, 1.0);

    // Compare depth for shadow calculation
    float visibility = (fragCoordLightSpace.z < fragCoordLightSpace.w) ? 0.5 : 1.0;

    vec3 color = lightColor * attenuation * visibility;

    gl_FragColor = vec4(color, 1.0);
}
 