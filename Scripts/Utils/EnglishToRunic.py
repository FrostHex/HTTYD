import json
import os
import re

# ── Runic transliteration map ──────────────────────────────────────────────
RUNIC_MAP: dict[str, str] = \
{
    "a": "ᛅ",
    "b": "ᛒ",
    "d": "ᛏ",
    "e": "ᛁ",
    "f": "ᚠ",
    "g": "ᚴ",
    "h": "ᚼ",
    "i": "ᛁ",
    "j": "ᛁ",
    "k": "ᚴ",
    "l": "ᛚ",
    "m": "ᛙ",
    "n": "ᚾ",
    "o": "ᚢ",
    "p": "ᛒ",
    "q": "ᚹ",
    "r": "ᚱ",
    "s": "ᛋ",
    "t": "ᛏ",
    "u": "ᚢ",
    "v": "ᚢ",
    "w": "ᚢ",
    "x": "ᚴᛋ",
    "y": "ᚢ",
    "z": "ᛋ",

    ")": "",
    "(": "",
    ",": "",
    ".": "",
    ":": "",
    "'": "",
    " ": "  "
}

_RE_PATTERNS: list[tuple[re.Pattern, str]] = \
[
    (re.compile(r"ck",  re.IGNORECASE), "ᚴ"),
    (re.compile(r"ch",  re.IGNORECASE), "ᚴ"),
    (re.compile(r"c(?=[eiyEIY])", re.IGNORECASE), "ᛋ"),
    (re.compile(r"c",   re.IGNORECASE), "ᚴ"),
    (re.compile(r"test!", re.IGNORECASE), "test"),
    (re.compile(r"!\)", re.IGNORECASE), ""),
]
# ──────────────────────────────────────────────────────────────────────────


def to_runic(text: str) -> str:
    """Replace every mapped Latin character in *text* with its runic glyph."""
    result: list[str] = []
    i = 0
    while i < len(text):
        matched = False
        ch = text[i]
        for pattern, replacement in _RE_PATTERNS:
            m = pattern.match(text, i)
            if m:
                result.append(replacement)
                i += m.end() - m.start()
                matched = True
                break
        if not matched:
            runic = RUNIC_MAP.get(ch.lower())
            result.append(runic if runic is not None else ch)
            i += 1
    return "".join(result)


def translate_value(value):
    """Recursively translate a JSON value (str, list, or dict)."""
    if isinstance(value, str):
        return to_runic(value)
    if isinstance(value, list):
        return [translate_value(item) for item in value]
    if isinstance(value, dict):
        return {k: translate_value(v) for k, v in value.items()}
    return value


# ── Source-faithful formatter ──────────────────────────────────────────────

def _jstr(s: str) -> str:
    """Encode a Python string to a JSON string literal."""
    return json.dumps(s, ensure_ascii=False)


def dump_source_style(src_path: str, out_data: dict) -> str:
    """Serialize out_data while exactly reproducing the formatting of src_path.

    Walks the source file line-by-line and:
    - Blank / whitespace-only lines: copied verbatim (preserves exact spacing).
    - Structural lines ({, }, [, ], ],): copied verbatim.
    - Array-valued key lines (`    "key":` with no inline value): key is
      re-emitted as `    "key": ` (adding a trailing space per the target
      convention), followed by the translated array items.
    - Inline string lines (`    "key": "value",`): re-emitted with the
      translated value on the same line.
    """
    with open(src_path, "r", encoding="utf-8") as f:
        src_lines = f.readlines()

    out_lines: list[str] = []
    i = 0

    while i < len(src_lines):
        raw = src_lines[i]
        stripped = raw.strip()

        # ── Blank / whitespace-only ────────────────────────────────────────
        if stripped == "":
            out_lines.append(raw)
            i += 1
            continue

        # ── Pure structural tokens ─────────────────────────────────────────
        if stripped in ("{", "}", "[", "]", "],"):
            out_lines.append(raw)
            i += 1
            continue

        # ── Array-valued entry: `    "key":` (nothing after the colon) ─────
        m_key_only = re.match(r'^(\s*)"([^"]+)":([ ]*)$', raw.rstrip('\n'))
        if m_key_only:
            indent, key, _trailing = m_key_only.groups()
            # Emit key line; always add a single trailing space after ":"
            out_lines.append(f'{indent}{_jstr(key)}: \n')
            i += 1

            # Next line should be `    [`
            open_line = src_lines[i]
            assert open_line.strip() == "[", (
                f"Expected '[' on line {i + 1}, got: {open_line!r}"
            )
            out_lines.append(open_line)
            i += 1

            # Collect translated array items until the closing `]` / `],`
            item_index = 0
            while i < len(src_lines):
                item_raw = src_lines[i]
                item_stripped = item_raw.strip()
                if item_stripped in ("]", "],"):
                    break
                # Each item line: `        "...",` or `        "..."`
                m_item = re.match(
                    r'^(\s*)"((?:[^"\\]|\\.)*)"(,?)$',
                    item_raw.rstrip('\n'),
                )
                if m_item:
                    item_indent, _src_val, comma = m_item.groups()
                    translated = out_data[key][item_index]
                    out_lines.append(
                        f'{item_indent}{_jstr(translated)}{comma}\n'
                    )
                    item_index += 1
                else:
                    out_lines.append(item_raw)
                i += 1

            # Emit closing `]` or `],`
            out_lines.append(src_lines[i])
            i += 1
            continue

        # ── Inline string entry: `    "key": "value",` ────────────────────
        m_inline = re.match(
            r'^(\s*)"([^"]+)":\s*"((?:[^"\\]|\\.)*)"(,?)$',
            raw.rstrip('\n'),
        )
        if m_inline:
            indent, key, _src_val, comma = m_inline.groups()
            translated = out_data[key]
            out_lines.append(f'{indent}{_jstr(key)}: {_jstr(translated)}{comma}\n')
            i += 1
            continue

        # ── Fallback: pass through unchanged ──────────────────────────────
        out_lines.append(raw)
        i += 1

    return "".join(out_lines)

# ──────────────────────────────────────────────────────────────────────────


def main() -> None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    base_dir   = os.path.normpath(os.path.join(script_dir, "..", "..", "Media", "Text"))

    src_path  = os.path.join(base_dir, "English.json")
    dest_path = os.path.join(base_dir, "Runic.json")

    if not os.path.isfile(src_path):
        raise FileNotFoundError(f"Source file not found: {src_path}")

    with open(src_path, "r", encoding="utf-8") as f:
        data: dict = json.load(f)

    runic_data = {key: translate_value(val) for key, val in data.items()}

    os.makedirs(base_dir, exist_ok=True)
    with open(dest_path, "w", encoding="utf-8") as f:
        f.write(dump_source_style(src_path, runic_data))

    print(f"Written: {dest_path}")


if __name__ == "__main__":
    main()