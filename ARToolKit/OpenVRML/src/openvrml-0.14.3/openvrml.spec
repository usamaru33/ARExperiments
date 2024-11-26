Name:           openvrml
Version:        0.14.3
Release:        1
Summary:        VRML97 runtime library.
Copyright:      LGPL
Group:          System Environment/Libraries
Packager:       The OpenVRML Development Team <openvrml-develop@lists.sf.net>
Source:         http://prdownloads.sf.net/openvrml/%{name}-%{version}.tar.gz
URL:            http://openvrml.org
BuildRoot:      %{_tmppath}/%{name}-root
BuildRequires:  pkgconfig >= 0.12.0
BuildRequires:  zlib-devel >= 1.1.3
BuildRequires:  libpng-devel >= 1.0.12
BuildRequires:  libjpeg-devel >= 6b
BuildRequires:  fontconfig-devel >= 2.0
BuildRequires:  freetype-devel >= 2.1.2
BuildRequires:  mozilla-devel >= 1.2.1
BuildRequires:  XFree86-devel >= 4.3
BuildRequires:  glut-devel >= 3.7
Requires:       zlib >= 1.1.3
Requires:       libpng >= 1.0.12
Requires:       libjpeg >= 6b
Requires:       fontconfig >= 2.0
Requires:       freetype >= 2.1.2
Requires:       mozilla >= 1.2.1
Prefix:         /usr

%description
OpenVRML is a VRML support library, including a runtime and facilities for
reading and writing VRML models.


%package devel
Summary:        Headers and static library for developing C++ programs with OpenVRML
Group:          Development/Libraries
Requires:	%{name}
%description devel
Headers and static library that programmers will need to develop C++ programs
using OpenVRML

%package gl
Summary:        OpenGL renderer for OpenVRML
Group:          System Environment/Libraries
Requires:       %{name} = %{version}
Requires:       XFree86-Mesa-libGLU >= 4.3
%description gl
OpenGL renderer for OpenVRML.

%package gl-devel
Summary:        OpenVRML OpenGL renderer headers and static library.
Group:          Development/Libraries
Requires:       %{name}-gl = %{version}
Requires:       XFree86-devel >= 4.3
%description gl-devel
Headers and static library that programmers will need to develop C++ programs
using the OpenVRML OpenGL renderer.

%package -n lookat
Summary:        VRML viewer
Group:          Applications/Multimedia
Requires:       %{name}-gl = %{version}
Requires:       glut >= 3.7
%description -n lookat
GLUT-based VRML viewer.


%prep
%setup


%build
./configure --prefix=%{prefix} CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS"
make %{?_smp_mflags}


%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
make prefix=$RPM_BUILD_ROOT%{prefix} install


%files
%doc AUTHORS COPYING.LESSER ChangeLog NEWS README THANKS
%{prefix}/lib/libopenvrml.so.*

