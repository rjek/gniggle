CFLAGS=-g

default:
	@echo "usage: make [target]"
	@echo "target can be one of:"
	@echo "    core              Builds only core code, no front end"
	@echo "    cli               Very dull CLI terminal front end"
	@echo
	@echo "    clean             Clean everything up"

core: libgniggle.a

clean: clean-cli
	rm -rf libgniggle.a game.o solve.o dictionary.o generate.o

libgniggle.a: game.o solve.o dictionary.o generate.o
	rm -rf libgniggle.a
	$(AR) q libgniggle.a game.o solve.o dictionary.o generate.o
	
game.o: game.c game.h dictionary.h generate.h
	$(CC) $(CFLAGS) -o game.o -c game.c
	
solve.o: solve.c solve.h dictionary.h
	$(CC) $(CFLAGS) -o solve.o -c solve.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -o dictionary.o -c dictionary.c
	
generate.o: generate.c generate.h
	$(CC) $(CFLAGS) -o generate.o -c generate.c

# -----------------------------------------------------------------------------
# Front-end build rules
# -----------------------------------------------------------------------------

cli: core frontends/cli/cli.o
	$(CC) -lz -o gniggle.cli frontends/cli/cli.o libgniggle.a
	
clean-cli:
	rm -rf frontends/cli/cli.o gniggle.cli

frontends/cli/cli.o: frontends/cli/cli.c
	$(CC) $(CFLAGS) -I ./ -o frontends/cli/cli.o -c frontends/cli/cli.c
