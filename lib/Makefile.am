pkglib_LTLIBRARIES = \
	libxspiderplat.la
	
libxspiderplat_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	`pkg-config --cflags libxml-2.0`
	
libxspiderplat_la_LDFLAGS = \
	`pkg-config --libs libxml-2.0`
	
libxspiderplat_la_SOURCES = \
	action.cxx
