OBJEKTI = n2.o o2.o o.o r2.o r.o n.o 
CFLAGS = -Wall -O2
LDFLAGS = -O2

MODULI = a/mod-OA.o		#ovisnosti
MODUL = mod-O.o			#ovaj modul

$(MODUL): $(MODULI) $(OBJEKTI)
	$(LD) -r $^ $(LDFLAGS) -o $@

.PHONY: $(MODULI)
$(MODULI):
	$(MAKE) -C $(dir $@)

# brisanje
.PHONY: brisi
brisi:
	$(MAKE) -C $(dir $(MODULI)) brisi
	rm -f $(OBJEKTI) $(MODUL)
