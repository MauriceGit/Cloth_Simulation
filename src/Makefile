# Quelldateien
SRCS             = texture.c vector.c main.c scene.c io.c particle.c logic.c stringOutput.c 

# ausfuehrbares Ziel
TARGET           = ueb05

# Pfad zum ImageLoader
IMGLOADER_DIR = ./imageLoader

# Name der ImageLoader-Bibliothek
IMGLOADER_LIB = libcgimage.a

# Ist libz verfügbar?
HAVE_LIBZ = yes


# Basis-Makefile einbinden
-include ./Makefile.common


# Precompiler flags
# Pfad der Schnittstelle des ImageLoaders hinzufuegen
CPPFLAGS_COMMON  += -I$(IMGLOADER_DIR)/include

# Linker flags
# Pfad der ImageLoader-Bibliothek hinzufuegen
LDFLAGS_COMMON  += -L$(IMGLOADER_DIR)/lib

# Linker libraries
# ImageLoader-Bibliothek linken
LDLIBS += -lcgimage

# Wenn libz verfuegbar ist, muessen wir sie mit einlinken, um die
# Bibliothek auch zum Laden von PNG-Bildern verwenden zu koennen.
ifeq ($(HAVE_LIBZ),yes)
LDLIBS += -lz
endif


.PHONY: distcleanimageloader


# TARGETS

# Programm ist von der ImageLoader-Bibliothek abhaengig
$(TARGET): $(IMGLOADER_DIR)/lib/$(IMGLOADER_LIB)

# Regel zur Erstellung der ImageLoader-Bibliothek
$(IMGLOADER_DIR)/lib/$(IMGLOADER_LIB):
	(cd $(IMGLOADER_DIR) && ./configure)
	$(MAKE) -C $(IMGLOADER_DIR)

# Vollstaendiges Aufraeumen beinhaltet auch Aufraeumen des ImageLoaders
distclean: distcleanimageloader

# Regel zum Aufraeumen des ImageLoaders
distcleanimageloader:
	if [ -f $(IMGLOADER_DIR)/Makefile ]; then $(MAKE) -C $(IMGLOADER_DIR) HAVE_LIBZ=$(HAVE_LIBZ) distclean; fi
