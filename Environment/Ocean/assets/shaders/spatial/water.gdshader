shader_type spatial;
render_mode world_vertex_coords, shadows_disabled, depth_draw_always;

#define MAX_CASCADES 8
#define REFLECTANCE  0.02

global uniform vec4 water_color : source_color;
global uniform vec4 foam_color : source_color;

uniform float roughness : hint_range(0.0, 1.0) = 0.4;
uniform float normal_strength : hint_range(0.0, 1.0) = 1.0;

// Cascade data
group_uniforms cascade_data;
uniform vec4 map_scales[MAX_CASCADES];
global uniform uint num_cascades;
global uniform sampler2DArray displacements;
global uniform sampler2DArray normals : hint_normal;

// SSR Uniforms
uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_nearest;
uniform sampler2D DEPTH_TEXTURE : hint_depth_texture, filter_nearest;

uniform float ssr_resolution : hint_range(0.01, 2.0) = 0.05;   // 默认值调小，步长更细
uniform float ssr_max_travel : hint_range(50.0, 800.0) = 450.0;
uniform float ssr_max_diff : hint_range(1.0, 80.0) = 40.0;
uniform float ssr_mix_strength : hint_range(0.0, 1.0) = 1.0;
uniform float ssr_fresnel_floor : hint_range(0.0, 1.0) = 0.35; // 反射强度下限，防止俯视角反射消失

varying float wave_height;
varying float foam_factor;
varying float fresnel;

// BICUBIC (保持不变)
vec4 cubic_weights(float a) {
    float a2 = a*a; float a3 = a2*a;
    float w0 = -a3 + a2*3.0 - a*3.0 + 1.0;
    float w1 = a3*3.0 - a2*6.0 + 4.0;
    float w2 = -a3*3.0 + a2*3.0 + a*3.0 + 1.0;
    float w3 = a3;
    return vec4(w0, w1, w2, w3) / 6.0;
}

vec4 texture_bicubic(in sampler2DArray sampler, in vec3 uvw) {
    vec2 dims = vec2(textureSize(sampler, 0).xy);
    vec2 dims_inv = 1.0 / dims;
    uvw.xy = uvw.xy * dims + 0.5;
    vec2 fuv = fract(uvw.xy);
    vec4 wx = cubic_weights(fuv.x);
    vec4 wy = cubic_weights(fuv.y);
    vec4 g = vec4(wx.xz + wx.yw, wy.xz + wy.yw);
    vec4 h = (vec4(wx.yw, wy.yw) / g + vec2(-1.5, 0.5).xyxy + floor(uvw.xy).xxyy) * dims_inv.xxyy;
    vec2 w = g.xz / (g.xz + g.yw);
    return mix(mix(texture(sampler, vec3(h.yw, uvw.z)), texture(sampler, vec3(h.xw, uvw.z)), w.x),
               mix(texture(sampler, vec3(h.yz, uvw.z)), texture(sampler, vec3(h.xz, uvw.z)), w.x), w.y);
}

// VERTEX
void vertex() {
    UV = VERTEX.xz;
    float distance_factor = min(exp(-(length(VERTEX.xz - CAMERA_POSITION_WORLD.xz) - 150.0)*0.007), 1.0);

    vec3 displacement = vec3(0);
    for (uint i = 0U; i < num_cascades; ++i) {
        vec4 scales = map_scales[i];
        displacement += texture(displacements, vec3(UV*scales.xy, float(i))).xyz * scales.z;
    }
    VERTEX += displacement * distance_factor;
    wave_height = displacement.y;
}

// SSR
bool is_within_screen_boundaries(vec2 pos) {
    return pos.x > 0.0 && pos.x < 1.0 && pos.y > 0.0 && pos.y < 1.0;
}

vec2 get_uv_from_view_position(vec3 pos_view, mat4 proj_mat) {
    vec4 clip = proj_mat * vec4(pos_view, 1.0);
    vec2 ndc = clip.xy / clip.w;
    return ndc * 0.5 + 0.5;
}

