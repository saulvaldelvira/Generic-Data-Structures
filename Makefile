.PHONY: default clean libs test examples install uninstall doxygen

SRC = src
BIN = bin

CFILES = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)
OFILES = $(patsubst %.c,%.o,$(CFILES))
TESTFILES = $(wildcard test/*)
EXAMPLES = $(wildcard example/*.c)

CC = cc
CCFLAGS += -Wall -Wextra -pedantic -std=c99 -Wstrict-prototypes -g -fPIC -O3

AR = ar
ARFLAGS = rcs

INSTALL_PATH ?= /usr/local

default: libs

libs: $(OFILES) | $(BIN)/
	@ echo "=> $(BIN)/libGDS.so"
	@ $(CC) $(CCFLAGS) -shared -o ./$(BIN)/libGDS.so $(OFILES)
	@ echo "=> $(BIN)/libGDS-static.a"
	@ $(AR) $(ARFLAGS) ./$(BIN)/libGDS-static.a $(OFILES)

install: $(BIN)/libGDS.so $(BIN)/libGDS-static.a
	  @ echo "INSTALL $(INSTALL_PATH)/lib/libGDS.so"
	  @ echo "INSTALL $(INSTALL_PATH)/lib/libGDS-static.a"
	  @ install -d $(INSTALL_PATH)/lib
	  @ install -m 644 $(BIN)/libGDS* $(INSTALL_PATH)/lib
	  @ echo "INSTALL $(INSTALL_PATH)/include/GDS/"
	  @ install -d $(INSTALL_PATH)/include/GDS
	  @ install -d $(INSTALL_PATH)/include/GDS/util
	  @ install -m 644 $(SRC)/*.h $(INSTALL_PATH)/include/GDS
	  @ install -m 644 $(SRC)/util/*.h $(INSTALL_PATH)/include/GDS/util

$(BIN)/libGDS.so $(BIN)/libGDS-static.a:
	@ $(MAKE) --no-print-directory libs

uninstall:
	  @ echo "RM $(INSTALL_PATH)/lib/libGDS.so"
	  @ echo "RM $(INSTALL_PATH)/lib/libGDS-static.a"
	  @ rm -f $(INSTALL_PATH)/lib/libGDS*
	  @ echo "RM $(INSTALL_PATH)/include/GDS/"
	  @ rm -rf $(INSTALL_PATH)/include/GDS

NO-RUN?= false # If true, only builds the test, without running them
test: $(TESTFILES) libs | $(BIN)/
	@ $(foreach T,$(filter %.c,$(TESTFILES)), \
	  $(CC) $(CCFLAGS) -o $(BIN)/$(patsubst %.c,%.out, $(notdir $(T))) $(T) -L./$(BIN)/ -lGDS-static; \
	  $(NO-RUN) || $(BIN)/$(patsubst %.c,%.out, $(notdir $(T))) || exit 1;)

examples: $(EXAMPLES) $(OFILES) | $(BIN)/
	@ $(foreach F,$(EXAMPLES), \
		echo " CC $(F)" ; \
		$(CC) $(CCFLAGS) -o $(patsubst example/%.c,bin/%,$(F)) $(F) $(OFILES) ;)

doxygen: ./doxygen/
	@ echo -e "\
	/** @mainpage \n \
		@verbinclude README.adoc */" > ./doxygen/doc.doxy
	@ echo "/**" >> doxygen/doc.doxy
	@ find ./src -name '*.h' -exec bash -c ' \
		name={} ; base=$$(basename $$name) ; \
		echo -n " * @file $$name " >> doxygen/doc.doxy ; \
		cat $$name | grep $$base | sed "s/.*$$base -//"' >> doxygen/doc.doxy \;
	@ echo "*/" >> doxygen/doc.doxy
	doxygen .doxyfile
	@ rm -f doxygen/doc.doxy

.c.o:
	@ echo "CC $@"
	@ $(CC) $(CCFLAGS) -o $@ -c $<

%/:
	@ mkdir $@

clean:
	@ rm -f $(OFILES)
	@ rm -rf doxygen
