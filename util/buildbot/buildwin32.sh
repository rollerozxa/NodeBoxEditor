#!/bin/bash
# Taken from https://github.com/minetest/minetest LGPL 2.1 or later

set -e

dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ $# -ne 1 ]; then
	echo "Usage: $0 <build directory>"
	exit 1
fi
builddir=$1
mkdir -p $builddir
mkdir -p $builddir/bin
builddir="$( cd "$builddir" && pwd )"
packagedir=$builddir/packages
libdir=$builddir/libs

toolchain_file=$dir/toolchain_mingw.cmake
mingwdlldir="/usr/i686-w64-mingw32/bin"

mkdir -p $packagedir
mkdir -p $libdir

cd $builddir

# Get nodeboxeditor
cd $builddir
if [ ! "x$EXISTING_nodeboxeditor_DIR" = "x" ]; then
	ln -s $EXISTING_nodeboxeditor_DIR nodeboxeditor
else
	[ -d nodeboxeditor ] && (cd nodeboxeditor && git pull) || (git clone https://github.com/ROllerozxa/nodeboxeditor)
fi
cd nodeboxeditor
git_hash=`git show | head -c14 | tail -c7`

# Build the thing
[ -d _build ] && rm -Rf _build/
mkdir _build
cd _build
cmake .. -G Ninja \
	-DCMAKE_TOOLCHAIN_FILE=$toolchain_file \
	-DCMAKE_INSTALL_PREFIX=/tmp

ninja

# Package the shit
mkdir -p temp_package/
mkdir -p temp_package/nodeboxeditor
cp ../bin/nodeboxeditor.exe temp_package/
cp lib/irrbloss/bin/Win32-gcc/Irrbloss.dll temp_package/
cp $mingwdlldir/lib{{gcc_s_dw2-1,png16-16,ssp-0,stdc++-6,winpthread-1},zlib1}.dll temp_package/
cp -r ../media/ temp_package/
