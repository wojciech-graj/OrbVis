#!/bin/bash

pacman -Syu
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-curl mingw-w64-x86_64-cglm mingw-w64-x86_64-gtk3 mingw-w64-x86_64-libepoxy make vim
