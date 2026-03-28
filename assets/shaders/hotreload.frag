#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform float uTime;
uniform vec2 uResolution;

void main()
{
    vec2 uv = vUV;
    vec2 p = (uv - 0.5) * 2.0;
    p.x *= uResolution.x / max(uResolution.y, 1.0);

    float ring = 0.5 + 0.5 * sin(14.0 * length(p) - uTime * 2.5);
    float scan = 0.5 + 0.5 * sin((uv.y + uTime * 0.4) * 120.0);
    vec3 base = mix(vec3(0.08, 0.10, 0.17), vec3(0.18, 0.75, 0.95), ring);
    base *= 0.9 + 0.1 * scan;

    FragColor = vec4(base, 1.0);
}
