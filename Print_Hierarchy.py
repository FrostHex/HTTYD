import os
import io
import contextlib

try:
    import pyperclip
except ImportError:
    pyperclip = None

def tree(dir_path='.',
         prefix='',
         ignore_dot_folders=False,
         stop_folders=None):
    """
    stop_folders: 不再递归的文件夹名列表
    """
    if stop_folders is None:
        stop_folders = []

    entries = sorted(os.listdir(dir_path))
    if ignore_dot_folders:
        entries = [
            n for n in entries
            if not (n.startswith('.') and os.path.isdir(os.path.join(dir_path, n)))
        ]

    count = len(entries)
    for idx, name in enumerate(entries):
        path = os.path.join(dir_path, name)
        is_dir = os.path.isdir(path)
        connector = '└── ' if idx == count - 1 else '├── '
        display = name + '/' if is_dir else name
        print(prefix + connector + display)

        if is_dir and name not in stop_folders:
            extension = '    ' if idx == count - 1 else '│   '
            tree(path, prefix + extension, ignore_dot_folders, stop_folders)


if __name__ == '__main__':
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        tree('.', ignore_dot_folders=True, stop_folders=["godot-cpp", "Ocean"])
    result = buf.getvalue()
    print(result)

    if pyperclip:
        try:
            pyperclip.copy(result)
            print("结果已复制到剪贴板")
        except Exception as e:
            print("复制到剪贴板时出错:", e)
    else:
        print("未安装 pyperclip，无法复制: pip install pyperclip")
