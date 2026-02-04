#CONTIKI_PROJECT = research_protocol
#all: $(CONTIKI_PROJECT)

# Adaugam inc la include path
#CFLAGS += -Iinc

# Asiguram includerea surselor
#PROJECTDIRS = src inc
#PROJECT_SOURCEFILES += protocol.c

# Activam Rime pentru comunicare simpla
#CONTIKI_WITH_RIME = 1

#CONTIKI = ../..
#include $(CONTIKI)/Makefile.include

CONTIKI_PROJECT = research_protocol
all: $(CONTIKI_PROJECT)

# Adăugăm fișierul secundar
PROJECT_SOURCEFILES += protocol.c

# Forțăm folosirea Rime și dezactivăm IPv6 pentru a evita eroarea de sicslowpan
CONTIKI_WITH_RIME = 1
# Linia de mai jos este cheia pentru eroarea ta actuală:
CONTIKI_WITH_IPV6 = 0

CONTIKI = ../..
include $(CONTIKI)/Makefile.include