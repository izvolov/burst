#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import os
import sys

def prepare_includes (root_directory, header_files):
    def get_rel_path (path):
        return os.path.relpath(path, root_directory)

    return [{"file": get_rel_path(file), "code": open(file).read()} for file in header_files]

if len(sys.argv) == 1 + 3:
    code = open(sys.argv[1]).read()
    project_directory = sys.argv[2]
    relative_include_directory = sys.argv[3]

    include_directory = os.path.join(project_directory, relative_include_directory)
    header_files = [os.path.join(directory, file)
        for directory, _, files in os.walk(include_directory) for file in files]

    request = json.dumps({
        "code": code,
        "options": "warning,c++14,boost-1.70",
        "compiler": "gcc-7.3.0",
        "compiler-option-raw": "-I{}".format(relative_include_directory),
        "codes": prepare_includes(project_directory, header_files),
        "save": True})

    print(request)
else:
    usage_template = (
        "Использование: {} "
        "<файл с текстом программы> "
        "<директория проекта> "
        "<относительная директория с заголовками>")

    print(usage_template.format(sys.argv[0]))
