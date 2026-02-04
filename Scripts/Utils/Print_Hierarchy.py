import os
import io
import contextlib

try:
    import pyperclip
except ImportError:
    pyperclip = None

# 定义要排除的名称元组（适用于文件夹和文件，部分匹配）
EXCLUDE_NAMES = (
                    'Addons', 'build', '.import', '.uid', '.git', '.editorconfig',
                    'Print_Hierarchy', '.o'
                )

# 定义只排除文件夹的名称元组（部分匹配，仅对文件夹生效）
EXCLUDE_FOLDERS = (
                    'Ocean', 'Cloud', 'Fog', 'Rocks', 'Mountain', 'Metal', 'Tree'
                  )

# 当为True时，排除列表中的文件夹仍会显示名称但不递归；当为False时，文件夹完全隐藏
SHOW_EXCLUDED_FOLDER_NAMES = True 

def is_excluded(name):
    """检查名称是否包含排除列表中的任意字符串（适用于文件和文件夹）"""
    return any(exclude in name for exclude in EXCLUDE_NAMES)

def is_folder_excluded(name):
    """检查文件夹名称是否包含排除列表中的任意字符串（仅适用于文件夹）"""
    return any(exclude in name for exclude in EXCLUDE_FOLDERS)


def tree(dir_path='./../../', prefix='', ignore_dot_folders=False):
    # 获取目录下所有条目并按字母排序
    entries = sorted(os.listdir(dir_path))
    if ignore_dot_folders:
        filtered_entries = []
        for name in entries:
            path = os.path.join(dir_path, name)
            is_dir = os.path.isdir(path)
            
            # 排除以.开头的文件夹
            if is_dir and name.startswith('.'):
                continue
            
            # 对于排除列表中的条目（部分匹配）
            if is_excluded(name):
                # 文件一律隐藏
                if not is_dir:
                    continue
                # 文件夹：根据 SHOW_EXCLUDED_FOLDER_NAMES 决定是否显示
                if not SHOW_EXCLUDED_FOLDER_NAMES:
                    continue
            
            # 对于仅文件夹排除列表中的条目（部分匹配）
            if is_dir and is_folder_excluded(name):
                # 根据 SHOW_EXCLUDED_FOLDER_NAMES 决定是否显示
                if not SHOW_EXCLUDED_FOLDER_NAMES:
                    continue
            
            filtered_entries.append(name)
        entries = filtered_entries
    count = len(entries)
    for index, name in enumerate(entries):
        path = os.path.join(dir_path, name)
        connector = '└── ' if index == count - 1 else '├── '
        print(prefix + connector + name)
        if os.path.isdir(path):
            extension = '    ' if index == count - 1 else '│   '
            # 如果是排除文件夹且设置了显示名称，则不递归进入
            if SHOW_EXCLUDED_FOLDER_NAMES and (is_excluded(name) or is_folder_excluded(name)):
                continue
            tree(path, prefix + extension, ignore_dot_folders)

def output(text):
    """将生成的文本输出到 DevNote.md 的 Project Hierarchy 代码块中"""
    devnote_path = os.path.join(os.path.dirname(__file__), './../../DevNote.md')
    devnote_path = os.path.normpath(devnote_path)
    
    try:
        with open(devnote_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"未找到文件: {devnote_path}")
        return
    
    # 查找 "Project Hierarchy" 行
    hierarchy_index = -1
    for i, line in enumerate(lines):
        if 'Project Hierarchy' in line:
            hierarchy_index = i
            break
    
    if hierarchy_index == -1:
        print("未找到 'Project Hierarchy'")
        return
    
    # 查找下一行的 ``` 开始标记
    start_index = -1
    indent = ''
    for i in range(hierarchy_index + 1, len(lines)):
        line = lines[i]
        stripped = line.lstrip(' \t')
        if stripped.startswith('```'):
            start_index = i
            # 计算缩进（空格和tab）
            indent = line[:len(line) - len(stripped)]
            break
    
    if start_index == -1:
        print("未找到代码块开始标记 ```")
        return
    
    # 查找对应的结束 ```
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
    
    # 构建新的内容，每行添加相同的缩进
    new_content_lines = []
    for line in text.split('\n'):
        if line:  # 非空行添加缩进
            new_content_lines.append(indent + line + '\n')
        else:  # 空行保持为空（或只有缩进）
            new_content_lines.append('\n')
    
    # 移除最后一个多余的空行（如果有）
    if new_content_lines and new_content_lines[-1] == '\n':
        new_content_lines.pop()
    
    # 重建文件内容
    new_lines = lines[:start_index + 1] + new_content_lines + lines[end_index:]
    
    with open(devnote_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
    
    print(f"已更新 {devnote_path} 中的 Project Hierarchy")

if __name__ == '__main__':
    # 捕获打印输出
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