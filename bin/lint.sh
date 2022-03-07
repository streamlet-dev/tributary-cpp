#!/bin/bash
PARENT=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
SRCDIR=$PARENT/../src
INCDIR=$PARENT/../include
clang-format --dry-run --Werror -style=file $SRCDIR/*.cpp $INCDIR/tributary/*.hpp
