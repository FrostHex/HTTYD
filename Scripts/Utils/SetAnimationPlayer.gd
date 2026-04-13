# Attaches to an AnimationTree node and automatically assigns the AnimationPlayer

@tool # calls _ready() in editor mode
extends AnimationTree

func _ready() -> void:
	assign_animation_player()

func assign_animation_player() -> void:
	# this script is attached directly to AnimationTree, so the parent is dragon_root.
	var dragon_root: Node = get_parent()
	if dragon_root == null:
		push_warning("SetAnimationPlayer: current AnimationTree has no parent.")
		return

	var model: Node = dragon_root.get_node_or_null("Model")
	if model == null:
		push_warning("SetAnimationPlayer: Model node not found.")
		return

	if model.get_child_count() == 0:
		push_warning("SetAnimationPlayer: Model has no child nodes.")
		return

	var anim_player: AnimationPlayer = model.get_child(0).get_node_or_null("AnimationPlayer") as AnimationPlayer
	if anim_player == null:
		push_warning("SetAnimationPlayer: AnimationPlayer node not found.")
		return

	var target_path: NodePath = get_path_to(anim_player)
	if not get_animation_player().is_empty():
		# print("skipping SetAnimationPlayer: already set to ", get_animation_player())
		return

	set_animation_player(target_path)
	# print("SetAnimationPlayer: animation_player = ", get_animation_player())
