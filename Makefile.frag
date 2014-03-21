

$(srcdir)/jsond_scanner.c: $(srcdir)/jsond_scanner.re
	$(RE2C) -t php_jsond_scanner_defs.h --no-generation-date -bci -o $@ jsond_scanner.re

$(srcdir)/jsond_parser.tab.c: $(srcdir)/jsond_parser.y
	$(YACC) --defines -d $(srcdir)/jsond_parser.y -o $@
