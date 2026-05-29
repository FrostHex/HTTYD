@tool
extends Node3D

func _ready():
    if Engine.is_editor_hint():
        replace_rocks()

func replace_rocks():
    # load rock scene resources.
    var rock_b = preload("res://Tscn/Rocks/Pillar/Rock_Pillar_B.tscn")
    var rock_c = preload("res://Tscn/Rocks/Pillar/Rock_Pillar_C.tscn")
    var rock_d = preload("res://Tscn/Rocks/Pillar/Rock_Pillar_D.tscn")
    var rock_e = preload("res://Tscn/Rocks/Pillar/Rock_Pillar_E.tscn")
    
    # collect nodes that need replacement.
    var replacements = []
    var counters = {"B": 0, "C": 0, "D": 0, "E": 0} # start from 1
    
    # recursively collect all nodes that need replacement.
    collect_nodes_to_replace(self, replacements, counters, rock_b, rock_c, rock_d, rock_e)
    
    # perform replacement.
    for replacement in replacements:
        var old_node = replacement["old_node"]
        var parent_node = replacement["parent"]
        
        # instantiate a new rock node.
        var new_rock = replacement["scene"].instantiate()
        
        # set properties for the new node.
        new_rock.name = replacement["name"]
        new_rock.position = replacement["position"]
        new_rock.rotation = replacement["rotation"]
        new_rock.scale = replacement["scale"]
        
        # add the new node to the correct parent first.
        parent_node.add_child(new_rock)
        new_rock.owner = get_tree().edited_scene_root
        
        # remove the old node afterward.
        parent_node.remove_child(old_node)
        old_node.queue_free()
        
        print("已替换节点: ", replacement["name"])

func collect_nodes_to_replace(node: Node, replacements: Array, counters: Dictionary, rock_b, rock_c, rock_d, rock_e):
    for child in node.get_children():
        var node_name = child.name
        var target_scene = null
        var rock_type = ""
        
        # determine replacement rock type from the node name.
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
            # recursively traverse child nodes.
            collect_nodes_to_replace(child, replacements, counters, rock_b, rock_c, rock_d, rock_e)
