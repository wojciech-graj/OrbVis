#!/bin/bash

BIN_NAME=orbvis.exe
MINGW_PATH=/c/msys64/mingw64

meson setup builddir
meson compile -C builddir
mkdir -p release/bin
cd release/bin
mv ../../$BIN_NAME $BIN_NAME
ldd $BIN_NAME | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" .
cd ..
mkdir -p lib/gdk-pixbuf-2.0/2.10.0/loaders
cp $MINGW_PATH/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache lib/gdk-pixbuf-2.0/2.10.0/loaders.cache
cp $MINGW_PATH/lib/gdk-pixbuf-2.0/2.10.0/loaders/{libpixbufloader-png.dll,libpixbufloader-svg.dll} lib/gdk-pixbuf-2.0/2.10.0/loaders
mkdir -p share
cd share
mkdir -p glib-2.0/schemas
cp $MINGW_PATH/share/glib-2.0/schemas/gschemas.compiled glib-2.0/schemas/gschemas.compiled
mkdir -p icons
cp -r $MINGW_PATH/share/icons icons
cd ..
mkdir -p ssl/certs
cd ssl/certs
cp -r $MINGW_PATH/ssl/certs/ca-bundle.crt ca-bundle.crt
cd ../..
cp ../util/README_WIN.txt README.txt
cp ../LICENSE LICENSE.txt
cd ..
