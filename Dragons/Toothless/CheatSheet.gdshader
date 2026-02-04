shader_type spatial;
render_mode cull_disabled;

uniform float time;
uniform float amplitude = 0.1;
uniform float speed = 50;
uniform float frequency = 30;
uniform float x_max = 1.355;
uniform float attached_width = 0.04;
uniform float offset = 3.5;
uniform sampler2D albedo_texture;
uniform vec3 albedo_color = vec3(1.0);
uniform float brightness = 0.18;
uniform float saturation = 1.3;

void vertex() 
{
    // calculate the distance from the vertex to the fixed edge
    float x_offset = clamp((x_max - attached_width - VERTEX.x), 0.0, x_max - attached_width);
    float factor = x_offset / (x_max - attached_width); // from 0 (fixed) to 1 (maximum sway)
    // wave function to simulate the swaying effect
    float wave = sin(VERTEX.z * frequency + time * speed);
    // change the amplitude based on the distance from the fixed edge
    VERTEX.y += wave * factor * amplitude;
    // add an offset along the Y axis, larger when X is smaller (away from the fixed edge)
    VERTEX.y += offset * factor * factor;
}

void fragment() 
{
    vec3 tex_color = texture(albedo_texture, UV).rgb;
    float gray = dot(tex_color, vec3(0.299, 0.587, 0.114));
    tex_color = mix(vec3(gray), tex_color, saturation);
    ALBEDO = tex_color * albedo_color * brightness;
}