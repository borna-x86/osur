CFLAGS = -Wall -O2
LDFLAGS = -O2

OBJEKTI = e.o r.o t.o
MODUL = mod-OAE.o

$(MODUL): $(OBJEKTI)
	$(LD) -r $^ $(LDFLAGS) -o $@

# brisanje
.PHONY: brisi
brisi:
	rm -f $(OBJEKTI) $(MODUL)
