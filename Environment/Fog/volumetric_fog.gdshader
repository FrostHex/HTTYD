shader_type fog;

/* main fog shader */
uniform float base_density: hint_range(0, 1) = 0.8;
uniform vec3 base_color: source_color;
uniform float fade_out_height = 2.0;
uniform float fade_out_distance = 1.5;

/* gradient_noise_3d.gdshaderinc */
// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// All noise functions here:
//
// https://www.shadertoy.com/playlist/fXlXzf&from=0&num=12

void fog() {
	vec3 base_pos = WORLD_POSITION * vec3(0.1, 0.5, 0.1);
	DENSITY = base_density;
	DENSITY *= 1.0 - smoothstep(fade_out_height, fade_out_height + fade_out_distance, WORLD_POSITION.y);
	ALBEDO = base_color;
}