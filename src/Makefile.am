bin_PROGRAMS = eaman

eaman_SOURCES = \
	main.c 		\
	main.h		\
	gui.c		\
	gui.h 		\
	gui_cb.c 	\
	gui_cb.h 	\
	common_archive.c \
	common_archive.h \
	bzip2.c		\
	bzip2.h		\
	gzip.c		\
	gzip.h		\
	rar.c		\
	rar.h		\
	extract.c	\
	extract.h	\
	settings.c	\
	settings.h	\
	gconf.c		\
	gconf.h		\
	add.c		\
	add.h		\
	utils.c		\
	utils.h		\
	deb.c		\
	deb.h		\
	rpm.c		\
	rpm.h		\
	iso.c		\
	iso.h		\
	zip.c		\
	zip.h		\
	7zip.c		\
	7zip.h		\
	tar.c		\
	tar.h

eaman_CFLAGS = \
	$(GTK_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(HILDON_CFLAGS) \
	$(GCONF_CFLAGS) \
	$(GVFS_CFLAGS) \
	-DDATADIR=\"$(datadir)\" \
	-DSRCDIR=\"$(top_srcdir)\" \
	-DLOCALEDIR=\"$(localedir)\"

eaman_LDADD = \
	$(GTK_LIBS) \
	$(GLIB_LIBS) \
	$(HILDON_LIBS) \
	$(GCONF_LIBS) \
	$(GVFS_LIBS)

INCLUDES = \
	-I${top_srcdir}
