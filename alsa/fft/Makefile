default: all_obj

HEX = hex/shader_256.hex \
    hex/shader_512.hex \
    hex/shader_1k.hex \
    hex/shader_2k.hex \
    hex/shader_4k.hex \
    hex/shader_8k.hex \
    hex/shader_16k.hex \
    hex/shader_32k.hex \
    hex/shader_64k.hex \
    hex/shader_128k.hex

OBJ = FFT.o mailbox.o gpu_fft.o gpu_fft_twiddles.o gpu_fft_shaders.o

# B = hello_fft.bin

F = -lrt -lm

all_obj: $(OBJ)

# fft.o: $(OBJ) $(HEX)
# 	$(CXX) $^ -c -o $@

test: $(OBJ) test.o
	$(CXX) $^ -o $@ $(F)

%.o: %.c
	$(CC) $(CCFLAGS) -O2 -c -o $@ $<

clean:
	rm -f $(OBJ) fft
