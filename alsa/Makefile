default: SoundAD

# test: testALSA openDevice record play

# LIB= -ldl -lm -lasound
# OBJ= main.o SoundAD.o

# testALSA: testALSA.cpp
# 	g++ -o testALSA testALSA.cpp $(LIB)

# openDevice: opendevice.cpp
# 	g++ -o opendevice opendevice.cpp $(LIB)

# record: record.cpp
# 	g++ -o record record.cpp $(LIB)

# play: play.cpp
# 	g++ -o play play.cpp $(LIB)

SoundAD: $(OBJ)
	g++ -o SoundADMain $(OBJ) $(LIB)

SoundAD.o: SoundAD.cpp SoundAD.h
	g++ -c -o SoundAD.o SoundAD.cpp 

clean: 
	rm -f $(OBJ)