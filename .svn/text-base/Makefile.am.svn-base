## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS =							\
	1.8								\
	dist-bzip2

SUBDIRS = icons po pixmaps src

desktopdir = $(datadir)/applications
desktop_in_files = archmanager.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	LEGGIMI \
	autogen.sh \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	$(desktop_in_files) \
	$(pixmaps_DATA)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	$(desktop_DATA)
