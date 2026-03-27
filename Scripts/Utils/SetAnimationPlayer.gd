# Attaches to an AnimationTree node and automatically assigns the AnimationPlayer

@tool # calls _ready() in editor mode
extends AnimationTree

func _ready() -> void:
	assign_animation_player()

func assign_animation_player() -> void:
	# 当前脚本直接挂在 AnimationTree 上，因此父节点就是 dragon_root。
	var dragon_root: Node = get_parent()
	if dragon_root == null:
		push_warning("SetAnimationPlayer: 当前 AnimationTree 没有父节点。")
		return

	var model: Node = dragon_root.get_node_or_null("Model")
	if model == null:
		push_warning("SetAnimationPlayer: 未找到 Model 节点。")
		return

	if model.get_child_count() == 0:
		push_warning("SetAnimationPlayer: Model 没有子节点。")
		return

	var anim_player: AnimationPlayer = model.get_child(0).get_node_or_null("AnimationPlayer") as AnimationPlayer
	if anim_player == null:
		push_warning("SetAnimationPlayer: 未找到 AnimationPlayer 节点。")
		return

	var target_path: NodePath = get_path_to(anim_player)
	if not get_animation_player().is_empty():
		# print("skipping SetAnimationPlayer: already set to ", get_animation_player())
		return

	set_animation_player(target_path)
	# print("SetAnimationPlayer: animation_player = ", get_animation_player())
