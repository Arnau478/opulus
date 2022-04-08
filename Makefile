CC=gcc

CFLAGS=-DDEBUG

C_SOURCES=$(wildcard src/*.c)

define print_info
	echo -e "\x1b[32m==>\x1b[0m" $(1)
endef

define print_cmd
	echo -e " \x1b[33m>\x1b[0m" $(1)
	eval $(1)
endef

opulus: $(C_SOURCES)
	@$(call print_info,"Building main binary")
	@$(call print_cmd,"$(CC) $^ -o $@ $(CFLAGS)")

clean:
	@$(call print_info,"Cleaning")
	@$(call print_cmd,"rm -f ./opulus")

.PHONY: clean
