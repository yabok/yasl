#!/usr/bin/env bash

print_help() {
	printf "%s\n" "Usage: $BASH_SOURCE [OPTION]...
Configure Makefile and pkg-config file appropriately

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
	pkgconfdir="$prefix"/share/pkgconfig
fi

sed -ri '/(DESTDIR|PREFIX|LIBDIR|INCLUDEDIR|PKGCONFDIR) \?= /d' Makefile
makefile_vars="DESTDIR ?= $destdir
PREFIX ?= $prefix
LIBDIR ?= $libdir
INCLUDEDIR ?= $includedir
PKGCONFDIR ?= $pkgconfdir"
cat - Makefile > Makefile.new <<<"$makefile_vars" && mv Makefile.new Makefile

sed -ri '/(prefix|libdir|includedir)=/d' libyasl.pc
pkgconfig_vars="prefix=$prefix
libdir=$libdir
includedir=$includedir"
cat - libyasl.pc > pc.new <<<"$pkgconfig_vars" && mv pc.new libyasl.pc