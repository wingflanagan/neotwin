SUBDIRS = mapscrn

bin_PROGRAMS  = ntwattach ntwcat ntwclip ntwclutter ntwcuckoo ntwdialog ntwevent ntwfindtwin ntwlsmsgport ntwlsobj ntwsendmsg ntwsetroot ntwsysmon ntwterm ntwthreadtest
sbin_PROGRAMS = ntwdm

AM_CPPFLAGS = -I$(top_srcdir)/include

ntwattach_SOURCES     = attach.c
ntwcat_SOURCES        = cat.c
ntwclip_SOURCES       = clip.c
ntwclutter_SOURCES    = clutter.c
ntwcuckoo_SOURCES     = cuckoo.c
ntwdialog_SOURCES     = dialog.c
ntwdm_SOURCES         = dm.c
ntwevent_SOURCES      = event.c util.c
ntwfindtwin_SOURCES   = findtwin.c
ntwlsmsgport_SOURCES  = lsmsgport.c
ntwlsobj_SOURCES      = lsobj.c
ntwsendmsg_SOURCES    = sendmsg.c
ntwsetroot_SOURCES    = setroot.c
ntwsysmon_SOURCES     = sysmon.c util.c
ntwterm_SOURCES       = pty.c term.c util.c
ntwthreadtest_SOURCES = threadtest.c

ntwattach_LDADD     = $(LIBTW)
ntwcat_LDADD        = $(LIBTW)
ntwclip_LDADD       = $(LIBTW)
ntwclutter_LDADD    = $(LIBTW)
ntwcuckoo_LDADD     = $(LIBTW)
ntwdialog_LDADD     = $(LIBTW)
ntwdm_LDADD         = $(LIBTW) $(LIBTUTF) $(LIBCRYPT)
ntwevent_LDADD      = $(LIBTW)
ntwfindtwin_LDADD   = $(LIBTW)
ntwlsmsgport_LDADD  = $(LIBTW)
ntwlsobj_LDADD      = $(LIBTW)
ntwsendmsg_LDADD    = $(LIBTW)
ntwsetroot_LDADD    = $(LIBTW)
ntwsysmon_LDADD     = $(LIBTW)
ntwterm_LDADD       = $(LIBTW) $(LIBTUTF)
ntwthreadtest_LDADD = $(LIBTW) $(LIBPTHREAD)

install-exec-hook:
	rm -f             $(DESTDIR)$(bindir)/ntwdetach
	$(LN_S) ntwattach $(DESTDIR)$(bindir)/ntwdetach
