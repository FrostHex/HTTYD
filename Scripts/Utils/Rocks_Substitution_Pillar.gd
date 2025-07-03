@tool
extends Node3D

func _ready():
    if Engine.is_editor_hint():
        replace_rocks()

func replace_rocks():
    # 加载岩石场景资源
    var rock_b = preload("res://Scenes/Rocks/Pillar/Rock_Pillar_B.tscn")
    var rock_c = preload("res://Scenes/Rocks/Pillar/Rock_Pillar_C.tscn")
    var rock_d = preload("res://Scenes/Rocks/Pillar/Rock_Pillar_D.tscn")
    var rock_e = preload("res://Scenes/Rocks/Pillar/Rock_Pillar_E.tscn")
    
    # 收集需要替换的节点信息
    var replacements = []
    var counters = {"B": 0, "C": 0, "D": 0, "E": 0} # start from 1
    
    # 递归收集所有需要替换的节点
    collect_nodes_to_replace(self, replacements, counters, rock_b, rock_c, rock_d, rock_e)
    
    # 执行替换操作
    for replacement in replacements:
        var old_node = replacement["old_node"]
        var parent_node = replacement["parent"]
        
        # 实例化新的岩石节点
        var new_rock = replacement["scene"].instantiate()
        
        # 设置新节点的属性
        new_rock.name = replacement["name"]
        new_rock.position = replacement["position"]
        new_rock.rotation = replacement["rotation"]
        new_rock.scale = replacement["scale"]
        
        # 先添加新节点到正确的父节点
        parent_node.add_child(new_rock)
        new_rock.owner = get_tree().edited_scene_root
        
        # 再移除旧节点
        parent_node.remove_child(old_node)
        old_node.queue_free()
        
        print("已替换节点: ", replacement["name"])

func collect_nodes_to_replace(node: Node, replacements: Array, counters: Dictionary, rock_b, rock_c, rock_d, rock_e):
    for child in node.get_children():
        var node_name = child.name
        var target_scene = null
        var rock_type = ""
        
        # 根据名称确定要替换的岩石类型
        if "Rock_1" in node_name:
            target_scene = rock_b
            rock_type = "B"
        elif "Rock_2" in node_name:
            target_scene = rock_c
            rock_type = "C"
        elif "Rock_3" in node_name:
            target_scene = rock_d
            rock_type = "D"
        elif "Rock_4" in node_name:
            target_scene = rock_e
            rock_type = "E"
        
        if target_scene:
            counters[rock_type] += 1
            var new_name = "Rock_Pillar_" + rock_type + "_" + str(counters[rock_type]).pad_zeros(2)
            
            replacements.append({
                "old_node": child,
                "parent": node,
                "scene": target_scene,
                "name": new_name,
                "position": child.position,
                "rotation": child.rotation,
                "scale": child.scale
            })
        else:
            # 递归遍历子节点
            collect_nodes_to_replace(child, replacements, counters, rock_b, rock_c, rock_d, rock_e)
