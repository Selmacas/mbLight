# MAkefile pro úvodní projekt

CC = msp430-gcc

# Jmnéno cíle
TARGET = modbusSvetlo

# Cesty  jednotlivým adresářům
PATH_BIN = ./
PATH_OBJ = obj/


SRCC = main.c
SRCC += modbus_lib/modbus_lib.c
SRCC += modbus_lib/modbus_crc.c


ARCH = -mmcu=msp430g2553

# release build

################################################################################

# Přepínače pro C
CFLAGS = ${ARCH} -std=c99 ${FLAGS}


# Přepínače pro Linker (GCC)
LDFLAGS = ${ARCH}

# Seznam prohledávaných adresářů
INC = -Imodbus_lib

OBJS := ${SRCC:.c=.o}

# Přidání prohledávaných adresářů k příznakům pro CC a CPP
CFLAGS += $(INC)

.PHONY = all clean flash

%.o: %.c
	${CC} $(CFLAGS) -c $< -o $@


all: $(PATH_BIN)$(TARGET).elf

$(PATH_BIN)$(TARGET).elf: ${OBJS}
	$(CC) $(LDFLAGS) -o $@ $^
	msp430-size $(PATH_BIN)$(TARGET).elf


clean:
	rm -vf $(TARGET).elf
	rm -vf ${OBJS}

