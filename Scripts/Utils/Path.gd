@tool
extends Path3D

# place markers along the path at a specified spacing in the editor (using a custom Node3D template).
# usage: attach this script to Path3D, set template_path in the inspector, adjust parameters, or click "regenerate now".

@export var spacing: float = 2.0: set = set_spacing
@export var marker_color: Color = Color(0.2, 0.8, 1.0, 0.4): set = set_marker_color
@export var emission_energy: float = 1.5: set = set_emission_energy

# custom template (can be placed anywhere in the scene), supports any Node3D (including CSG and Mesh).
@export var template_path: NodePath = NodePath(""): set = set_template_path
@export var align_to_path: bool = true

# whether to rebuild automatically when parameters change.
@export var auto_update: bool = true 

# rebuild immediately when checked, then auto-reset to false after completion.
@export var regenerate_now: bool = false: set = _set_regenerate_now

# generated container node name.
@export var container_name: StringName = StringName("__PathMarkers")

# container placement: false = child of Path3D, true = sibling of Path3D (recommended for easy cleanup).
@export var place_as_sibling: bool = true

# whether to show in runtime as well (enabled by default so it is visible during play).
@export var visible_in_game: bool = true: set = set_visible_in_game

func _ready() -> void:
	if Engine.is_editor_hint():
		if auto_update:
			_rebuild_markers()
	_update_runtime_visibility()

func set_spacing(v: float) -> void:
	spacing = max(0.001, v)
	if Engine.is_editor_hint() and auto_update:
		_rebuild_markers()

func set_marker_color(v: Color) -> void:
	marker_color = v
	if Engine.is_editor_hint() and auto_update:
		_rebuild_markers()

func set_emission_energy(v: float) -> void:
	emission_energy = max(0.0, v)
	if Engine.is_editor_hint() and auto_update:
		_rebuild_markers()

func set_template_path(v: NodePath) -> void:
	template_path = v
	if Engine.is_editor_hint() and auto_update:
		_rebuild_markers()

func set_visible_in_game(v: bool) -> void:
	visible_in_game = v
	_update_runtime_visibility()

func _set_regenerate_now(v: bool) -> void:
	regenerate_now = false
	if not Engine.is_editor_hint():
		return
	if v:
		_rebuild_markers()

func _update_runtime_visibility() -> void:
	var container: Node3D = _get_container()
	if container:
		var vis: bool = Engine.is_editor_hint() or visible_in_game
		container.visible = vis
		for child in container.get_children():
			if child is VisualInstance3D:
				(child as VisualInstance3D).visible = vis

func _get_container() -> Node3D:
	var parent_node: Node = get_parent() if place_as_sibling and get_parent() != null else self
	var node: Node = parent_node.get_node_or_null(NodePath(container_name))
	if node and node is Node3D:
		return node
	return null

func _ensure_container() -> Node3D:
	var container: Node3D = _get_container()
	if container == null:
		container = Node3D.new()
		container.name = container_name
		
		# choose placement based on place_as_sibling.
		var parent_node: Node = get_parent() if place_as_sibling and get_parent() != null else self
		parent_node.add_child(container)
		
		# set owner so it can be saved with the scene.
		if Engine.is_editor_hint():
			var scene_owner: Node = owner if owner != null else get_tree().edited_scene_root
			container.owner = scene_owner
	return container

func _clear_children(node: Node) -> void:
	for c in node.get_children():
		c.queue_free()

func _set_owner_recursive(n: Node, o: Node) -> void:
	n.owner = o
	for ch in n.get_children():
		_set_owner_recursive(ch, o)

func _disable_collision_recursive(n: Node) -> void:
	# disable CSG collision.
	if n is CSGPolygon3D:
		(n as CSGPolygon3D).use_collision = false
	# disable physics collision (for collision objects).
	if n is CollisionObject3D:
		var co: CollisionObject3D = n as CollisionObject3D
		co.collision_layer = 0
		co.collision_mask = 0
		if co is Area3D:
			(co as Area3D).monitorable = false
	for ch in n.get_children():
		_disable_collision_recursive(ch)


