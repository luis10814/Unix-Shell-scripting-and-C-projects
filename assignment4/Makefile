-include local.mk

CC ?= cc
PERL ?= perl
CFLAGS ?= -Wall -g
GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0)
GLIB_LDFLAGS = $(shell pkg-config --libs glib-2.0)

# These are the common sources used by the server and the tests
LIB_SOURCES = request.c hdrlist.c mimetypes.c
LIB_OBJS = $(patsubst %.c, %.o, $(LIB_SOURCES))

# All the sources used in the HTTP server
HTTPD_SOURCES = httpd.c $(LIB_SOURCES)
HTTPD_OBJS = $(patsubst %.c, %.o, $(HTTPD_SOURCES))

# These are the tests for internal C components
TEST_SOURCES = $(wildcard test_*.c)
TEST_OBJS = $(patsubst %.c, %.o, $(TEST_SOURCES))

SUBMIT_FILES = $(wildcard *.c *.h *.ch *.sh *.txt) Makefile
HANDOUT_FILES = $(LIB_SOURCES)
HANDOUT_FILES += $(patsubst %.c, %.h, $(LIB_SOURCES))
HANDOUT_FILES += httpdefs.h debug.h mime_type_table.ch Makefile listen.sh

.PHONY: all clean check

all: httpd

clean:
	rm -f httpd run-tests
	rm -f $(LIB_OBJS)
	rm -f run-tests.o $(TEST_OBJS)
	rm -f $(patsubst %.c, %.ch, $(TEST_SOURCES))

check: run-tests
	./run-tests

httpd: $(HTTPD_OBJS)
	$(CC) -o $@ $(HTTPD_OBJS) $(LDFLAGS) $(GLIB_LDFLAGS)

a4-submit.tgz: $(SUBMIT_FILES)
	mkdir -p assignment4-submission
	cp $(SUBMIT_FILES) assignment4-submission
	tar cvzf $@ assignment4-submission

a4-handout.tgz: $(HANDOUT_FILES) assignment4.pdf
	pax -w -s '@^@assignment4/@' -z -f $@ assignment4.pdf $(HANDOUT_FILES) test-site

run-tests: run-tests.o $(TEST_OBJS) $(LIB_OBJS)
	$(CC) -o $@ run-tests.o $(TEST_OBJS) $(LIB_OBJS) $(LDFLAGS) $(GLIB_LDFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(GLIB_CFLAGS) -c -o $@ $<

test_%.ch: test_%.c
	$(PERL) scan-tests.pl $< >.$@.tmp && mv .$@.tmp $@

ifdef MIME_TYPES
mime_type_table.ch: parse-mime.pl $(MIME_TYPES)
	$(PERL) parse-mime.pl $(MIME_TYPES) >$@.tmp && mv $@.tmp $@
endif

deps.mk: $(LIB_SOURCES) $(TEST_SOURCES)
	$(CC) $(CPPFLAGS) $(GLIB_CFLAGS) -MM -MG $(LIB_SOURCES) $(TEST_SOURCES) >$@ || (rm -f $@; exit 2)

tags: $(wildcard *.c *.h)
	ctags *.c *.h

%.pdf: ../%.md
	pandoc -t latex --latex-engine=xelatex -o $@ $<

-include deps.mk
