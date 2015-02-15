

$(srcdir)/jsond_scanner.c: $(srcdir)/jsond_scanner.re
	$(RE2C) -t $(srcdir)/php_jsond_scanner_defs.h --no-generation-date -bci -o $@ $(srcdir)/jsond_scanner.re

$(srcdir)/jsond_parser.tab.c: $(srcdir)/jsond_parser.y
	$(YACC) --defines=jsond_parser.tab.h -l $(srcdir)/jsond_parser.y -o $@
