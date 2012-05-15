pkglib_LTLIBRARIES = \
	libxspiderplat.la
	
libxspiderplat_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	`pkg-config --cflags libxml-2.0`
	
libxspiderplat_la_LDFLAGS = \
	-version-info 1:0:0 \
	`pkg-config --libs libxml-2.0`
	
libxspiderplat_la_SOURCES = \
	abnfalt.cxx \
	abnfaltch.cxx \
	abnfcon.cxx \
	abnfeof.cxx \
	abnfm.cxx \
	abnfr.cxx \
	abnfralt.cxx \
	abnfrep.cxx \
	abnfrscore.cxx \
	abnfrset.cxx \
	abnfterch.cxx \
	abnfterfn.cxx \
	abnfterstr.cxx \
	uri.cxx
	
libxspiderplat_la_INCLUDES = \
	abnfm.h \
	abnfr.h
