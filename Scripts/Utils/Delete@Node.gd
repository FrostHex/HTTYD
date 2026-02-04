@tool
extends Node3D

## 在编辑器中使用：
## - 递归遍历当前节点的所有子节点（不包含自身）
## - 删除所有名称中包含 "@" 的子节点（及其整个子树）

@export var auto_run: bool = true

func _ready() -> void:
	if Engine.is_editor_hint() and auto_run:
		_remove_nodes_with_at()

## 入口：递归删除所有名称包含 "@" 的子节点
func _remove_nodes_with_at() -> void:
	var targets: Array = []
	_collect_with_at(self, targets)

	if targets.is_empty():
		print("[temp.gd] 未发现名称包含 @ 的子节点。")
		return

	# 为避免层级变化影响路径，从更深层开始删除
	targets.sort_custom(func(a, b): return a.get_path().get_name_count() > b.get_path().get_name_count())

	for n in targets:
		if is_instance_valid(n):
			n.queue_free()

	print("[temp.gd] 已删除 %d 个名称包含 @ 的节点。" % targets.size())

## 深度优先收集：命名包含 "@" 的节点（不包含根本身）
func _collect_with_at(root: Node, acc: Array) -> void:
	for c in root.get_children():
		if c.name.find("@") != -1:
			acc.append(c)
			# 命中后无需深入其子树（整棵子树会随父一起删除）
		else:
			_collect_with_at(c, acc)