func _make_material() -> StandardMaterial3D:
	var mat: StandardMaterial3D = StandardMaterial3D.new()
	# semi-transparent.
	mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat.albedo_color = marker_color
	# unshaded rendering to further reduce cost.
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	# emission (Bloom must be enabled in the environment to see glow).
	mat.emission_enabled = true
	mat.emission = Color(marker_color.r, marker_color.g, marker_color.b, 1.0)
	mat.emission_energy_multiplier = emission_energy
	# optionally disable shadows and similar effects (not required).
	mat.disable_receive_shadows = true
	return mat

func _apply_marker_material_recursive(n: Node, mat: Material) -> void:
	if n is CSGShape3D:
		(n as CSGShape3D).material = mat
	elif n is MeshInstance3D:
		(n as MeshInstance3D).material_override = mat
	for ch in n.get_children():
		_apply_marker_material_recursive(ch, mat)

func _basis_from_x(x_axis: Vector3, up_hint: Vector3 = Vector3.UP) -> Basis:
	# validate input vector length.
	if x_axis.length() < 1e-6:
		return Basis.IDENTITY
	
	var x: Vector3 = x_axis.normalized()
	
	# choose an up direction that is not parallel to x.
	var up: Vector3 = up_hint
	if abs(x.dot(up)) > 0.95:
		up = Vector3.FORWARD if abs(x.dot(Vector3.FORWARD)) < 0.95 else Vector3.RIGHT
	
	# derive y from up, then derive z to keep an orthonormal right-handed basis.
	var y: Vector3 = up.cross(x)
	if y.length() < 1e-6:
		# fallback again with a different up direction.
		up = Vector3.RIGHT if abs(x.dot(Vector3.RIGHT)) < 0.95 else Vector3.FORWARD
		y = up.cross(x)
		if y.length() < 1e-6:
			# final fallback: return identity basis.
			return Basis.IDENTITY
	
	y = y.normalized()
	var z: Vector3 = x.cross(y)
	if z.length() < 1e-6:
		return Basis.IDENTITY
	z = z.normalized()
	
	return Basis(x, y, z)

func _rebuild_markers() -> void:
	if not Engine.is_editor_hint():
		return
	var c: Curve3D = curve
	var container: Node3D = _ensure_container()
	_clear_children(container)

	if c == null:
		return

	var length: float = c.get_baked_length()
	if length <= 0.0 or spacing <= 0.0:
		return

	var mat: StandardMaterial3D = _make_material()

	var dist: float = 0.0
	while dist <= length + 0.0001:
		var p: Vector3 = c.sample_baked(dist)
		var dir: Vector3 = Vector3.ZERO
		var align_ok: bool = false
		if align_to_path:
			# compute tangent direction at this point (forward sample, fallback to backward at the end).
			var delta: float = max(0.001, min(0.1, spacing * 0.5))
			var d2: float = min(dist + delta, length)
			var p2: Vector3 = c.sample_baked(d2)
			dir = (p2 - p)
			if dir.length() < 1e-5 and dist > 0.0:
				var d0: float = max(dist - delta, 0.0)
				var p0: Vector3 = c.sample_baked(d0)
				dir = (p - p0)
			if dir.length() >= 1e-5:
				align_ok = true
		var template_node: Node3D = get_node_or_null(template_path) as Node3D
		if template_node and template_node is Node3D:
			var inst: Node = template_node.duplicate()
			if inst and inst is Node3D:
				var n3d: Node3D = inst as Node3D
				# keep original scale; set position and orientation.
				var original_scale: Vector3 = n3d.scale
				n3d.transform.origin = p
				if align_to_path and align_ok:
					# align local +X axis to the path tangent.
					n3d.basis = _basis_from_x(dir.normalized(), Vector3.UP)
					n3d.scale = original_scale
				# keep original size without modifying scale.
				container.add_child(n3d)
				if Engine.is_editor_hint():
					var scene_owner: Node = owner if owner != null else get_tree().edited_scene_root
					_set_owner_recursive(n3d, scene_owner)
				_disable_collision_recursive(n3d)
				_apply_marker_material_recursive(n3d, mat)
				n3d.visible = Engine.is_editor_hint() or visible_in_game
		dist += spacing

	_update_runtime_visibility()
