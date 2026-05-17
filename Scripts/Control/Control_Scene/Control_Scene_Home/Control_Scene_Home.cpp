#include "Control_Scene_Home.h"
#include "Menu.h"
#include "Settings.h"
#include "Control_Main.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>

using namespace godot;

// ================================================================
// 构造 / 析构
// ================================================================

Control_Scene_Home::Control_Scene_Home()
{
}

Control_Scene_Home::~Control_Scene_Home()
{
    // Menu is a child Node; scene tree handles its lifetime.
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
        menu = memnew(Menu);
    }
    menu->Initialize(this, settings, control_main, viewport_container);
}
