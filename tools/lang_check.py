#!/usr/bin/env python3
"""Every English text of the launcher (L("es", "en") in *.c) must have a Portuguese entry in lang_pt.c, and no entry
may be left over (the [lang] tables carry their own Portuguese row). usage: python tools/lang_check.py   (exit 1 if not)"""
import re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
STR = r'"(?:[^"\\]|\\.)*"'
CALL = re.compile(r'\bL\(\s*(' + STR + r')\s*,\s*((?:' + STR + r'\s*)+)\)')
PAIR = re.compile(r'\{\s*((?:' + STR + r'\s*)+),\s*((?:' + STR + r'\s*)+)\}')


def join(lits):  # "a" "b" -> the C string's text, escapes kept as written
    return "".join(re.findall(STR, lits)).replace('""', "")


def english():
    out = set()
    for f in sorted(ROOT.glob("*.c")):
        if f.name == "lang_pt.c":
            continue
        s = f.read_text(encoding="utf-8")
        out |= {join(m.group(2)) for m in CALL.finditer(s)}
    return out


def table():
    s = (ROOT / "lang_pt.c").read_text(encoding="utf-8")
    return {join(m.group(1)): join(m.group(2)) for m in PAIR.finditer(s)}


def main():
    en, pt = english(), table()
    missing, extra = sorted(en - pt.keys()), sorted(pt.keys() - en)
    for e in missing:
        print("missing pt:", e)
    for e in extra:
        print("unused pt:", e)
    bad = [e for e in pt if e.count("%") != pt[e].count("%")]
    for e in bad:
        print("printf args differ:", e, "->", pt[e])
    print(f"{len(en)} English texts, {len(pt)} Portuguese, {len(missing)} missing, {len(extra)} unused")
    sys.exit(1 if missing or extra or bad else 0)


if __name__ == "__main__":
    main()
