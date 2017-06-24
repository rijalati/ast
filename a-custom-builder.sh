#!/usr/bin/env bash

export CC="/usr/bin/gcc-4.9 -lm -ldl -std=gnu99"
export AR="/usr/bin/ar"
TMPFILE="$(mktemp 00-build-out.XXXXXX)"
export TMPFILE
export CCFLAGS='-DSHOPT_SYSRC -D_map_libc=1 -D_BLD_ast=1 -DSHOPT_CMDLIB_BLTIN=0 -D_BLD_sfio=1 -DAT_FDCWD=1 -DSH_CMDLIB_DIR="/usr/ast/bin" -DSHOPT_CMDLIB_HDR="tmp_gnulinux_builtin_header.h" -D_GNU_SOURCE=1 -D__USE_GNU=1'
#bin/package never only make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' COSHELL="/bin/sh" | tee "${TMPFILE}"
#eval /opt/ast/bin/ksh bin/package verbose only make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' | tee -a "${TMPFILE}"
eval /opt/ast/bin/ksh bin/package verbose only make ast-base PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' | tee -a "${TMPFILE}"
#EOF.
