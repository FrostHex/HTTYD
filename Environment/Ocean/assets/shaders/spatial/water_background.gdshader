shader_type spatial;
render_mode world_vertex_coords, shadows_disabled;
/**
 * Water lighting model roughly follows the 2019 GDC talk: "Wakes, Explosions and Lighting: Interactive Water Simulation in Atlas".
 * Source: https://gpuopen.com/gdc-presentations/2019/gdc-2019-agtd6-interactive-water-simulation-in-atlas.pdf
 */

#define MAX_CASCADES 8    // I am unsure how to generalize this.
#define REFLECTANCE  0.02 // Reflectance from air to water (eta=1.33).

global uniform vec4 water_color : source_color;

uniform float roughness : hint_range(0.0, 1.0) = 0.4;
uniform float normal_strength : hint_range(0.0, 1.0) = 1.0; // Global normal strength
uniform float circle_fade_start_ratio : hint_range(0.0, 1.0) = 0.0;
uniform float mesh_half_side : hint_range(0.001, 1000000.0) = 256.0;
uniform float follow_tile_size : hint_range(0.001, 1000000.0) = 1.0;

group_uniforms cascade_data;
uniform vec4 map_scales[MAX_CASCADES];               // Scales for displacement/normal maps. Packed: [uv scale, displacement scale, normal scale]
global uniform uint num_cascades;
global uniform sampler2DArray displacements;         // Each layer represents one wave cascade.

varying float fresnel;
varying vec2 mesh_local_xz;

void vertex()
{
	// Cache local mesh coordinates before camera-follow offset so alpha mask behavior stays unchanged.
	mesh_local_xz = (inverse(MODEL_MATRIX) * vec4(VERTEX, 1.0)).xz;

	// Follow camera using snapped world tiles, same behavior style as water clipmap movement.
	float tile_size = max(follow_tile_size, 1e-5);
	vec2 snapped_camera = ceil(CAMERA_POSITION_WORLD.xz / tile_size) * tile_size;
	vec2 mesh_origin_world = (MODEL_MATRIX * vec4(0.0, 0.0, 0.0, 1.0)).xz;
	VERTEX.xz += snapped_camera - mesh_origin_world;
}

void fragment() 
{
	float dist = length(VERTEX.xz);

	ALBEDO = water_color.rgb;

	// Inscribed-circle mask in mesh local space.
	// Use local geometry size instead of UVs to avoid full transparency on tiled/non-0..1 UVs.
	float normalized_radius = length(mesh_local_xz) / max(mesh_half_side, 1e-5);
	float fade_start = clamp(circle_fade_start_ratio, 0.0, 1.0);

	float circle_alpha = 1.0;
	if (normalized_radius >= 1.0)
	{
		circle_alpha = 0.0;
	}
	else if (fade_start > 0.0)
	{
		// fade_start defines where fading begins as a ratio of inscribed-circle radius.
		// Example: fade_start=0.5 => radius < 0.5 is fully opaque, then linearly fades to 0 at radius=1.
		if (normalized_radius > 1.0 - fade_start)
		{
			float fade_t = clamp((normalized_radius - 1.0 + fade_start) / max(fade_start, 1e-5), 0.0, 1.0);
			// Quintic smootherstep: endpoint derivatives are zero and visually less abrupt.
			float eased_t = fade_t * fade_t * fade_t * (fade_t * (fade_t * 6.0 - 15.0) + 10.0);
			circle_alpha = 1.0 - eased_t;
		}
	}
	ALPHA = circle_alpha;

	// discontinue the use of normals, set it to a fixed upward normal
	NORMAL = (VIEW_MATRIX * vec4(vec3(0.0, 1.0, 0.0), 0.0)).xyz;

	fresnel = mix(pow(1.0 - dot(VIEW, NORMAL), 5.0*exp(-2.69*roughness)) / (1.0 + 22.7*pow(roughness, 1.5)), 1.0, REFLECTANCE);
	ROUGHNESS = 0.4; // Roughness is proportional to foam/fog amount and fresnel.
}

float smith_masking_shadowing(in float cos_theta, in float alpha) 
{
	float a = cos_theta / (alpha * sqrt(1.0 - cos_theta*cos_theta)); // Approximate: 1.0 / (alpha * tan(acos(cos_theta)))
	float a_sq = a*a;
	return a < 1.6 ? (1.0 - 1.259*a + 0.396*a_sq) / (3.535*a + 2.181*a_sq) : 0.0;
}

// Source: https://github.com/godotengine/godot/blob/7b56111c297f24304eb911fe75082d8cdc3d4141/drivers/gles3/shaders/scene.glsl#L995
float ggx_distribution(in float cos_theta, in float alpha) 
{
	float a_sq = alpha*alpha;
	float d = 1.0 + (a_sq - 1.0) * cos_theta * cos_theta;
	return a_sq / (PI * d*d);
}

void light() 
{
	vec3 halfway = normalize(LIGHT + VIEW);
	float dot_nl = max(dot(NORMAL, LIGHT), 2e-5);
	float dot_nv = max(dot(NORMAL, VIEW), 2e-5);

	// --- SPECULAR ---
	float light_mask = smith_masking_shadowing(dot_nv, roughness);
	float view_mask = smith_masking_shadowing(dot_nl, roughness);
	float microfacet_distribution = ggx_distribution(dot(NORMAL, halfway), roughness);
	float geometric_attenuation = 1.0 / (1.0 + light_mask + view_mask);
	SPECULAR_LIGHT += fresnel * microfacet_distribution * geometric_attenuation * dot_nl / (4.0 * dot_nv + 0.1) * ATTENUATION * LIGHT_COLOR;

	// --- DIFFUSE ---
	const vec3 sss_modifier = vec3(0.9,1.15,0.85); // Subsurface scattering produces a 'greener' color.
	float sss_height = 1.0*max(0.0, 2.5) * pow(max(dot(LIGHT, -VIEW), 0.0), 4.0) * pow(0.5 - 0.5 * dot(LIGHT, NORMAL), 3.0);
	float sss_near = 0.5*pow(dot_nv, 2.0);
	float lambertian = 0.5*dot_nl;
	DIFFUSE_LIGHT += ((sss_height + sss_near) * sss_modifier / (1.0 + light_mask) + lambertian) * (1.0 - fresnel) * ATTENUATION * LIGHT_COLOR;
}
