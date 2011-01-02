SUBDIRS = \
	common \
	core

bin_PROGRAMS = gpup

gpup_CXXFLAGS = \
	-I$(top_srcdir)/src

gpup_SOURCES = \
	main.hh main.cc

gpup_LDADD = \
	$(top_builddir)/src/core/libPS3PupProcessor.la