%files devel
%doc AUTHORS COPYING.LESSER ChangeLog NEWS README THANKS
%doc doc/manual
%{prefix}/include/%{name}/openvrml/*.h
%{prefix}/lib/libopenvrml.la
%{prefix}/lib/libopenvrml.a
%{prefix}/lib/libopenvrml.so
%{prefix}/lib/pkgconfig/openvrml.pc

%files gl
%doc AUTHORS COPYING.LESSER ChangeLog NEWS README THANKS
%{prefix}/lib/libopenvrml-gl.so.*

%files gl-devel
%doc AUTHORS COPYING.LESSER ChangeLog NEWS README THANKS
%{prefix}/include/%{name}/openvrml/gl
%{prefix}/lib/libopenvrml-gl.la
%{prefix}/lib/libopenvrml-gl.a
%{prefix}/lib/libopenvrml-gl.so
%{prefix}/lib/pkgconfig/openvrml-gl.pc

%files -n lookat
%doc AUTHORS COPYING.LESSER ChangeLog NEWS README THANKS
%{prefix}/bin/lookat
%{prefix}/share/pixmaps/lookat.xpm


%changelog
* Sun Nov 16 2003  Braden McDaniel  <braden@endoframe.com>
- Removed glut/glut-devel dependency from openvrml-gl/openvrml-gl-devel
  packages.
- Added glut dependency to lookat package.
- Changed openvrml-gl's XFree86 dependency to XFree86-Mesa-libGLU; changed
  minimum version of XFree86 from 4.2 to 4.3.
- Removed unused ARCH_FLAGS variable from "configure" invocation.
- Added _smp_mflags macro to invocation of make.

* Sun Sep 28 2003  Braden McDaniel  <braden@endoframe.com>
- Changed COPYING.LIB to COPYING.LESSER.
- Moved pkg-config metadata files to devel packages.
- Changes for parity with installation directory names.

* Tue Dec 17 2002  Braden McDaniel  <braden@endoframe.com>
- Removed conformance.html from %doc.  This doc now incorporated into doxygen
  docs.

* Tue Dec 10 2002  Braden McDaniel  <braden@endoframe.com>
- Added BuildRequires for pkgconfig, fontconfig-devel, freetype-devel.
- Added Requires for fontconfig, freetype.
- Updated configure arguments.
- Added conformance.html to openvrml's %doc.
- Added API (doxygen) docs to openvrml-devel's %doc.
- Removed GL renderer headers in openvrml-devel.
- Added AUTHORS, COPYING.LIB, ChangeLog, NEWS, README, THANKS to %doc for all
  packages.

* Sun Dec 08 2002  Braden McDaniel  <braden@endoframe.com>
- Removed COPYING from %doc.

* Thu Dec 05 2002  Braden McDaniel  <braden@endoframe.com>
- Added minimum versions to BuildRequires dependencies.
- configure --without-ftflibs.
- Require Mozilla 1.2.1; removed obsolete CPPFLAGS and LDFLAGS when configuring.

* Sun Jun 30 2002  Braden McDaniel  <braden@endoframe.com>
- configure --without-doxygen.
- Removed "doc/conformance.html" from %doc.

* Sat Jun 15 2002  Braden McDaniel  <braden@endoframe.com>
- Use CPPFLAGS and LDFLAGS for Mozilla 1.0.0 when running configure.
- Require Mozilla 1.0.0.

* Mon May 20 2002  Braden McDaniel  <braden@endoframe.com>
- Fixed pkgconfig metafile names.
- Updated dependencies to reflect RPMs shipping with Red Hat 7.3.
- Updated source download URI.
- Removed prefix.
- Added BuildRequires.
- Added Requires for zlib, libpng, libjpeg, mozilla.
- configure --without-jdk.

* Thu Jan 24 2002  Braden McDaniel  <braden@endoframe.com>
- Added package metafiles for pkgconfig.
- Removed autoconf macros for use by client packages.

* Fri Jun 29 2001  Braden McDaniel  <braden@endoframe.com>
- Removed MPL-1.1.txt from %doc. File nolonger distributed.
- Prefer the shorter "sf.net" to "sourceforge.net".

* Wed Jun 6 2001  Braden McDaniel  <braden@endoframe.com>
- Added COPYING to %doc.
- Changed URL to <http://openvrml.org>
- Corrected packager e-mail address.

* Thu Jan 25 2001  Braden McDaniel  <braden@endoframe.com>
- Fixed location of lookat icon in %files section for lookat
- Added Mesa >= 3.2 as a requirement for openvrml-gl.

* Tue Jan 2 2001  Braden McDaniel  <braden@endoframe.com>
- Removed obsolete TODO from %doc.

* Sun Dec 31 2000  Clayton Cottingham <drfrog@smartt.com>
- Change in install section rm command is now recursive

* Fri Dec 15 2000  Braden McDaniel  <braden@endoframe.com>
- Added lookat.xpm to files section for lookat.

* Fri Dec 8 2000  Braden McDaniel  <braden@endoframe.com>
- Changes to reflect library name and header location changes.

* Sun Dec 3 2000  Braden McDaniel  <braden@endoframe.com>
- Removed README.SOUND from %doc

* Sun Nov 26 2000  Braden McDaniel  <braden@endoframe.com>
- Added conformance.html to %doc.
- Removed obsolete COPYING.ANTLR from %doc.
- Changed URL to <http://openvrml.sourceforge.net>.
- Removed obsolete NODES from %doc.

* Thu Nov 23 2000  Braden McDaniel  <braden@endoframe.com>
- Gave much love. Checked into CVS. Still not guaranteed to work, though.

* Tue Oct 24 2000  Braden McDaniel  <braden@endoframe.com>
- Received initial spec file from Clayton Cottingham <drfrog@smartt.com>.
