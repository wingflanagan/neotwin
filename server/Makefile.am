SUBDIRS = hw

bin_PROGRAMS       = ntwdisplay ntwin ntwin_server
bin_SCRIPTS        = ntwstart
pkglib_LTLIBRARIES =

if LIBRCPARSE_la
  pkglib_LTLIBRARIES += librcparse.la
endif
if LIBSOCKET_la
  pkglib_LTLIBRARIES += libsocket.la
endif
if LIBTERM_la
  pkglib_LTLIBRARIES += libterm.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include $(LTDLINCL) -DPLUGINDIR="\"$(pkglibdir)\"" -DCONFDIR="\"$(sysconfdir)/twin\""
ntwin_CPPFLAGS        = -I$(top_srcdir)/include $(LTDLINCL) -DBINDIR="\"$(bindir)\""

ntwdisplay_SOURCES    = alloc.cpp display.cpp dl_helper.cpp hw.cpp log.cpp missing.cpp palette.cpp
ntwin_SOURCES         = wrapper.c
ntwin_server_SOURCES  = alloc.cpp builtin.cpp data.cpp dl.cpp dl_helper.cpp draw.cpp extreg.cpp \
                        hw.cpp hw_multi.cpp log.cpp main.cpp methods.cpp missing.cpp palette.cpp \
                        obj/all.cpp obj/display.cpp obj/gadget.cpp obj/group.cpp \
                        obj/id.cpp obj/menu.cpp obj/menuitem.cpp obj/module.cpp obj/msg.cpp obj/msgport.cpp \
                        obj/mutex.cpp obj/obj.cpp obj/row.cpp obj/screen.cpp obj/widget.cpp obj/window.cpp \
                        printk.cpp privilege.cpp rcopt.cpp rcrun.cpp remote.cpp resize.cpp shm.cpp scroller.cpp \
                        util.cpp wm.cpp

librcparse_la_SOURCES = rcparse_tab.cpp rcparse_lex.cpp
libterm_la_SOURCES    = pty.cpp tterm.cpp tty.cpp
libsocket_la_SOURCES  = md5.cpp socket.cpp

ntwdisplay_LDFLAGS    = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)
ntwin_server_LDFLAGS  = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)

librcparse_la_LDFLAGS = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libsocket_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libterm_la_LDFLAGS    = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)

ntwdisplay_LDADD      = $(LIBTW) $(LIBTUTF) $(LIBDL) $(LIBTSTL)
ntwin_server_LDADD    =          $(LIBTUTF) $(LIBDL) $(LIBTSTL)

librcparse_la_LIBADD  = $(LIBTUTF) $(LIBTSTL)
libsocket_la_LIBADD   = $(LIBTUTF) $(LIBTSTL) $(LIBSOCK) $(LIBZ)
libterm_la_LIBADD     = $(LIBTUTF) $(LIBTSTL)
