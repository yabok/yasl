#!/usr/bin/env bash

print_help() {
	printf "%s\n" "Usage: $BASH_SOURCE [OPTION]...
Configure Makefile and pkg-config file appropriately

  --cc=COMMAND          C compiler [clang]
  --destdir=DIR         Root directory to install into [/]
  --prefix=DIR          Set the prefix for the other two variables [/usr/local]
  --libdir=DIR          Library code path [PREFIX/lib]
  --includedir=DIR      C header file path [PREFIX/include]
  --pkgconfdir=DIR      Location of pkg-config files [PREFIX/share/pkgconfig]
  --help                Display this help and exit

Report bugs to <yasl@lists.kyriasis.com>."
}

for i in "$@"; do
	case "$1" in
		--cc=*)
			cc="${i#*=}"
			shift
			;;
		--destdir=*)
			destdir="${i#*=}"
			shift
			;;
		--prefix=*)
			prefix="${i#*=}"
			shift
			;;
		--libdir=*)
			libdir="${i#*=}"
			shift
			;;
		--includedir=*)
			includedir="${i#*=}"
			shift
			;;
		--pkgconfdir=*)
			pkgconfdir="${i#*=}"
			shift
			;;
		--help|*)
			print_help
			exit 0
			;;
	esac
done

if [[ -z "$cc" ]]; then
	cc=clang
fi

if [[ -z "$destdir" ]]; then
	destdir=/
fi

if [[ -z "$prefix" ]]; then
	prefix=/usr/local
fi

if [[ -z "$libdir" ]]; then
	libdir="$prefix"/lib
fi

if [[ -z "$includedir" ]]; then
	includedir="$prefix"/include
fi

if [[ -z "$pkgconfdir" ]]; then
	pkgconfdir="$libdir"/pkgconfig
fi

makefile_vars="DESTDIR ?= $destdir
PREFIX ?= $prefix
LIBDIR ?= $libdir
INCLUDEDIR ?= $includedir
PKGCONFDIR ?= $pkgconfdir

CC = $cc
"
cat - Makefile.in > Makefile <<<"$makefile_vars"

pkgconfig_vars="prefix=$prefix
libdir=$libdir
includedir=$includedir"
cat - libyasl.pc.in > libyasl.pc <<<"$pkgconfig_vars"
