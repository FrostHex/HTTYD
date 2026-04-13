@tool
extends Node3D

@export var target_node: Node3D : set = set_target_node

func set_target_node(value: Node3D):
    target_node = value
    if Engine.is_editor_hint() and target_node:
        copy_transform_to_target()

func copy_transform_to_target():
    if not target_node:
        print("目标节点为空，无法复制transform")
        return
    
    # copy position, rotation, and scale to the target node.
    target_node.position = position
    target_node.rotation = rotation
    target_node.scale = scale
    
    print("已将transform复制到节点: ", target_node.name)
    print("Position: ", position)
    print("Rotation: ", rotation)
    print("Scale: ", scale)
