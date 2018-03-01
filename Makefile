NAME=cows_n_bulls
CPU_F=8000000

all: exe
	avr-objcopy -O ihex -R .eeprom $(NAME) $(NAME).hex
exe: obj
	avr-gcc -mmcu=atmega328p lcd.o $(NAME).o -o $(NAME)
obj: $(NAME).c lcd
	avr-gcc -Os -DF_CPU=$(CPU_F) -mmcu=atmega328p -c -o $(NAME).o $(NAME).c
lcd: lcd.c lcd.h
	avr-gcc -Os -DF_CPU=$(CPU_F) -mmcu=atmega328p -c -o lcd.o lcd.c
upload: all
	avrdude -c avrisp -p m328p -P /dev/ttyACM0 -b 19200 -U flash:w:$(NAME).hex



clean:
	rm $(NAME)
	rm $(NAME).o
	rm $(NAME).hex
