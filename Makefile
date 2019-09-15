CFLAGS  :=      -Wall -O2 -pie -fPIE -fstack-check -fno-plt -mno-red-zone -Wl,-z,relro,-z,now -ggdb
LDLIBS  :=      -lreadline
CC      :=      gcc

# Shell src
RLS             := rlshell

# Shell commands
SHCMD           := cmd_spec.h commands.[ch]

all: ${RLS}

${RLS}: ${RLS}.c ${SHCMD}
	${CC} ${CFLAGS} ${RLS}.c -o ${RLS} ${LDLIBS}

clean:
	rm -f ${RLS}

