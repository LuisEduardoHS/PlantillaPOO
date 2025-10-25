#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;

float random(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec2 uv = TexCoords;

    float glitch = step(0.95, random(vec2(time, uv.y * 10.0)));
    uv.x += glitch * (random(uv * time) - 0.5) * 0.1;

    float r = texture(screenTexture, uv + vec2(0.005 * sin(time*5.0), 0.0)).r;
    float g = texture(screenTexture, uv).g;
    float b = texture(screenTexture, uv - vec2(0.005 * cos(time*5.0), 0.0)).b;

    vec3 color = vec3(r, g, b);

    float noise = random(uv + time) * 0.08;
    color += noise;

    float scanline = 0.9 + 0.1 * sin(uv.y * 800.0);
    color *= scanline;

    FragColor = vec4(color, 1.0);
}
