Name:           archmanager
Version:        0.0.1
Release:        1%{?dist}
Summary:        Archive manager.

Group:          Applications/Archiving
License:        GPLv2+
#URL:            
Source0:        archmanager_fedora.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  gtk2-devel, libxml2-devel, gettext, desktop-file-utils
Requires:       arj, binutils, bzip2, cpio, gzip, xdg-utils, tar, unzip, zip

%description
 archManager is an application for manipulating
 7z, bzip2, gzip, rar, tar, zip, DEB and RPM files. It allows you to create
 archives and add, extract, and delete files from them. 


%prep
%setup -qn archmanager
%patch0 -p0
./autogen.sh


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%find_lang %{name}
desktop-file-install --vendor fedora                            \
        --dir ${RPM_BUILD_ROOT}%{_datadir}/applications         \
        --delete-original                                       \
        ${RPM_BUILD_ROOT}%{_datadir}/applications/%{name}.desktop


%post
update-desktop-database &> /dev/null ||:
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :


%postun
update-desktop-database &> /dev/null ||:
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :


%clean
rm -rf $RPM_BUILD_ROOT


%files -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog NEWS README TODO


%changelog

