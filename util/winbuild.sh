#!/bin/bash

BIN_NAME=orbvis.exe
MINGW_PATH=/c/msys64/mingw64

rm -rf release
make clean
make -j8 windows
mkdir -p release/bin
cd release/bin
mv ../../$BIN_NAME $BIN_NAME
cp -r ../../res res
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
cp -r $MINGW_PATH/share/icons/Adwaita icons/Adwaita
cd ..
cp -r $MINGW_PATH/ssl ssl
cp ../util/README_WIN.txt README.txt
cp ../LICENSE LICENSE.txt
cd ..
make clean
