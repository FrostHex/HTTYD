shader_type spatial;
// 去掉 depth_draw_always：半透明面无需强制写深度，减少 early-z 压力
render_mode world_vertex_coords, shadows_disabled, blend_mix;

#define REFLECTANCE 0.02

global uniform vec4 water_color : source_color;

uniform float roughness : hint_range(0.0, 1.0) = 0.4;

// Radial edge fade
uniform float circle_fade_start_ratio : hint_range(0.0, 1.0) = 0.0;
uniform float mesh_half_side          : hint_range(0.001, 1000000.0) = 256.0;
uniform float follow_tile_size        : hint_range(0.001, 1000000.0) = 1.0;

// Fresnel 移到 vertex，每顶点计算一次，省去逐像素 pow/exp
varying float fresnel;
varying vec2  mesh_local_xz;

void vertex() {
    // 在世界坐标偏移前，先取模型本地坐标用于 alpha mask
    mesh_local_xz = (inverse(MODEL_MATRIX) * vec4(VERTEX, 1.0)).xz;
    vec2 model_origin_world = (MODEL_MATRIX * vec4(0.0, 0.0, 0.0, 1.0)).xz;

    // 跟随摄像机（与 background 一致）
    float tile_size = max(follow_tile_size, 1e-5);
    vec2 snapped_camera = ceil(CAMERA_POSITION_WORLD.xz / tile_size) * tile_size;
    VERTEX.xz += snapped_camera - model_origin_world;

    // Fresnel（顶点级别，法线固定朝上，dot(up, toCam) = 视角的 y 分量）
    vec3 to_camera = normalize(CAMERA_POSITION_WORLD - VERTEX);
    float ndotv    = clamp(to_camera.y, 0.0, 1.0);
    float r        = roughness;
    fresnel = mix(
        pow(1.0 - ndotv, 5.0 * exp(-2.69 * r)) / (1.0 + 22.7 * pow(r, 1.5)),
        1.0, REFLECTANCE
    );
}

void fragment() {
    ALBEDO    = water_color.rgb;
    ROUGHNESS = 0.15;

    // 固定朝上法线（无 normal 贴图）
    NORMAL = (VIEW_MATRIX * vec4(0.0, 1.0, 0.0, 0.0)).xyz;

    // 径向渐变透明（inscribed-circle mask）
    float normalized_radius = length(mesh_local_xz) / max(mesh_half_side, 1e-5);
    float fade_start        = clamp(circle_fade_start_ratio, 0.0, 1.0);

    float circle_alpha = 1.0;
    if (normalized_radius >= 1.0) {
        circle_alpha = 0.0;
    } else if (fade_start > 0.0 && normalized_radius > 1.0 - fade_start) {
        float fade_t  = clamp((normalized_radius - 1.0 + fade_start) / max(fade_start, 1e-5), 0.0, 1.0);
        float eased_t = fade_t * fade_t * fade_t * (fade_t * (fade_t * 6.0 - 15.0) + 10.0);
        circle_alpha  = 1.0 - eased_t;
    }

    // 俯视角优化：摄像机几乎垂直朝下时，VIEW 向量的 y 分量（view space）接近 -1。
    // 此时 fresnel 趋近 0，水面视觉贡献极低，直接将 alpha 缩减到接近 0，
    // 触发透明批次的早期跳过，大幅削减 overdraw 和混合开销。
    // VIEW 是 view space 下从片元到摄像机的单位向量，垂直俯视时 VIEW.y ≈ 1（Godot view space y 朝上）。
    // top_down_t：0 = 斜视，1 = 完全俯视（VIEW.y > topdown_threshold 时线性升至 1）
    const float topdown_threshold = 0.85; // 对应约 32° 以内的仰角开始渐隐
    const float topdown_hard      = 0.97; // 对应约 14° 完全透明
    float top_down_t = clamp((VIEW.y - topdown_threshold) / (topdown_hard - topdown_threshold), 0.0, 1.0);
    float view_alpha = 1.0 - top_down_t * top_down_t; // 平方缓出，过渡自然

    ALPHA = circle_alpha * water_color.a * view_alpha;
}
