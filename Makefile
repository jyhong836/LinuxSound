OBJ=main.o sound.o keyboardctl.o
INC=sound.h keyboardctl.h
CON= -g

default:$(INC) $(OBJ) 
	g++ -o sound.elf $(OBJ) $(CON)

main.o:main.cpp
	g++ -c  main.cpp $(CON)

sound.o:sound.cpp sound.h
	g++ -c  sound.cpp $(CON)

keyboardctl.o:keyboardctl.cpp keyboardctl.h
	g++ -c  keyboardctl.cpp $(CON)

run:sound.elf
	./sound.elf -

.PHONY:clean
clean:
	rm $(OBJ)

gdb:sound.elf
	gdb sound.elf