Name:       microtube

Summary:    µTube
Version:    3.7.1
Release:    1
Group:      Qt/Qt
License:    GPLv3
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 1.1.31.7
Requires:   nodejs18
Requires:   npm18
Requires:   git
Requires:   libaudioresource-qt
Requires:   mpris-qt5-qml-plugin
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5OpenGL)
BuildRequires:  pkgconfig(Qt5OpenGLExtensions)
BuildRequires:  pkgconfig(audioresource-qt)
BuildRequires:  pkgconfig(gstreamer-webrtc-1.0)
BuildRequires:  pkgconfig(gstreamer-sdp-1.0)
BuildRequires:  pkgconfig(gstreamer-1.0)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(nemo-gstreamer-interfaces-1.0)
BuildRequires:  pkgconfig(json-glib-1.0)
BuildRequires:  qt5-qtsql-devel
BuildRequires:  dbus-glib-devel
BuildRequires:  desktop-file-utils
BuildRequires:  nemo-qml-plugin-notifications-qt5-devel
BuildRequires:  cmake

%description
µtube is alternative youtube client!

%prep
%setup -q -n %{name}-%{version}

%build
%cmake . 
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/applications/%{name}-url.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
