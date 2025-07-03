@tool
extends Node3D

func _ready():
    if Engine.is_editor_hint():
        print("自动清除 LOD 节点")
        _delete_lod_nodes_recursive(self)

func _delete_lod_nodes_recursive(node: Node):
    for child in node.get_children():
        _delete_lod_nodes_recursive(child)

    if node.name.contains("LOD01") or node.name.contains("LOD02") or node.name.contains("LOD03") or node.name.contains("LOD04"):
        print("删除节点: ", node.name)
        node.queue_free()