vec3 get_view_position_from_uv(vec2 uv, float depth, mat4 inv_proj_mat) {
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 view_pos = inv_proj_mat * ndc;
    return view_pos.xyz / view_pos.w;
}

vec4 get_ssr_color(vec3 surface_view_pos, vec3 normal_view, vec3 view_dir, mat4 proj_mat, mat4 inv_proj_mat) {
    vec3 reflect_dir = normalize(reflect(view_dir, normal_view));

    // Godot view space 中 z 轴朝相机为负，反射方向 z > 0 意味着朝向相机背后，
    // 无法在屏幕上找到对应像素，直接返回未命中
    if (reflect_dir.z > 0.0) return vec4(-1.0, -1.0, -1.0, 0.0);

    // offset 保持小值，避免射线起点离水面过远而跳过近处正确命中点
    vec3 ray_pos = surface_view_pos + normal_view * 0.1;

    for (float dist = 0.0; dist < ssr_max_travel; dist += ssr_resolution) {
        // 固定步长：去掉自适应步长，防止远处步长过大跳过细柱子几何
        ray_pos += reflect_dir * ssr_resolution;

        vec2 screen_uv = get_uv_from_view_position(ray_pos, proj_mat);
        if (!is_within_screen_boundaries(screen_uv)) break;

        float sample_depth = texture(DEPTH_TEXTURE, screen_uv).x;
        vec3 sample_pos = get_view_position_from_uv(screen_uv, sample_depth, inv_proj_mat);

        // Godot view space: z 为负值，越远离相机越负
        // ray_pos.z - sample_pos.z > 0 表示射线已穿过物体表面（射线比采样点更远），即命中
        float depth_diff = ray_pos.z - sample_pos.z;

        if (depth_diff > 0.0 && depth_diff < ssr_max_diff) {
            return vec4(texture(SCREEN_TEXTURE, screen_uv).rgb, 1.0);
        }
    }
    return vec4(-1.0, -1.0, -1.0, 0.0);
}

// FRAGMENT
void fragment() {
    float map_size = float(textureSize(normals, 0).x);
    float dist = length(VERTEX.xz);

    vec3 gradient = vec3(0);
    for (uint i = 0U; i < num_cascades; ++i) {
        vec4 scales = map_scales[i];
        vec3 coords = vec3(UV*scales.xy, float(i));
        float ppm = map_size * min(scales.x, scales.y);
        gradient += mix(texture_bicubic(normals, coords), texture(normals, coords), min(1.0, ppm*0.1)).xyw * vec3(scales.ww, 1.0);
    }

    foam_factor = smoothstep(0.0, 1.0, gradient.z*0.75) * exp(-dist*0.005);
    ALBEDO = mix(water_color.rgb, foam_color.rgb, foam_factor);

    gradient *= mix(0.015, normal_strength, exp(-dist*0.0175));
    NORMAL = (VIEW_MATRIX * vec4(normalize(vec3(-gradient.x, 1.0, -gradient.y)), 0.0)).xyz;

    fresnel = mix(
        pow(1.0 - dot(VIEW, NORMAL), 5.0*exp(-2.69*roughness)) / (1.0 + 22.7*pow(roughness, 1.5)), 
        1.0, REFLECTANCE
    );
    ROUGHNESS = (1.0 - fresnel) * foam_factor + 0.15;

    // SSR
    vec3 surface_view_pos = get_view_position_from_uv(SCREEN_UV, FRAGCOORD.z, INV_PROJECTION_MATRIX);
    vec4 ssr = get_ssr_color(surface_view_pos, NORMAL, VIEW, PROJECTION_MATRIX, INV_PROJECTION_MATRIX);

    // fresnel 加下限，防止俯视角时 fresnel 趋近 0 导致反射颜色贡献极弱
    float fresnel_boosted = max(fresnel, ssr_fresnel_floor);

    if (ssr.x >= 0.0) {
        ALBEDO = mix(ALBEDO, ssr.rgb, ssr_mix_strength * fresnel_boosted);
    } else {
        // ALBEDO = vec3(1.0, 0.0, 0.0);   // 红色 = 未命中
    }

    ALPHA = 1.0;
}