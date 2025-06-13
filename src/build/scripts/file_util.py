#!/usr/bin/env python3

import os
import shutil
import sys


def copy(src: str, dst: str, recursive=False):
    if recursive:
        if os.path.exists(src) and os.path.isdir(src):
            shutil.copytree(src, dst)
            return 0
        else:
            print(f"Source {src} is not a valid directory.")
            return 1
    else:
        if os.path.exists(src) and os.path.isfile(src):
            shutil.copy(src, dst)
            return 0
        else:
            print(f"Source {src} is not a valid file.")
            return 1


def move(src: str, dst: str, recursive=False):
    if recursive:
        if os.path.exists(src) and os.path.isdir(src):
            shutil.move(src, dst)
            return 0
        else:
            print(f"Source {src} is not a valid directory.")
            return 1
    else:
        if os.path.exists(src) and os.path.isfile(src):
            shutil.move(src, dst)
            return 0
        else:
            print(f"Source {src} is not a valid file.")
            return 1


def flatten_if_single_dir(path: str):
    items = os.listdir(path)
    if len(items) == 1:
        sole_item = os.path.join(path, items[0])
        if os.path.isdir(sole_item):
            # Move contents of sole_item into path
            for subitem in os.listdir(sole_item):
                src = os.path.join(sole_item, subitem)
                dst = os.path.join(path, subitem)
                shutil.move(src, dst)
            # Remove now-empty directory
            os.rmdir(sole_item)


def main(argv):
    if len(argv) >= 5:
        action = argv[1].lower()
        src = argv[2]
        dst = argv[3]
        recursive = argv[4].lower() == "true"

        if action == "copy":
            return copy(src, dst, recursive)
        elif action == "move":
            return move(src, dst, recursive)
        else:
            print(f"Unknown action: {action}")
            return -2
    else:
        print("Usage: file_util.py [copy|move] src dst recursive")
        return -1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
