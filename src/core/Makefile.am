noinst_LTLIBRARIES = libPS3PupProcessor.la

libPS3PupProcessor_la_CXXFLAGS = \
	$(LIBXMLPP_CFLAGS) \
	-I$(top_srcdir)/src

#INCLUDES = $(LIBXMLPP_CFLAGS) -I$(top_srcdir)

libPS3PupProcessor_la_SOURCES = \
	PS3PupProcessor.hh PS3PupProcessor.cc

libPS3PupProcessor_la_LIBADD = \
	$(LIBXMLPP_LIBS) \
	$(top_builddir)/src/common/libendianness.la \
	$(top_builddir)/src/common/libstrings.la
