#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

using namespace godot;

struct TransformData {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
};

class TransformApplier : public Node3D {
    GDCLASS(TransformApplier, Node3D);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    static std::vector<TransformData> load_transforms_ordered(const String &path);

private:
    std::vector<Quaternion> load_quaternions_ordered(const String &path);
};
