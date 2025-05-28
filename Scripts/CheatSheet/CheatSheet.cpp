#include "CheatSheet.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>

using namespace godot;

void CheatSheet::_bind_methods()
{
}

CheatSheet::CheatSheet()
{
}

CheatSheet::~CheatSheet()
{
}

void CheatSheet::_ready() 
{
    MeshInstance3D* mesh = get_parent()->get_node<Node>("Pivot")->get_node<Node>("Toothless")->get_node<Node>("rig")
                         ->get_node<Node>("Skeleton3D")->get_node<MeshInstance3D>("cheat_sheet");
    if (mesh) 
    {
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
        float z_max = 0.0f;
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
                        z_max = vertices[0].z;
                        for (int i = 1; i < vertices.size(); ++i) 
                        {
                            if (vertices[i].z > z_max) z_max = vertices[i].z;
                        }
                    }
                }
            }
        }
        if (material.is_valid()) 
        {
            material->set_shader_parameter("z_max", z_max);
            if (albedo_tex.is_valid()) 
            {
                material->set_shader_parameter("albedo_texture", albedo_tex); // use the original texture
            }
        }
    }
}

void CheatSheet::_physics_process(double delta)
{
    if (material.is_valid())
    {
        double time_sec = Time::get_singleton()->get_ticks_msec() / 1000.0;
        material->set_shader_parameter("time", time_sec);
        // UtilityFunctions::print("Time in seconds: ", time_sec);
    }
}