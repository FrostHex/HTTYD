#include "CheatSheet.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


CheatSheet::CheatSheet() : state_current(STATE_ATTACHED)
{
    state_process_funcs[STATE_ATTACHED] = &CheatSheet::ProcessAttached;
    state_process_funcs[STATE_DETATCHED] = &CheatSheet::ProcessDetatched;
    state_process_funcs[STATE_HELD] = &CheatSheet::ProcessHeld;
    state_process_funcs[STATE_MOUTHED] = &CheatSheet::ProcessMouthed;
    state_process_funcs[STATE_HELD_CRISIS] = &CheatSheet::ProcessHeldCrisis;
    state_process_funcs[STATE_DISCARDED] = &CheatSheet::ProcessDiscarded;
}


CheatSheet::~CheatSheet()
{
}


void CheatSheet::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

    mesh = get_parent()->get_node<MeshInstance3D>("SpeciesSlot/ToothlessRoot/Model/Toothless/rig/Skeleton3D/cheat_sheet");
    dragon = get_parent()->get_parent()->get_node<RigidBody3D>("Dragon");
    pickable = nullptr;
    call_deferred("SetupPickable");

    // get the override material first, otherwise get the surface material
    Ref<Material> base_material = mesh->get_surface_override_material(0);
    if (!base_material.is_valid()) 
    {
        Ref<Mesh> mesh_res = mesh->get_mesh();
        if (mesh_res.is_valid()) 
        {
            base_material = mesh_res->surface_get_material(0);
        }
    }
    Ref<Texture2D> albedo_tex;
    if (base_material.is_valid()) 
    {
        // try to get the albedo texture from StandardMaterial3D
        Ref<StandardMaterial3D> std_mat = base_material;
        if (std_mat.is_valid()) 
        {
            albedo_tex = std_mat->get_texture(StandardMaterial3D::TEXTURE_ALBEDO);
        } 
        else 
        {
            // try to get the albedo texture from ShaderMaterial
            Ref<ShaderMaterial> shader_mat = base_material;
            if (shader_mat.is_valid()) 
            {
                Variant v = shader_mat->get_shader_parameter("albedo_texture");
                if (v.get_type() == Variant::OBJECT) 
                {
                    Object* obj = v;
                    albedo_tex = Ref<Texture2D>(Object::cast_to<Texture2D>(obj));
                }
            }
        }
    }
    material = mesh->get_material_override();
    material->set_shader_parameter("time", 0.0f);
    Ref<Mesh> mesh_res = mesh->get_mesh();
    float x_max = 0.0f;
    if (mesh_res.is_valid()) 
    {
        // only process ArrayMesh
        ArrayMesh* arr_mesh = Object::cast_to<ArrayMesh>(*mesh_res);
        if (arr_mesh && arr_mesh->get_surface_count() > 0) 
        {
            // only process the first surface
            Array arr = arr_mesh->surface_get_arrays(0);
            if (arr.size() > Mesh::ARRAY_VERTEX) 
            {
                PackedVector3Array vertices = arr[Mesh::ARRAY_VERTEX];
                if (vertices.size() > 0) 
                {
                    x_max = vertices[0].x;
                    for (int i = 1; i < vertices.size(); ++i) 
                    {
                        if (vertices[i].x > x_max) x_max = vertices[i].x;
                    }
                }
            }
        }
    }
    if (material.is_valid()) 
    {
        material->set_shader_parameter("x_max", x_max);
        if (albedo_tex.is_valid()) 
        {
            material->set_shader_parameter("albedo_texture", albedo_tex); // use the original texture
        }
    }
}


void CheatSheet::Detatch()
{
    Vector3 rotation = mesh->get_global_rotation();
    detatch_direction = mesh->get_global_transform().basis.get_column(1);
    mesh->get_parent()->remove_child(mesh);
    pickable->call_deferred("add_child", mesh);
    pickable->set_position(detatch_position);
    pickable->set_rotation(Vector3(0, 0, 0));
    mesh->set_position(Vector3(-0.33f - 0.45f, -1.3f - 0.65f, 0.0f) - pickable->get_position());
    state_current = STATE_DETATCHED;
}


void CheatSheet::_physics_process(double delta)
{
    if (material.is_valid())
    {
        double time_sec = Time::get_singleton()->get_ticks_msec() / 1000.0;
        material->set_shader_parameter("time", time_sec);
    }
    (this->*state_process_funcs[state_current])(delta);
}


void CheatSheet::ProcessAttached(double delta)
{
}


void CheatSheet::ProcessDetatched(double delta)
{
    pickable->set_position(detatch_position);

    // rotate the cheat sheet randomly
    Transform3D transform = pickable->get_transform();
    float random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(0), random_angle); // rotate around x-axis
    random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(1), random_angle); // rotate around y-axis
    random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(2), random_angle); // rotate around z-axis
    pickable->set_transform(transform);
    // move the cheat sheet towards the ground
    Vector3 global_pos = pickable->get_global_position();
    global_pos += detatch_direction * flutter_speed;
    global_pos.y -= 0.01f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * flutter_speed;
    pickable->set_global_position(global_pos);

    detatch_position = pickable->get_position();
}


