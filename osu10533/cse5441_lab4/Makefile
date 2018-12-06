all: user
user:
	nvcc -dc lab4_arbitrary.cu
	nvcc -O -o lab4_arbitrary  lab4_arbitrary.o /fs/project/PAS1421/transform_cuda.o
	nvcc -dc lab4_simd.cu
	nvcc -O -o lab4_simd  lab4_simd.o /fs/project/PAS1421/transform_cuda.o

clean: 
	rm -rf lab4_arbitrary.o
	rm -rf lab4_simd.o
