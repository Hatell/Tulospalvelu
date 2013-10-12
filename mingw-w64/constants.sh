
export HOST=i686-w64-mingw32

export DEP_PREFIX=/usr/$HOST/sys-root/mingw
export PREFIX=$PWD/$HOST/root

export PKG_CONFIG_LIBDIR=
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig:$DEP_PREFIX/lib/pkgconfig

export LD_LIBRARY_PATH=$PREFIX/lib:$DEP_PREFIX/lib 
export PATH=$PREFIX/bin:$DEP_PREFIX/bin:$PATH

export C_INCLUDE_PATH="$DEP_PREFIX/include"

export ACLOCAL_FLAGS="-I $DEP_PREFIX/share/aclocal"

export CFLAGS="-I$DEP_PREFIX/include -I$PREFIX/include"

export CPATH="$DEP_PREFIX/include:$PREFIX/include"

export LDFLAGS="-L$DEP_PREFIX/lib -L$PREFIX/lib" 

