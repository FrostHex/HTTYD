@tool
extends Node3D

## usage in editor:
## - after attaching, recursively traverse the current node and all of its child nodes
## - print each node name with hierarchy structure


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