void CheatSheet::ProcessHeld(double delta)
{
}

void CheatSheet::ProcessMouthed(double delta)
{
    pickable->set_position(detatch_position);
    pickable->set_rotation(detatch_rotation);
}


void CheatSheet::ProcessHeldCrisis(double delta)
{
}


void CheatSheet::ProcessDiscarded(double delta)
{
    // rotate the cheat sheet randomly
    Transform3D transform = pickable->get_transform();
    float random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(0), random_angle); // rotate around x-axis
    random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(1), random_angle); // rotate around y-axis
    random_angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.15f;
    transform.basis = transform.basis.rotated(transform.basis.get_column(2), random_angle); // rotate around z-axis
    pickable->set_transform(transform);
    // move the cheat sheet
    pickable->set_position(detatch_position + dragon->get_position());
    detatch_position += Vector3(0.0f, 0.005f, 0.0f) - dragon->get_linear_velocity() * 0.001f;
    delete_count++;
    if (delete_count > 300) // 5 seconds in 60 FPS
    {
        // UtilityFunctions::print("Cheat Sheet is deleted.");
        pickable->get_parent()->remove_child(pickable);
        pickable->queue_free();
        delete_count = 0;
        set_physics_process(false);
    }
}


void CheatSheet::_on_pickable_picked_up(Node* pickable)
{
    if (state_current == STATE_DETATCHED) 
    {
        state_current = STATE_HELD;
        // UtilityFunctions::print("Cheat Sheet is now held.");
    }
    else if (state_current == STATE_MOUTHED) 
    {
        state_current = STATE_HELD_CRISIS;
        // UtilityFunctions::print("Cheat Sheet is now held in crisis.");
    }
}


void CheatSheet::_on_pickable_dropped(Node* pickable)
{
    if (state_current == STATE_HELD) 
    {
        state_current = STATE_MOUTHED;
        Node* camera_main = get_parent()->get_parent()->get_node_or_null("Camera_Main");
        if (camera_main && camera_main->has_node("XR")) // change the parent to XR camera if it exists
        {
            Transform3D global_pos = this->pickable->get_global_transform();
            this->pickable->get_parent()->remove_child(this->pickable);
            get_parent()->get_parent()->get_node<Node>("Camera_Main/XR/XROrigin/XRCamera")->add_child(this->pickable);
            this->pickable->set_global_transform(global_pos);
        }
        detatch_position = this->pickable->get_position();
        detatch_rotation = this->pickable->get_rotation();
        // UtilityFunctions::print("Cheat Sheet is now mouthed.");
    }
    else if (state_current == STATE_HELD_CRISIS) 
    {
        state_current = STATE_DISCARDED;
        // change the parent to the main node
        Transform3D global_pos = this->pickable->get_global_transform();
        this->pickable->get_parent()->remove_child(this->pickable);
        get_parent()->get_parent()->add_child(this->pickable);
        this->pickable->set_global_transform(global_pos);
        detatch_position = this->pickable->get_position() - dragon->get_position();
        // UtilityFunctions::print("Cheat Sheet is now detached.");
    }
}

void CheatSheet::SetupPickable()
{
    pickable = Object::cast_to<RigidBody3D>(get_parent()->get_node_or_null("SpeciesSlot/ToothlessRoot/Sockets/Socket_Back_Mount/Socket_Back/Camera_Main/XRToolsPickable"));
    if (!pickable)
    {
        UtilityFunctions::printerr("CheatSheet: XRToolsPickable not found after deferred setup.");
        return;
    }

    Callable on_picked = Callable(this, "_on_pickable_picked_up");
    Callable on_dropped = Callable(this, "_on_pickable_dropped");
    if (!pickable->is_connected("picked_up", on_picked))
    {
        pickable->connect("picked_up", on_picked);
    }
    if (!pickable->is_connected("dropped", on_dropped))
    {
        pickable->connect("dropped", on_dropped);
    }
}


void CheatSheet::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Detatch"), &CheatSheet::Detatch);
    ClassDB::bind_method(D_METHOD("SetupPickable"), &CheatSheet::SetupPickable);
    ClassDB::bind_method(D_METHOD("_on_pickable_picked_up", "pickable"), &CheatSheet::_on_pickable_picked_up);
    ClassDB::bind_method(D_METHOD("_on_pickable_dropped", "pickable"), &CheatSheet::_on_pickable_dropped);

    BIND_ENUM_CONSTANT(STATE_ATTACHED);
    BIND_ENUM_CONSTANT(STATE_DETATCHED);
    BIND_ENUM_CONSTANT(STATE_HELD);
    BIND_ENUM_CONSTANT(STATE_MOUTHED);
    BIND_ENUM_CONSTANT(STATE_HELD_CRISIS);
    BIND_ENUM_CONSTANT(STATE_DISCARDED);
}