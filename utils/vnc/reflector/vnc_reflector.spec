%define realname vncreflector
%define name %{realname}
%define ipv6name IPv6_%{realname}
%define ver 1
%define rel 0
%define prefix %{_usr}
%define docdir %{_defaultdocdir}
%define _bindir /usr/local/isabel/bin

Summary : VNC server which acts as a proxy for a number of VNC clients
Summary(es): Servidor VNC que actua como proxy de VNC
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: GPL
Vendor: Isabel Researching Group
Packager: Diego Andres Acosta <acosta@dit.upm.es>
URL: http://dit.upm.es
Group: Applications/Multimedia
Source:http://jungla.dit.upm.es/~acosta/src/IPv6_vncreflector/IPv6_vncreflector.tar.gz 
BuildRoot: %{_tmppath}/%{name}-root
Provides: %{ipv6name}
 
%description

IPv6 VNC Reflector is based on VNC Reflector version 1.4 an has been modified in order to support IPv6. This porting was done by the UPM into the Euro6IX (www.euro6ix.org).

VNC Reflector is a specialized VNC server which acts as a proxy sitting
between real VNC server (a host) and a number of VNC clients. It was
designed to work efficiently with large number of clients.


%prep
%setup -n isabelVNCReflector 

%build
%{__make} 

%install
%{__rm} -rf %{buildroot}
%{__install} -d -m0755 %{buildroot}%{_bindir}
%{__install} -m0755  %{ipv6name} %{buildroot}%{_bindir}

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%doc ChangeLog LICENSE README IPv6Readme vnc_reflector.spec 
%{_bindir}/*

%changelog
* Mon Sep 04 2004 Diego Acosta <acosta@dit.upm.es> - 1.0
- name Changed (IPv6_vncreflector)
- version changed (1.0)
- IPv6 suport added.
- spec file updated.
 

* Mon Jun 30 2003 Dag Wieers <dag@wieers.com> - 1.2.4-0
- Initial package. (using DAR)

