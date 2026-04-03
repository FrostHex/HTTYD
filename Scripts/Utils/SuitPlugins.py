from pathlib import Path

# Run it after updating the plugins (copying and overwriting the plugin folders in Addons) to automatically replace the paths

REPLACE_RULES: tuple[tuple[str, str], ...] = (
	("res://addons/sky_3d", "res://Addons/sky_3d"),
	("res://addons/godot-xr-tools", "res://Addons/godot-xr-tools"),
    ("var safe := min(_head_shape_cast.get_closest_collision_safe_fraction()", "var safe : float = min(float(_head_shape_cast.get_closest_collision_safe_fraction())"),
)


def replace_in_addons() -> None:
	script_dir = Path(__file__).resolve().parent
	addons_dir = (script_dir / "../../Addons").resolve()
	# project_file = (script_dir / "../../project.godot").resolve() 
	excluded_dir = (addons_dir / "godot-cpp").resolve()

	if not addons_dir.exists() or not addons_dir.is_dir():
		print(f"Addons directory not found: {addons_dir}")
		return

	replaced_files = 0
	total_replacements = 0

	def process_one_file(file_path: Path) -> tuple[bool, int]:
		try:
			content = file_path.read_text(encoding="utf-8")
		except (UnicodeDecodeError, OSError):
			# Skipping binary or unreadable file
			return False, 0

		original_content = content
		file_replacements = 0

		for old_text, new_text in REPLACE_RULES:
			count = content.count(old_text)
			if count > 0:
				content = content.replace(old_text, new_text)
				file_replacements += count

		if content == original_content:
			return False, 0

		try:
			file_path.write_text(content, encoding="utf-8")
		except OSError:
			return False, 0

		return True, file_replacements

	for file_path in addons_dir.rglob("*"):
		if not file_path.is_file():
			continue
		if excluded_dir in file_path.resolve().parents:
			continue

		is_updated, file_replacements = process_one_file(file_path)
		if is_updated:
			replaced_files += 1
			total_replacements += file_replacements
			print(f"Updated: {file_path} (replacements: {file_replacements})")

	# if project_file.exists() and project_file.is_file():
	# 	is_updated, file_replacements = process_one_file(project_file)
	# 	if is_updated:
	# 		replaced_files += 1
	# 		total_replacements += file_replacements
	# 		print(f"Updated: {project_file} (replacements: {file_replacements})")
	# else:
	# 	print(f"project.godot not found: {project_file}")

	print(
		f"Done. Updated files: {replaced_files}, total replacements: {total_replacements}"
	)


if __name__ == "__main__":
	replace_in_addons()
