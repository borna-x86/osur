CFLAGS = -Wall -O2
LDFLAGS = -O2

OBJEKTI = a.o b.o a2.o b2.o 
MODULI = e/mod-OAE.o		#ovisnosti
MODUL = mod-OA.o			#ovaj modul

$(MODUL): $(OBJEKTI) $(MODULI)
	$(LD) -r $^ $(LDFLAGS) -o $@

.PHONY: $(MODULI)
$(MODULI):
	$(MAKE) -C $(dir $@)

# brisanje
.PHONY: brisi
brisi:
	$(MAKE) -C $(dir $(MODULI)) brisi
	rm -f $(OBJEKTI) $(MODUL)
