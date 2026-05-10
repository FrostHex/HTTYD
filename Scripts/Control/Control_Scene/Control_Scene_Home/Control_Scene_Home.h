# ifndef CONTROL_SCENE_HOME_H
# define CONTROL_SCENE_HOME_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include "Control_Top.h"

namespace godot
{
    class Settings;
    class Control_Main;
    class Control;
    class Panel;
    class Label;
    class Tween;

    // ======================================================
    // CardHandUI — 手牌系统
    //
    // 手牌状态机：
    //   HAND_MENU          — 展示主菜单卡牌
    //   HAND_SETTINGS_MAIN — 展示所有设置项卡牌，扇形排列
    //   HAND_SETTINGS_SUB  — 展示某项 enum/int 的子选项，同样扇形排列
    //
    // 每张卡牌由 CardData 描述，CardNode 持有场景节点引用
    // ======================================================

    class Control_Scene_Home : public Control_Top
    {
        GDCLASS(Control_Scene_Home, Node);

    public:
        Control_Scene_Home();
        ~Control_Scene_Home();
        void _ready() override;
        void _process(double delta) override;
        void _input(const Ref<InputEvent>& event) override;

    protected:
        static void _bind_methods();

    private:
        // ── 基础引用 ────────────────────────────────────────
        Settings*       settings        = nullptr;
        Control_Main*   control_main    = nullptr;

        Node*   viewport_container  = nullptr;
        Control* ui_root            = nullptr;
        Node*   settings_panel      = nullptr;   // 整个设置面板容器节点（用于 visible 切换）
        Node*   card_hand_root      = nullptr;   // 手牌根节点（Control，anchored 到底部）
        Node*   badge_icon          = nullptr;

        // ── 卡牌数据结构 ────────────────────────────────────

        // 卡牌类型
        enum CardType
        {
            CARD_MENU,          // 主菜单牌
            CARD_SETTING,       // 主手牌：代表一个设置项
            CARD_OPTION,        // 子手牌：enum/int 某个具体值
            CARD_BACK,          // 子手牌：返回主手牌
        };

        struct CardData
        {
            CardType    type;
            String      prop_name;      // 对应 Settings 属性名，子牌继承父项名
            String      display_name;   // 卡牌顶部标题
            String      value_text;     // 卡牌中央显示值
            int         option_value;   // 仅 CARD_OPTION 有效：该选项对应的 int 值
            int         variant_type;   // Variant::Type
            int         hint;           // PropertyHint
            String      hint_string;
            bool        is_custom;
            String      scene_name;     // CARD_MENU: 目标场景名
            bool        opens_settings = false; // CARD_MENU: 打开设置面板
            bool        shows_badge = false;    // CARD_MENU: 绑定徽章显示
        };

        struct CardNode
        {
            CardData    data;
            Control*    root_ctrl   = nullptr;  // 卡牌根 Control 节点
            Panel*      panel       = nullptr;  // 卡牌底板（Panel）
            Label*      title_label = nullptr;  // 顶部标题 Label
            Label*      value_label = nullptr;  // 中央值 Label

            // 动画目标状态（由 _layout_hand 计算，由 _process 插值）
            Vector2     target_pos;
            float       target_rot  = 0.f;
            Vector2     target_scale = Vector2(1, 1);
            float       target_z    = 0.f;
        };

        // ── 手牌状态 ────────────────────────────────────────

        enum HandState
        {
            HAND_MENU,          // 展示主菜单卡牌
            HAND_SETTINGS_MAIN, // 展示主设置项
            HAND_SETTINGS_SUB,  // 展示 enum/int 子选项
        };

        HandState               hand_state          = HAND_MENU;
        String                  sub_parent_prop;    // 当前展开的父设置项名

        Array                   current_settings_list;  // GetExposedSettings() 缓存

        TypedArray<Dictionary>  card_data_list;     // 当前手牌的 CardData（序列化为 Dictionary）
        Vector<CardNode>        hand_cards;         // 当前手牌节点

        // ── 悬停 / 拖拽状态 ────────────────────────────────

        int     hovered_index       = -1;
        int     dragged_index       = -1;
        Vector2 drag_start_pos;         // 鼠标按下时的手牌局部位置
        Vector2 drag_card_start;        // 卡牌按下时的局部位置
        bool    drag_triggered      = false;    // 是否已超过拖拽阈值
        float   drag_release_height = 80.f;   // 松开触发"使用"的最低上移像素数

        // ── 布局常量 ────────────────────────────────────────

        static constexpr float CARD_WIDTH           = 143.f;
        static constexpr float CARD_HEIGHT          = 208.f;
        static constexpr float FAN_RADIUS           = 1200.f;   // 扇形虚拟圆半径
        static constexpr float FAN_MAX_ANGLE_DEG    = 25.f;    // 扇形总角度（度）
        static constexpr float HOVER_SCALE          = 1.35f;
        static constexpr float HOVER_LIFT           = 60.f;    // 悬停时上移像素
        static constexpr float ANIM_SPEED           = 30.f;    // 插值速度

        // ── 卡牌构建 / 布局 ────────────────────────────────

        void _build_card_hand_root();
        void _set_hand_host(Node* host);
        void _rebuild_menu_hand();
        void _rebuild_settings_main_hand();
        void _rebuild_sub_hand(const String& prop_name, const CardData& parent_data);
        CardNode _create_card_node(const CardData& data, int index);
        void _destroy_hand();
        Vector2 _get_hand_local_mouse_pos() const;

        void _layout_hand();    // 计算每张卡牌的 target_pos / target_rot / target_z
        void _animate_hand(double delta);   // 在 _process 中插值逼近 target

        // ── 卡牌交互 ────────────────────────────────────────

        void _on_card_used(int index);
        void _apply_bool_toggle(const CardData& data);
        void _apply_option_select(const CardData& data);

        void _update_settings_main_value_labels();  // 刷新主手牌卡面中央的当前值文字

        // ── 场景按钮 / 旧有逻辑 ────────────────────────────

        void _on_button_pressed(const String& scene_name);
        void _on_settings_button_pressed();
        void _on_close_button_pressed();
        void _on_sky_time_changed(double value);

        void _update_badge_display();
        void _on_language_changed();

        String _get_json_text(const String& key, const String& fallback = "");
        String _get_setting_display_name(const String& prop_name);

        // _build_settings_entries / _clear_settings_entries / _update_setting_labels /
        // _on_setting_enum_changed / _on_setting_bool_toggled / _refresh_settings_ui
        // 已由卡牌系统取代，不再保留

        // ── 工具函数 ────────────────────────────────────────

        // 将 CamelCase 属性名转换为带空格的显示名
        // 例: "VolumetricClouds" → "Volumetric Clouds"
        static String _camel_to_display(const String& name);

        // 从 Settings 读取某属性的当前值并格式化为字符串
        String _format_setting_value(const CardData& data) const;
    };
}

#endif // CONTROL_SCENE_HOME_H