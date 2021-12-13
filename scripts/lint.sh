#!/bin/bash
clang-format --dry-run --Werror -style=file src/*.cpp include/tributary/*.h
