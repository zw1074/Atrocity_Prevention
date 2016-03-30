# Check if it has nvcc

all: Data_extract_cuda

Data_extract_cuda: check
	@echo "It has nvcc in libarary!\n"
	nvcc -std=c++11 Data_extract_cuda.cu -o Data_extract_cuda

check:
	@type nvcc >/dev/null 2>&1 || @echo "You don't have nvcc here, we will use default one!\n"; g++ -std=c++11 -fopenmp Data_extract.cpp -o Data_extract

ifeq (yes, ${find Data_extract_cuda})
	@rm Data_extract
endif

##Data_extract_cuda:
##ifndef DOT
##	$(shell echo "You don't have nvcc here, we will use default gcc!\n"); g++ -std=c++11 -fopenmp Data_extract.cpp -o Data_extract
##else
##	echo "It has nvcc in library!\n"
##	nvcc -std=c++11 Data_extract_cuda.cu -o Data_extract_cuda
##endif
