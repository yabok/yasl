# package options

Summary:        Yet Another String Library
Name:           yasl
Version:        0.0
Release:        1%{?dist}
License:        BSD
Group:          System Environment/Libraries
URL:            https://github.com/yabok/yasl
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  setup
BuildRequires:  meson

%description
yasl is Yet Another String Library.

%package        devel
Summary:        Development files for yasl
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{?epoch:%{epoch}:}%{version}-%{release}

%description    devel
Development files for yasl

%prep
%autosetup -c

%build
%meson
%meson_build

%check
%meson_test

%install
%meson_install

%files
%defattr(-,root,root,-)
%{_libdir}/libyasl.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/libyasl.so
%{_libdir}/pkgconfig/libyasl.pc
%{_includedir}/yasl.h

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%changelog
* %(date "+%a %b %d %Y") (Automated RPM build) - %{version}-%{release}
- See git log for actual changes.
