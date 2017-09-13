oled:oled.o ssd1331.o
	gcc -Wall -o oled oled.o ssd1331.o -lbcm2835
ssd1331.o:ssd1331.c ssd1331.h
	gcc -Wall -c ssd1331.c -lbcm2835
oled.o:oled.c ssd1331.h 
	gcc -Wall -c oled.c -lbcm2835
clean:
	rm ssd1331.o oled.o oled
