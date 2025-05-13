#include "TransformApplier.h"
#include <godot_cpp/classes/node3d.hpp>

void TransformApplier::_bind_methods() {
    // 可选方法绑定
}

std::vector<TransformData> TransformApplier::load_transforms_ordered(const String &path) {
    std::vector<TransformData> result;
    Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
    if (!file.is_valid()) {
        UtilityFunctions::printerr("Failed to open file: ", path);
        return result;
    }
    TransformData current;
    int found = 0;
    while (!file->eof_reached()) {
        String line = file->get_line().strip_edges();
        if (line.begins_with("Position:")) {
            PackedStringArray parts = line.replace("Position:", "").strip_edges().split(",");
            if (parts.size() == 3) {
                current.position = Vector3(parts[0].to_float(), parts[1].to_float(), parts[2].to_float());
                found |= 1;
            }
        } else if (line.begins_with("Rotation (Quaternion):")) {
            PackedStringArray parts = line.replace("Rotation (Quaternion):", "").strip_edges().split(",");
            if (parts.size() == 4) {
                current.rotation = Quaternion(parts[0].to_float(), parts[1].to_float(), parts[2].to_float(), parts[3].to_float());
                found |= 2;
            }
        } else if (line.begins_with("Scale:")) {
            PackedStringArray parts = line.replace("Scale:", "").strip_edges().split(",");
            if (parts.size() == 3) {
                current.scale = Vector3(parts[2].to_float(), parts[0].to_float(), parts[1].to_float());
                found |= 4;
            }
        } else if (line.begins_with("[")) {
            // 新对象开始，保存上一个
            if (found == 7) result.push_back(current);
            found = 0;
        }
    }
    if (found == 7) result.push_back(current);
    return result;
}

void TransformApplier::_ready() {
    String file_path = ProjectSettings::get_singleton()->globalize_path("E:/Projects/.History/HTTYD_Unity/Assets/godot_transform_export.txt");
    std::vector<TransformData> transforms = load_transforms_ordered(file_path);
    int t_index = 0;
    for (int i = 0; i < get_parent()->get_child_count(); ++i) {
        Node *child = get_parent()->get_child(i);
        Node3D *node = Object::cast_to<Node3D>(child);
        if (node && t_index < transforms.size()) {
            Transform3D t = node->get_transform();
            t.origin = transforms[t_index].position;
            t.basis = Basis(transforms[t_index].rotation).scaled(transforms[t_index].scale);
            node->set_transform(t);
            UtilityFunctions::print("Set transform for node ", node->get_name(), " pos=", transforms[t_index].position, " rot=", transforms[t_index].rotation, " scale=", transforms[t_index].scale);
            t_index++;
        }
    }
    if (t_index < transforms.size()) {
        UtilityFunctions::printerr("Warning: Not enough 3D nodes to apply all transforms.");
    }
}