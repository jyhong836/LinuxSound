default: SoundADMain

# test: testALSA openDevice record play
CFLAGS += # header files
DIR_FFT = ./fft/
LIBS += -ldl -lm -lasound
LDFLAGS += # lib files
OBJ += main.o SoundAD.o 
FFTOBJ = $(DIR_FFT)FFT.o $(DIR_FFT)mailbox.o $(DIR_FFT)gpu_fft.o $(DIR_FFT)gpu_fft_twiddles.o $(DIR_FFT)gpu_fft_shaders.o
CXXFALGS += -O2 -g -DCORRECT_AMP # -DVERB

# testALSA: testALSA.cpp
# 	g++ -o testALSA testALSA.cpp $(LIB)

# openDevice: opendevice.cpp
# 	g++ -o opendevice opendevice.cpp $(LIB)

# record: record.cpp
# 	g++ -o record record.cpp $(LIB)

# play: play.cpp
# 	g++ -o play play.cpp $(LIB)

SoundADMain: $(OBJ) $(FFTOBJ)
	$(CXX) $(CXXFALGS) -o SoundADMain $(OBJ) $(FFTOBJ) $(LIBS)

SoundAD.o: SoundAD.cpp SoundAD.h
	$(CXX) $(CXXFALGS) -c -o SoundAD.o SoundAD.cpp 

%.o: %.cpp
	$(CXX) $(CXXFALGS) -c -o $@ $<

clean: 
	rm -f $(OBJ)