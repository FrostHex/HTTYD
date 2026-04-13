@tool
extends Node3D

## usage in editor:
## - recursively traverse all child nodes of the current node (excluding itself)
## - delete all child nodes whose names contain "@" (including their full subtrees)

@export var auto_run: bool = true

func _ready() -> void:
	if Engine.is_editor_hint() and auto_run:
		_remove_nodes_with_at()

## entry point: recursively delete all child nodes whose names contain "@"
func _remove_nodes_with_at() -> void:
	var targets: Array = []
	_collect_with_at(self, targets)

	if targets.is_empty():
		print("[temp.gd] 未发现名称包含 @ 的子节点。")
		return

	# delete from deeper levels first to avoid path issues caused by hierarchy changes.
	targets.sort_custom(func(a, b): return a.get_path().get_name_count() > b.get_path().get_name_count())

	for n in targets:
		if is_instance_valid(n):
			n.queue_free()

	print("[temp.gd] 已删除 %d 个名称包含 @ 的节点。" % targets.size())

## depth-first collection: nodes whose names contain "@" (excluding the root itself)
func _collect_with_at(root: Node, acc: Array) -> void:
	for c in root.get_children():
		if c.name.find("@") != -1:
			acc.append(c)
			# no need to traverse its subtree after a match (the subtree will be removed with the parent).
		else:
			_collect_with_at(c, acc)
