"""docs/ の Markdown から、title 付きのコードブロックを code/ に PlatformIO プロジェクトとして書き出す。

- ```cpp title="src/main.cpp"          → code/<ページ名>/src/main.cpp
- ```cpp title="sender/src/main.cpp"   → code/<ページ名>/sender/src/main.cpp
- ```ini title="platformio.ini"        → そのページのすべてのプロジェクトで使う
- platformio.ini が書かれていないプロジェクトには、標準の platformio.ini を置く

使い方：uv run python tools/extract_code.py
"""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs"
CODE = ROOT / "code"

DEFAULT_INI = """[env:esp32dev]
platform = espressif32 @ 6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
"""

BLOCK = re.compile(r'^([ \t]*)```\w+ title="([^"]+)"\n(.*?)^\1```[ \t]*$', re.M | re.S)


def page_name(md: Path) -> str:
    # docs/01-basics/02-blink.md → 02-blink、docs/02-advanced/wifi.md → wifi
    return md.stem


def main():
    for md in sorted(DOCS.rglob("*.md")):
        files = {}
        for indent, title, body in BLOCK.findall(md.read_text(encoding="utf-8")):
            if not (title.endswith("main.cpp") or title.endswith("platformio.ini")):
                continue
            lines = [line[len(indent):] for line in body.splitlines()]
            files[title] = "\n".join(lines) + "\n"
        if not files:
            continue

        base = CODE / page_name(md)
        projects = {t.rsplit("src/main.cpp", 1)[0] for t in files if t.endswith("src/main.cpp")}
        for project in projects:
            ini = files.get(project + "platformio.ini") or files.get("platformio.ini") or DEFAULT_INI
            out = base / project
            (out / "src").mkdir(parents=True, exist_ok=True)
            (out / "platformio.ini").write_text(ini, encoding="utf-8", newline="\n")
            (out / "src" / "main.cpp").write_text(
                files[project + "src/main.cpp"], encoding="utf-8", newline="\n"
            )
            print(out.relative_to(ROOT).as_posix())


if __name__ == "__main__":
    main()
