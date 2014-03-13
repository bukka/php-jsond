

$(srcdir)/jsond_scanner.c: $(srcdir)/jsond_scanner.re
	@(cd $(top_srcdir); $(RE2C) -t php_jsond_scanner_defs.h --no-generation-date -bci -o jsond_scanner.c jsond_scanner.re)

$(srcdir)/jsond_parser.tab.c: $(srcdir)/jsond_parser.y
	@(cd $(top_srcdir); $(YACC) -d $(srcdir)/jsond_parser.y)