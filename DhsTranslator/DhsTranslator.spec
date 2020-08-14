%define _prefix __auto__
%define gemopt opt
%define name DhsTranslator
%define version 1.1
%define release 3
%define repository gemini

%define debug_package %{nil}

Summary: %{name} Package
Name: %{name}
Version: %{version}
Release: %{release}.%{dist}.%{repository}
License: BSD-3
## Source:%{name}-%{version}.tar.gz
Group: Gemini
Source0: %{name}-%{version}.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-root
BuildArch: %{arch}
Prefix: %{_prefix}
## You may specify dependencies here
BuildRequires:axis2c-unofficial-devel%{?_isa} dhsClient-devel%{?_isa} drama-devel%{?_isa}
Requires: axis2c-unofficial%{?_isa} drama%{?_isa} dhsClient%{?_isa}
## Switch dependency checking off
# AutoReqProv: no

%description
DHS HTTP translator service.

%prep
%setup -n %{name}

%build
make

%install
## Write install instructions here, e.g
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/%{gemopt}/axis2c/services/dhs
cp src/libDhsTranslator.so $RPM_BUILD_ROOT/%{_prefix}/%{gemopt}/axis2c/services/dhs

## if you want to do something after installation uncomment the following
## and list the actions to perform:
#%post

## actions, e.g. /sbin/ldconfig

## If you want to have a devel-package to be generated and do some
## %post-stuff regarding it uncomment the following:
# %post devel

## if you want to do something after uninstallation uncomment the following
## and list the actions to perform. But be aware of e.g. deleting directories,
## see the example below how to do it:
#%postun

## If you want to have a devel-package to be generated and do some
## %postun-stuff regarding it uncomment the following:
# %postun devel

## Its similar for %pre, %preun, %pre devel, %preun devel.

%clean
## Usually you won't do much more here than
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
## list files that are installed here, e.g
## %{_prefix}/zzz/zzz
%{_prefix}/%{gemopt}/axis2c/services/dhs

%changelog
* Thu Apr 2 2015 Javier Luhrs <jluhrs@gemini.edu> 1.0-0
- Packaged module DhsTranslator
