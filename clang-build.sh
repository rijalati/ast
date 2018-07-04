#!/usr/bin/env ksh
set -vx

export USE_CLANG=yes
export CC="clang-6.0"
export CCFLAGS=' -m64 -fPIC -march=native -std=gnu99 -D_GNU_SOURCE=1 -DSHOPT_SYSRC -DSHOPT_CMDLIB_BLTIN=1 -DSH_CMDLIB_DIR=/usr/ast/bin -D_map_libc=1 -fstrict-aliasing -Wstrict-aliasing -fstrict-overflow -Wstrict-overflow -Wsequence-point -Wno-parentheses -Wno-unused -Wno-trigraphs -Wuninitialized -Waddress -Wmissing-variable-declarations -fdiagnostics-fixit-info -fdiagnostics-print-source-range-info -fdiagnostics-show-category=name -flto -fcolor-diagnostics -fdiagnostics-absolute-paths -M -dependency-dot ast-open-deps.dot -dependency-file ast-open-deps.txt' 
export CXX=clang++
export HOSTTYPE=linux.i386-64
export CC
export SHELL=/opt/ast/bin/ksh
export PACKAGEROOT="${HOME}/src/git/ast-open"

print "Building debug version..."
time /opt/ast/bin/ksh "${HOME}/src/git/ast-open/bin/package" make ast-open PACKAGE_OPTIONS=map-libc CCFLAGS=" -v --analyze -g -fstandalone-debug -fdebug-macro -glldb -O0 ${CCFLAGS}" CC=${CC} HOSTTYPE=${HOSTTYPE} INSTALLROOT="${PACKAGEROOT}/arch/${HOSTTYPE}.debug" SHELL=/opt/ast/bin/ksh

#print "Building optimized version..."
#time /opt/ast/bin/ksh "${HOME}/src/git/ast-open/bin/package" make ast-open PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" CC="${CC} -O2" HOSTTYPE="${HOSTTYPE}" SHELL='/opt/ast/bin/ksh'

#time /opt/ast/bin/ksh "${HOME}/src/git/ast-open/bin/package" make ast-ast PACKAGE_OPTIONS='map-libc' CCFLAGS="${CCFLAGS}" CC="${CC}" HOSTTYPE="${HOSTTYPE}" SHELL='/opt/ast/bin/ksh'
