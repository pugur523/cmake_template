#!/usr/bin/env python3

import tomllib
import sys
from typing import Any

from build_util import project_config_file


def load_project_metadata(file_path: str) -> dict[str, Any]:
    try:
        with open(file_path, "rb") as f:
            data = tomllib.load(f)
            return data["project"]
    except FileNotFoundError:
        print(f"Error: Config file not found: {file_path}", file=sys.stderr)
        sys.exit(1)
    except KeyError:
        print(f"Error: Missing [project] section in {file_path}", file=sys.stderr)
        sys.exit(2)
    except tomllib.TOMLDecodeError as e:
        print(f"Error: Failed to parse TOML: {e}", file=sys.stderr)
        sys.exit(3)


def main() -> int:
    prj = load_project_metadata(project_config_file)

    print(prj.get("name", "<no name>"))
    print(prj.get("main_executable_name", "<no executable name>"))
    print(prj.get("version", "<no version>"))
    print(prj.get("description", "<no description>"))
    print(prj.get("homepage", "<no homepage>"))
    print(prj.get("c_version", "<no c version>"))
    print(prj.get("cxx_version", "<no c++ version>"))
    print(prj.get("author", "<no author>"))
    print(prj.get("author_email", "<no author email>"))

    return 0


if __name__ == "__main__":
    sys.exit(main())
