#!/usr/bin/env bash

export CC="/usr/bin/clang -lm -ldl -std=gnu99"
export LDFLAGS="-fuse-ld=lld"
export AR="/usr/bin/clang"
TMPFILE="$(mktemp 00-build-out.XXXXXX)"
export TMPFILE
export CCFLAGS='-march=native -mtune=native -D_PACKAGE_ast=0 -DSHOPT_SYSRC -D_map_libc=1 -D_BLD_ast=1 -DSHOPT_CMDLIB_BLTIN=1 -D_BLD_cmd=1 -Dvt_threaded=1 -D_hdr_fcntl=1 -D_BLD_DLL=1 -D_BLD_sfio=1 -D_BLD_stdio=1 -D_astimport=1 -D_DLL=1 -D_dll_import=1 -D_lib_unlink=1 -D_hdr_mmap=1 -D_hdr_stdlib=1 -D_sys_types=1 -DAT_FDCWD=1 -DSH_CMDLIB_DIR="/usr/ast/bin" -DSHOPT_CMDLIB_HDR="tmp_gnulinux_builtin_header.h" -D_GNU_SOURCE=1 -D__USE_GNU=1'
#bin/package never only make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' COSHELL="/bin/sh" | tee "${TMPFILE}"
#eval /opt/ast/bin/ksh bin/package verbose only make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' | tee -a "${TMPFILE}"
eval /opt/ast/bin/ksh bin/package verbose only make ast-base PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" HOSTTYPE="linux.i386-64" SHELL='/bin/sh' | tee -a "${TMPFILE}"
#EOF.
