@tool
extends Node3D

func _ready():
    if Engine.is_editor_hint():
        replace_rocks()

func replace_rocks():
    # 加载岩石场景资源
    var rock_aa = preload("res://Scenes/Rocks/Basic/Rock_AA.tscn")
    var rock_ba = preload("res://Scenes/Rocks/Basic/Rock_BA.tscn")
    var rock_ca = preload("res://Scenes/Rocks/Basic/Rock_CA.tscn")
    
    # 收集需要替换的节点信息
    var replacements = []
    var counters = {"AA": 0, "BA": 0, "CA": 0}
    
    # 递归收集所有需要替换的节点
    collect_nodes_to_replace(self, replacements, counters, rock_aa, rock_ba, rock_ca)
    
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

func collect_nodes_to_replace(node: Node, replacements: Array, counters: Dictionary, rock_aa, rock_ba, rock_ca):
    for child in node.get_children():
        var node_name = child.name
        var target_scene = null
        var rock_type = ""
        
        # 根据名称确定要替换的岩石类型
        if "A_03" in node_name:
            target_scene = rock_ca
            rock_type = "CA"
        elif "A_02" in node_name:
            target_scene = rock_ba
            rock_type = "BA"
        elif "A_01" in node_name:
            target_scene = rock_aa
            rock_type = "AA"
        
        if target_scene:
            counters[rock_type] += 1
            var new_name = "Rock_" + rock_type + "_" + str(counters[rock_type]).pad_zeros(2)
            
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
            collect_nodes_to_replace(child, replacements, counters, rock_aa, rock_ba, rock_ca)


