#include "Control_Scene_Home.h"
#include "Menu_Home.h"
#include "Settings.h"
#include "Control_Main.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/system_font.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

// ================================================================
// 构造 / 析构
// ================================================================

Control_Scene_Home::Control_Scene_Home()
{
}

Control_Scene_Home::~Control_Scene_Home()
{
    // Menu_Home is a child Node; scene tree handles its lifetime.
    menu = nullptr;
}

// ================================================================
// _bind_methods
// ================================================================

void Control_Scene_Home::_bind_methods()
{
}

// ================================================================
// _ready
// ================================================================

void Control_Scene_Home::_process(double delta)
{
	// Home scene does NOT call _process_top - no escape menu
}

void Control_Scene_Home::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    Control_Scene_Top::_ready();
    SetHomeScene(true);

    // ── 获取 Settings 单例 ──────────────────────────────────
    settings = Settings::GetSingleton();
    if (!settings)
    {
        UtilityFunctions::printerr("Control_Scene_Home: Could not find Settings singleton");
        return;
    }

    // ── 获取 Control_Main 引用 ──────────────────────────────
    SceneTree *tree = get_tree();
    if (tree)
    {
        Window *root = tree->get_root();
        if (root)
        {
            control_main = Object::cast_to<Control_Main>(
                root->get_node_or_null(NodePath("Main/Control_Main")));
            if (!control_main)
            {
                UtilityFunctions::printerr(
                    "Control_Scene_Home: Could not find Control_Main at Main/Control_Main");
            }
        }
    }

    // ── 初始化 HomeText / HomePaper（按语言）──────────────
    Node *parent_node = get_parent();
    if (parent_node)
    {
        home_paper = Object::cast_to<Node3D>(
            parent_node->get_node_or_null(NodePath("HomePaper")));
        if (!home_paper)
        {
            UtilityFunctions::printerr("Control_Scene_Home: Could not find HomePaper node");
        }

        home_label = Object::cast_to<Label3D>(
            parent_node->get_node_or_null(NodePath("HomePaper/HomeText")));
        if (!home_label)
        {
            UtilityFunctions::printerr("Control_Scene_Home: Could not find HomeText label");
        }
    }

    if (home_paper)
    {
        bool show_home_paper = settings && settings->IsFirstRun();
        home_paper->set_visible(show_home_paper);
    }

    String json_file = "res://Media/Text/English.json";
    if (settings && settings->GetValLanguage() == 1)
    {
        json_file = "res://Media/Text/Chinese.json";
    }

    // If Chinese, apply a system font fallback stack (same as tutorial).
    if (settings && home_label && settings->GetValLanguage() == 1)
    {
        Ref<SystemFont> zh_font;
        zh_font.instantiate();
        PackedStringArray font_names;
        font_names.push_back("SimHei");
        font_names.push_back("Microsoft YaHei");
        zh_font->set_font_names(font_names);
        home_label->set_font(zh_font);
    }

    Ref<FileAccess> f = FileAccess::open(json_file, FileAccess::READ);
    if (f.is_valid())
    {
        String content = f->get_as_text();
        f->close();

        Ref<JSON> json = memnew(JSON);
        Error parse_result = json->parse(content);
        if (parse_result == OK)
        {
            Dictionary data = json->get_data();
            if (data.has("home"))
            {
                Array home_array = data["home"];
                if (home_label && home_array.size() > 0)
                {
                    home_label->set_text(String(home_array[0]));
                }
            }
        }
        else
        {
            UtilityFunctions::printerr("Failed to parse JSON file: " + json_file);
        }
    }
    else
    {
        UtilityFunctions::printerr("Failed to open JSON file: " + json_file);
    }

    // ── 处理 XR / 普通视口 ──────────────────────────────────
    viewport_container = get_parent()->get_node<Node>("SubViewportContainer");
    if (settings->GetValEnableHeadset())
    {
        Node* canvas_layer = viewport_container->get_node<Node>("Viewport/CanvasLayer");
        viewport_container = get_parent()->get_node<Node>("XRToolsViewport2DIn3D");
        canvas_layer->reparent(viewport_container->get_node<Node>("Viewport"));

        Ref<XRInterface> primary = XRServer::get_singleton()->get_primary_interface();
        bool xr_active = primary.is_valid() && primary->is_initialized();
        if (!xr_active)
        {
            Node *xr_viewport    = viewport_container->get_node<Node>("Viewport");
            Node *canvas_in_xr   = xr_viewport->get_node<Node>("CanvasLayer");
            Node *base_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node *base_viewport  = base_container->get_node<Node>("Viewport");
            canvas_in_xr->reparent(base_viewport);
            viewport_container = base_container;
            UtilityFunctions::print("XR initialization failed, restored UI to SubViewportContainer.");
        }
    }

    // ── 构建卡牌手牌根节点并建立主菜单 ──────────────────────
    if (!menu)
    {
        menu = memnew(Menu_Home);
    }
    menu->Initialize(this, settings, control_main, viewport_container);
}
