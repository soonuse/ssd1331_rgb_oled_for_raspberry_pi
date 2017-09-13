oled:oled.o ssd1331.o
	gcc -Wall -o oled oled.o ssd1331.o -lwiringPi 
ssd1331.o:ssd1331.c ssd1331.h
	gcc -Wall -c ssd1331.c -lwiringPi
oled.o:oled.c ssd1331.h 
	gcc -Wall -c oled.c -lwiringPi
clean:
	rm ssd1331.o oled.o oled
