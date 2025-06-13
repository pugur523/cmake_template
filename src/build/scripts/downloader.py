#!/usr/bin/env python3

import os
import requests
import zipfile
import tarfile
from urllib.parse import urlparse
from typing import Optional, Callable


def get_filename_from_url_or_header(url: str, headers: dict) -> str:
    if "content-disposition" in headers:
        for part in headers["content-disposition"].split(";"):
            if part.strip().startswith("filename="):
                return part.split("=", 1)[1].strip(' "')
    return os.path.basename(urlparse(url).path)


def ensure_file_path(dest_dir: str, filename: str) -> str:
    return os.path.join(dest_dir, filename)


def is_supported_archive(filename: str) -> bool:
    exts = [".zip", ".tar", ".tar.gz", ".tgz", ".tar.bz2", ".tar.xz"]
    return any(filename.endswith(ext) for ext in exts)


def extract_archive(file_path: str, dest_dir: str) -> None:
    if file_path.endswith(".zip"):
        with zipfile.ZipFile(file_path, "r") as z:
            z.extractall(dest_dir)
    elif file_path.endswith((".tar.gz", ".tgz", ".tar", ".tar.bz2", ".tar.xz")):
        with tarfile.open(file_path, "r:*") as t:
            t.extractall(dest_dir)
    else:
        raise ValueError(f"Unsupported archive format: {file_path}")


def download(
    dest_dir: str,
    download_url: str,
    extract: Optional[bool] = None,
    filename: str = "",
    post_extract: Optional[Callable[[str], None]] = None,
) -> str:
    """
    Downloads a file and optionally extracts it.

    :param dest_dir: Directory to save the file or extract to
    :param download_url: URL to download
    :param extract: Whether to extract; auto-detected if None
    :param filename: Filename to save as; auto-detected from URL if empty
    :param post_extract: Callable(path) -> None, called after extraction with dest_dir
    :return: Path to the saved file or extracted directory
    """
    os.makedirs(dest_dir, exist_ok=True)

    print(f"Downloading from {download_url} ...")
    resp = requests.get(download_url, stream=True)
    resp.raise_for_status()

    if not filename:
        filename = get_filename_from_url_or_header(download_url, dict(resp.headers))
    file_path = ensure_file_path(dest_dir, filename)

    with open(file_path, "wb") as f:
        for chunk in resp.iter_content(chunk_size=8192):
            if chunk:
                f.write(chunk)

    if extract is None:
        extract = is_supported_archive(filename)

    if extract:
        print(f"Extracting {filename} ...")
        extract_archive(file_path, dest_dir)
        os.remove(file_path)
        if post_extract:
            post_extract(dest_dir)
        print(f"Extracted to: {os.path.abspath(dest_dir)}")
        return os.path.abspath(dest_dir)
    else:
        print(f"Saved to: {os.path.abspath(file_path)}")
        return os.path.abspath(file_path)
