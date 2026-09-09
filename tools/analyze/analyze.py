#!/usr/bin/env python3
"""
Ructix OS Kernel Code Analyzer

Counts lines in .S, .c, .h, linker.ld, Makefile.
Tracks growth between runs and saves statistics to results/.code_stats.json.
"""

import os
import sys
import json
import hashlib
from pathlib import Path
from datetime import datetime
from collections import defaultdict
from typing import Dict, List, Tuple


class RuctixAnalyzer:
    def __init__(self, root_path: str):
        self.root_path = Path(root_path)

        script_dir = Path(__file__).parent.resolve()
        results_dir = script_dir / "results"
        results_dir.mkdir(exist_ok=True)
        self.history_file = results_dir / ".code_stats.json"

        self.target_extensions = {
            ".S": "Assembly",
            ".c": "C Source",
            ".h": "C Header",
            ".ld": "Linker Script",
        }

        self.target_files = {
            "Makefile": "Makefile",
            "linker.ld": "Linker Script",
        }

        self.results = {
            "timestamp": datetime.now().isoformat(),
            "total_files": 0,
            "total_lines": 0,
            "total_code": 0,
            "total_comments": 0,
            "total_blanks": 0,
            "by_type": defaultdict(lambda: {"files": 0, "lines": 0, "code": 0, "comments": 0, "blanks": 0}),
            "by_dir": defaultdict(lambda: {"files": 0, "lines": 0}),
            "file_details": [],
            "hash": None,
        }

    def should_analyze(self, file_path: Path) -> bool:
        return file_path.suffix in self.target_extensions or file_path.name in self.target_files

    def get_file_type(self, file_path: Path) -> str:
        if file_path.suffix in self.target_extensions:
            return self.target_extensions[file_path.suffix]
        return self.target_files.get(file_path.name, "Unknown")

    def analyze_file(self, file_path: Path) -> Tuple[int, int, int, int]:
        try:
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                lines = f.readlines()
        except Exception:
            return (0, 0, 0, 0)

        total = len(lines)
        blanks = 0
        comments = 0
        code = 0
        in_multiline = False
        ext = file_path.suffix
        name = file_path.name

        for line in lines:
            stripped = line.strip()

            if not stripped:
                blanks += 1
                continue

            is_comment = False

            if ext == ".S":  # Assembly
                if stripped.startswith(";") or stripped.startswith("#"):
                    is_comment = True
                elif in_multiline and "*/" in stripped:
                    in_multiline = False
                    is_comment = True

            elif ext in (".c", ".h"):  # C / C++
                if stripped.startswith("//"):
                    is_comment = True
                elif stripped.startswith("/*") or in_multiline:
                    is_comment = True
                    if "*/" in stripped:
                        in_multiline = False

            elif ext == ".ld":  # Linker script
                if stripped.startswith("/*") or in_multiline:
                    is_comment = True
                    if "*/" in stripped:
                        in_multiline = False
                elif stripped.startswith("//"):
                    is_comment = True

            elif name == "Makefile":  # Makefile
                if stripped.startswith("#"):
                    is_comment = True

            if is_comment:
                comments += 1
            else:
                code += 1

        return (total, code, comments, blanks)

    def scan(self):
        files = []
        for f in self.root_path.rglob("*"):
            if f.is_file() and self.should_analyze(f):
                files.append(f)
        files.sort()

        for f in files:
            rel_path = f.relative_to(self.root_path)
            ftype = self.get_file_type(f)
            total, code, comments, blanks = self.analyze_file(f)

            self.results["total_files"] += 1
            self.results["total_lines"] += total
            self.results["total_code"] += code
            self.results["total_comments"] += comments
            self.results["total_blanks"] += blanks

            self.results["file_details"].append({
                "path": str(rel_path),
                "type": ftype,
                "total": total,
                "code": code,
                "comments": comments,
                "blanks": blanks,
            })

            self.results["by_type"][ftype]["files"] += 1
            self.results["by_type"][ftype]["lines"] += total
            self.results["by_type"][ftype]["code"] += code
            self.results["by_type"][ftype]["comments"] += comments
            self.results["by_type"][ftype]["blanks"] += blanks

            parent = str(rel_path.parent) if str(rel_path.parent) != "." else "root"
            self.results["by_dir"][parent]["files"] += 1
            self.results["by_dir"][parent]["lines"] += total

        self.results["hash"] = hashlib.md5(
            json.dumps(self.results["file_details"], sort_keys=True).encode()
        ).hexdigest()

    def load_previous(self) -> Dict:
        if self.history_file.exists():
            try:
                with open(self.history_file, "r") as f:
                    return json.load(f)
            except Exception:
                return None
        return None

    def get_growth(self, prev: Dict) -> Dict:
        growth = {"files": 0, "lines": 0, "code": 0}
        if prev:
            growth["files"] = self.results["total_files"] - prev.get("total_files", 0)
            growth["lines"] = self.results["total_lines"] - prev.get("total_lines", 0)
            growth["code"] = self.results["total_code"] - prev.get("total_code", 0)
        return growth

    def save(self):
        with open(self.history_file, "w") as f:
            json.dump(self.results, f, indent=2, default=str)

    def print_report(self):
        prev = self.load_previous()
        growth = self.get_growth(prev)

        print("\n" + "=" * 70)
        print(f"RUCTIX KERNEL ANALYSIS - {datetime.now().strftime('%Y-%m-%d %H:%M')}")
        print("=" * 70)

        print("\n📊 Main Statistics:")
        print(f"   Files:    {self.results['total_files']:4d}")
        print(f"   Lines:    {self.results['total_lines']:5d}")
        print(f"   Code:     {self.results['total_code']:5d}  ({self.results['total_code'] / self.results['total_lines'] * 100:.1f}%)")
        print(f"   Comments: {self.results['total_comments']:4d}  ({self.results['total_comments'] / self.results['total_lines'] * 100:.1f}%)")
        print(f"   Blanks:   {self.results['total_blanks']:4d}  ({self.results['total_blanks'] / self.results['total_lines'] * 100:.1f}%)")

        if prev:
            print("\n📈 Project Growth:")
            print(f"   Files:    {growth['files']:+d}  ({growth['files'] / prev.get('total_files', 1) * 100:+.1f}%)")
            print(f"   Lines:    {growth['lines']:+d}  ({growth['lines'] / prev.get('total_lines', 1) * 100:+.1f}%)")
            print(f"   Code:     {growth['code']:+d}  ({growth['code'] / prev.get('total_code', 1) * 100:+.1f}%)")
            print(f"   Previous: {prev.get('timestamp', 'N/A')[:16]}")
        else:
            print("\n📈 GROWTH: First run — no previous data")

        print("\n📂 BY FILE TYPE:")
        print(f"   {'Type':<15} {'Files':>6} {'Lines':>8} {'Code':>8}")
        print("   " + "-" * 40)
        for ftype, stats in sorted(self.results["by_type"].items(), key=lambda x: x[1]["lines"], reverse=True):
            print(f"   {ftype:<15} {stats['files']:>6} {stats['lines']:>8} {stats['code']:>8}")

        print("\n📁 TOP DIRECTORIES:")
        print(f"   {'Directory':<20} {'Files':>8} {'Lines':>8}")
        print("   " + "-" * 38)
        for dir_name, stats in sorted(self.results["by_dir"].items(), key=lambda x: x[1]["lines"], reverse=True)[:5]:
            display = dir_name if dir_name != "root" else "/"
            print(f"   {display:<20} {stats['files']:>8} {stats['lines']:>8}")

        print("\n🏆 LARGEST FILES:")
        print(f"   {'File':<35} {'Lines':>8} {'Code':>8}")
        print("   " + "-" * 52)
        for f in sorted(self.results["file_details"], key=lambda x: x["total"], reverse=True)[:5]:
            name = f["path"]
            if len(name) > 33:
                name = name[:30] + "..."
            print(f"   {name:<35} {f['total']:>8} {f['code']:>8}")

        self.save()
        print(f"\n💾 Stats saved to: {self.history_file}")
        print("=" * 70)


def main():
    if len(sys.argv) > 1 and sys.argv[1] in ("--help", "-h"):
        print(__doc__)
        print("\nUsage:")
        print("  analyze.py [path]")
        print("  analyze.py --help   Show this help")
        print("\nIf no path is given, analyzes the current directory.")
        return

    path = sys.argv[1] if len(sys.argv) > 1 else os.getcwd()

    if not os.path.exists(path):
        print(f"❌ Error: path '{path}' not found")
        sys.exit(1)

    analyzer = RuctixAnalyzer(path)
    analyzer.scan()
    analyzer.print_report()


if __name__ == "__main__":
    main()