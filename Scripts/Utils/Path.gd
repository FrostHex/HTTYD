@tool
extends Path3D

# 在编辑器中按指定间距沿路径放置标记（使用自定义 Node3D 模板）。
# 用法：将此脚本挂到 Path3D 上，在检查器中设置 template_path，调整参数或点击“立即重建”。

@export var spacing: float = 2.0: set = set_spacing
@export var marker_color: Color = Color(0.2, 0.8, 1.0, 0.4): set = set_marker_color
@export var emission_energy: float = 1.5: set = set_emission_energy

# 自定义模板（可放在场景任意位置），支持任意 Node3D（含 CSG、Mesh 等）
@export var template_path: NodePath = NodePath(""): set = set_template_path
@export var align_to_path: bool = true

# 修改参数时是否自动重建
@export var auto_update: bool = true 

# 勾选后立即重建，操作完成后会自动复位为 false
@export var regenerate_now: bool = false: set = _set_regenerate_now

# 生成物容器节点名称
@export var container_name: StringName = StringName("__PathMarkers")

# 容器节点放置位置：false=Path3D子节点，true=Path3D同级（推荐，便于后续移除脚本）
@export var place_as_sibling: bool = true

# 是否在游戏运行时也显示（默认显示，便于在运行中可见）
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
		
		# 根据 place_as_sibling 决定放置位置
		var parent_node: Node = get_parent() if place_as_sibling and get_parent() != null else self
		parent_node.add_child(container)
		
		# 设置 owner 以便保存到场景
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
	# 关闭 CSG 碰撞
	if n is CSGPolygon3D:
		(n as CSGPolygon3D).use_collision = false
	# 关闭物理碰撞（若是碰撞对象）
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
	# 半透明
	mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat.albedo_color = marker_color
	# 非光照着色，进一步减轻开销
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	# 发光（需要环境启用 Bloom 才能看到泛光）
	mat.emission_enabled = true
	mat.emission = Color(marker_color.r, marker_color.g, marker_color.b, 1.0)
	mat.emission_energy_multiplier = emission_energy
	# 可根据需要关闭阴影等（非必须）
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
	# 输入向量长度检查
	if x_axis.length() < 1e-6:
		return Basis.IDENTITY
	
	var x: Vector3 = x_axis.normalized()
	
	# 选择一个与 x 不平行的上方向
	var up: Vector3 = up_hint
	if abs(x.dot(up)) > 0.95:
		up = Vector3.FORWARD if abs(x.dot(Vector3.FORWARD)) < 0.95 else Vector3.RIGHT
	
	# 使用 up 派生 y，再派生 z，保证正交、右手系
	var y: Vector3 = up.cross(x)
	if y.length() < 1e-6:
		# 再次兜底挑一个不同的向上
		up = Vector3.RIGHT if abs(x.dot(Vector3.RIGHT)) < 0.95 else Vector3.FORWARD
		y = up.cross(x)
		if y.length() < 1e-6:
			# 最终兜底：返回单位矩阵
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
			# 计算该点的切线方向（前向采样，若末端则后向）
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
				# 保持原始缩放；设置位置与朝向
				var original_scale: Vector3 = n3d.scale
				n3d.transform.origin = p
				if align_to_path and align_ok:
					# 使局部 +X 轴沿路径切线
					n3d.basis = _basis_from_x(dir.normalized(), Vector3.UP)
					n3d.scale = original_scale
				# 保持原始大小，不修改 scale
				container.add_child(n3d)
				if Engine.is_editor_hint():
					var scene_owner: Node = owner if owner != null else get_tree().edited_scene_root
					_set_owner_recursive(n3d, scene_owner)
				_disable_collision_recursive(n3d)
				_apply_marker_material_recursive(n3d, mat)
				n3d.visible = Engine.is_editor_hint() or visible_in_game
		dist += spacing

	_update_runtime_visibility()
