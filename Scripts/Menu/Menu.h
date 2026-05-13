#ifndef MENU_H
#define MENU_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot
{
	class Settings;
	class Control_Main;
	class Control;
	class Panel;
	class Label;
	class TextureRect;
	class InputEvent;

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

	class Menu : public Node
	{
		GDCLASS(Menu, Node);

	public:
		Menu();
		~Menu();

		void Initialize(Node* owner, Settings* settings, Control_Main* control_main, Node* viewport_container);

		void _process(double delta) override;
		void _input(const Ref<InputEvent>& event) override;

	protected:
		static void _bind_methods();

	private:

		// ── 基础引用 ────────────────────────────────────────
		Settings*       settings        = nullptr;
		Control_Main*   control_main    = nullptr;

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
			String      member_name;    // Settings member (snake_case) for JSON key lookup
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
			Control*    root_ctrl    = nullptr;  // 卡牌根 Control 节点
			Panel*      panel        = nullptr;  // 卡牌底板（Panel）
			Label*      title_label  = nullptr;  // 顶部标题 Label
			Label*      value_label  = nullptr;  // 中央值 Label
			Label*      detail_label = nullptr;  // 具体描述 Label

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

		static constexpr float CARD_WIDTH           = 180.f;
		static constexpr float CARD_HEIGHT          = 250.f;
		static constexpr float FAN_RADIUS           = 2000.f;   // 扇形虚拟圆半径
		static constexpr float FAN_MAX_ANGLE_DEG    = 18.5f;    // 扇形总角度（度）
		static constexpr float HOVER_SCALE          = 1.5f;
		static constexpr float HOVER_LIFT           = 60.f;    // 悬停时上移像素
		static constexpr float ANIM_SPEED           = 30.f;    // 插值速度

		// ── 卡牌内部区域布局常量 ─────────────────────────────
		// 类型标识（右上角角标）
		static constexpr float TYPE_LABEL_X         = 148.f;
		static constexpr float TYPE_LABEL_Y         = 9.f;
		static constexpr float TYPE_LABEL_W         = 22.f;
		static constexpr float TYPE_LABEL_H         = 22.f;
		static constexpr int   TYPE_LABEL_FONT_SIZE = 18;

		// 顶部标题
		static constexpr float TITLE_X              = 40.f;
		static constexpr float TITLE_Y              = 10.f;
		static constexpr float TITLE_W_MARGIN       = 79.f;     // 左右各留边距，宽度 = CARD_WIDTH - margin
		static constexpr float TITLE_H              = 23.f;
		static constexpr int   TITLE_FONT_SIZE      = 15;

		// 中央值
		static constexpr float VALUE_X              = 10.f;
		static constexpr float VALUE_Y_RATIO        = 0.15f;   // 相对 CARD_HEIGHT 的比例
		static constexpr float VALUE_H_RATIO        = 0.54f;   // 相对 CARD_HEIGHT 的比例
		static constexpr float VALUE_W_MARGIN       = 19.f;
		static constexpr int   VALUE_FONT_SIZE      = 17;

		// 具体描述（卡牌下半部，值标签之后）
		static constexpr float DETAIL_X             = 10.f;
		static constexpr float DETAIL_Y_RATIO       = 0.795f;   // 相对 CARD_HEIGHT 的比例
		static constexpr float DETAIL_H_RATIO       = 0.16f;   // 相对 CARD_HEIGHT 的比例
		static constexpr float DETAIL_W_MARGIN      = 20.f;
		static constexpr int   DETAIL_FONT_SIZE     = 10;
		static constexpr int   DETAIL_LINE_SPACING  = 0;      // detail 文本行距

		Color dark_gray = Color(0.12f, 0.12f, 0.12f, 1.f);

		// ── Debug 开关 ───────────────────────────────────────
		// 若为 true，则将类型标识/标题/值/描述区域染色以便调试布局
		bool debug_ui = false;

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

		void _update_badge_display();
		void _on_language_changed();

		String _get_json_text(const String& key, const String& fallback = "");

		// 从 Settings 读取某属性的当前值并格式化为字符串
		String _format_setting_value(const CardData& data) const;
	};
}

#endif // MENU_H