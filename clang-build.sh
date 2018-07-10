#!/usr/bin/env ksh
set -vx

export USE_CLANG=yes
export CC="clang-6.0"
export CCFLAGS=' -m64 -fPIC -std=gnu99 -I/home/rlatimore/include/ast -D_GNU_SOURCE=1 -D__USE_GNU=1 -D_BLD_DLL=1 -D_BLD_ast=1 -D_PACKAGE_ast=1 -D_BLD_cmd=1 -Dvt_threaded=1 -D_hdr_fcntl=1 -D_BLD_sfio=1 -D_BLD_stdio=1 -D_astimport=1 -D_dll_import=1 -DSHOPT_FS_3D=1 -DSHOPT_KIA=1 -DSHOPT_SYSRC -DSHOPT_CMDLIB_BLTIN=1 -DSHOPT_CMDLIB_DIR=/opt/ast/bin -DSHOPT_CMDLIB_HDR=/home/rlatimore/src/git/ast-open/config_ast.h -D_map_libc=1 -DSHOPT_OPTIMIZE=1 -DSHOPT_BASH=1 -fno-omit-frame-pointer -fstrict-aliasing -Wstrict-aliasing -fstrict-overflow -Wstrict-overflow -Wsequence-point -Wno-parentheses -Wno-unused -Wno-trigraphs -Wuninitialized -Waddress -Wmissing-variable-declarations -fdiagnostics-fixit-info -fdiagnostics-print-source-range-info -fdiagnostics-show-category=name -fcolor-diagnostics -fdiagnostics-absolute-paths -fdiagnostics-show-option -fcaret-diagnostics'
export CXX=clang++
export HOSTTYPE=linux.i386-64
export SHELL=/opt/ast/bin/ksh
export COSHELL=/opt/ast/bin/ksh
export PACKAGEROOT="${HOME}/src/git/ast-open"
export PATH="${PWD}/bin:${PATH}"


print "Building debug version..."
#bin/libast_prereq.sh

#time /opt/ast/bin/ksh "${PACKAGEROOT}/bin/package" make ast-ast PACKAGE_OPTIONS=map-libc CCFLAGS=" -v -glldb -g -fno-builtin -O0 ${CCFLAGS}" CC=${CC} HOSTTYPE=${HOSTTYPE} INSTALLROOT="${PACKAGEROOT}/arch/${HOSTTYPE}" SHELL=/opt/ast/bin/ksh
/opt/ast/bin/ksh "${PACKAGEROOT}/bin/package" verbose only make ast-base PACKAGE_OPTIONS=map-libc CCFLAGS="-v -glldb -g -fno-builtin -O0 ${CCFLAGS}" CC=${CC} HOSTTYPE=${HOSTTYPE} INSTALLROOT="${PACKAGEROOT}/arch/${HOSTTYPE}" SHELL=/bin/sh
#print "Building optimized version..."
#time /opt/ast/bin/ksh "${HOME}/src/git/ast-open/bin/package" make ast-open PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" CC="${CC} -O2" HOSTTYPE="${HOSTTYPE}" SHELL='/opt/ast/bin/ksh'

#time /opt/ast/bin/ksh "${HOME}/src/git/ast-open/bin/package" make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" CC="${CC}" HOSTTYPE="${HOSTTYPE}" SHELL='/opt/ast/bin/ksh'
