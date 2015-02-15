

$(srcdir)/jsond_scanner.c: $(srcdir)/jsond_scanner.re
	$(RE2C) -t $(srcdir)/php_jsond_scanner_defs.h --no-generation-date -bci -o $@ $(srcdir)/jsond_scanner.re

$(srcdir)/jsond_parser.tab.c: $(srcdir)/jsond_parser.y
	$(YACC) --defines --skeleton=$(srcdir)/jsond_yacc.c -l $(srcdir)/jsond_parser.y -o $@
