CFLAGS = -Wall -O2
LDFLAGS = -O2

OBJEKTI = prog.o a.o b.o
MODULI = o/mod-O.o

pokreni: prog
	@echo "Building done. Running program."
	@./prog

prog: prog.o
	$(CC) $(LDFLAGS) prog.o -o $@

prog.o: $(MODULI) a.o b.o
	$(LD) -r $^ $(LDFLAGS) -o $@

.PHONY: $(MODULI)
$(MODULI):
	$(MAKE) -C $(dir $@)

# brisanje
.PHONY: brisi
brisi:
	$(MAKE) -C $(dir $(MODULI)) brisi 
	rm -f $(OBJEKTI) prog
