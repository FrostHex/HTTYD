import os
import io
import contextlib

try:
    import pyperclip
except ImportError:
    pyperclip = None

# define the tuple of names to exclude (applies to both folders and files, partial match).
EXCLUDE_NAMES = (
                    'Addons', 'build', '.import', '.uid', '.git', '.editorconfig',
                    'Print_Hierarchy'
                )

# define the tuple of folder-only exclusions (partial match, folders only).
EXCLUDE_FOLDERS = (
                    'Ocean', 'Cloud', 'Fog', 'Rocks', 'Mountain', 'Metal', 'Tree',
                    'Demo'
                  )

# when True, excluded folders are shown by name but not recursed into; when False, excluded folders are hidden entirely.
SHOW_EXCLUDED_FOLDER_NAMES = True 
ADD_SLASH_TO_FOLDERS = True  # whether to append a slash to folder names to distinguish folders from files.

def is_excluded(name):
    """check whether the name contains any excluded string (for files and folders)."""
    return any(exclude in name for exclude in EXCLUDE_NAMES)

def is_folder_excluded(name):
    """check whether a folder name contains any folder-only excluded string."""
    return any(exclude in name for exclude in EXCLUDE_FOLDERS)


def tree(dir_path='./../../', prefix='', ignore_dot_folders=False):
    # get all entries in the directory and sort them alphabetically.
    entries = sorted(os.listdir(dir_path))
    if ignore_dot_folders:
        filtered_entries = []
        for name in entries:
            path = os.path.join(dir_path, name)
            is_dir = os.path.isdir(path)
            
            # exclude folders starting with a dot.
            if is_dir and name.startswith('.'):
                continue
            
            # process entries in the shared exclusion list (partial match).
            if is_excluded(name):
                # always hide files.
                if not is_dir:
                    continue
                # for folders, decide visibility using SHOW_EXCLUDED_FOLDER_NAMES.
                if not SHOW_EXCLUDED_FOLDER_NAMES:
                    continue
            
            # process entries in the folder-only exclusion list (partial match).
            if is_dir and is_folder_excluded(name):
                # decide visibility using SHOW_EXCLUDED_FOLDER_NAMES.
                if not SHOW_EXCLUDED_FOLDER_NAMES:
                    continue
            
            filtered_entries.append(name)
        entries = filtered_entries
    count = len(entries)
    for index, name in enumerate(entries):
        path = os.path.join(dir_path, name)
        is_dir = os.path.isdir(path)
        display_name = name + '/' if (is_dir and ADD_SLASH_TO_FOLDERS) else name
        connector = '└── ' if index == count - 1 else '├── '
        print(prefix + connector + display_name)
        if is_dir:
            extension = '    ' if index == count - 1 else '│   '
            # if this is an excluded folder and names are shown, do not recurse into it.
            if SHOW_EXCLUDED_FOLDER_NAMES and (is_excluded(name) or is_folder_excluded(name)):
                continue
            tree(path, prefix + extension, ignore_dot_folders)

def output(text):
    """write the generated text into the Project Hierarchy code block in Documentation.md."""
    devnote_path = os.path.join(os.path.dirname(__file__), './../../Media/Docs/Documentation.md')
    devnote_path = os.path.normpath(devnote_path)
    
    try:
        with open(devnote_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"未找到文件: {devnote_path}")
        return
    
    # find the "Project Hierarchy" line.
    hierarchy_index = -1
    for i, line in enumerate(lines):
        if 'Project Hierarchy' in line:
            hierarchy_index = i
            break
    
    if hierarchy_index == -1:
        print("未找到 'Project Hierarchy'")
        return
    
    # find the next code block start marker ```.
    start_index = -1
    indent = ''
    for i in range(hierarchy_index + 1, len(lines)):
        line = lines[i]
        stripped = line.lstrip(' \t')
        if stripped.startswith('```'):
            start_index = i
            # capture indentation (spaces and tabs).
            indent = line[:len(line) - len(stripped)]
            break
    
    if start_index == -1:
        print("未找到代码块开始标记 ```")
        return
    
    # find the matching code block end marker ```.
    end_index = -1
    for i in range(start_index + 1, len(lines)):
        line = lines[i]
        stripped = line.lstrip(' \t')
        if stripped.startswith('```'):
            end_index = i
            break
    
    if end_index == -1:
        print("未找到代码块结束标记 ```")
        return
    
    # build new content and apply the same indentation per line.
    new_content_lines = []
    for line in text.split('\n'):
        if line:  # add indentation for non-empty lines.
            new_content_lines.append(indent + line + '\n')
        else:  # keep empty lines as empty (or indentation-only).
            new_content_lines.append('\n')
    
    # remove the trailing extra empty line (if any).
    if new_content_lines and new_content_lines[-1] == '\n':
        new_content_lines.pop()
    
    # rebuild final file content.
    new_lines = lines[:start_index + 1] + new_content_lines + lines[end_index:]
    
    with open(devnote_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
    
    print(f"已更新 {devnote_path} 中的 Project Hierarchy")

if __name__ == '__main__':
    # capture printed output.
    buffer = io.StringIO()
    with contextlib.redirect_stdout(buffer):
        tree('.', ignore_dot_folders=True)
    result = buffer.getvalue()
    print(result)
    
    if pyperclip:
        try:
            pyperclip.copy(result)
            print("结果已复制到剪贴板")
        except Exception as e:
            print("复制到剪贴板时出错:", e)
    else:
        print("未安装 pyperclip 模块, 无法自动复制到剪贴板: pip install pyperclip")

    output(result)