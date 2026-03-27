@tool
extends Node3D

## 在编辑器中使用：
## - 挂载后递归遍历当前节点和它的所有子节点
## - 打印各节点的名称和层级关系


func _ready() -> void:
	if not Engine.is_editor_hint():
		return
	var lines: PackedStringArray = []
	lines.append("%s (%s)" % [name, get_class()])
	_collect_children(self, "", lines)
	var output := "\n".join(lines)
	print(output)
	DisplayServer.clipboard_set(output)


func _collect_children(node: Node, prefix: String, lines: PackedStringArray) -> void:
	var children := node.get_children()
	for i in children.size():
		var child := children[i]
		var is_last := i == children.size() - 1
		lines.append(prefix + ("└── " if is_last else "├── ") + "%s (%s)" % [child.name, child.get_class()])
		_collect_children(child, prefix + ("    " if is_last else "│   "), lines)
