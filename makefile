# DIRECTORY
OUTPUT_FOLDER = bin
TEST_FOLDER = result

# JOIN
# Run all
all-local: openmp-local mpi-local serial
all-compile: compile-serial compile-mpi compile-openmp
all: openmp mpi serial

# COMPARE
compare:
	@diff -s ${TEST_FOLDER}/cuda.txt ${TEST_FOLDER}/open-mp.txt

# TEST CASE
# Make test case
tc:
	@./test_cases/generator

# SERVER
# Copy to server
copy-server:
	@eval $$(ssh-agent) && \
		ssh-add ~/.ssh/k02-05 && \
		echo "Inserting executables to server..." && \
		scp -r bin hostname makefile k02-05@4.145.183.206:/home/k02-05 && \
		ssh k02-05@4.145.183.206

# Copy to branch server
copy-branch-server:
	@scp -r bin hostname makefile k02-05@10.0.1.7:/home/k02-05
	@scp -r bin hostname makefile k02-05@10.0.1.4:/home/k02-05

# SERIAL
# Compile the serial
compile-serial: 
	@g++ src/serial/serial.cpp -o $(OUTPUT_FOLDER)/serial -lm

# Run the serial in server
serial:
	@time ./${OUTPUT_FOLDER}/serial < ./test_cases/2048.txt > ${TEST_FOLDER}/serial.txt

# OPEN MPI
# Compile the MPI
compile-mpi: 
	@mpicc src/open-mpi/open-mpi.cpp -g -o $(OUTPUT_FOLDER)/open-mpi -lm -lstdc++ -lmpi_cxx -lmpi

# Run the MPI in server
mpi: 
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@time mpirun --hostfile hostname ./${OUTPUT_FOLDER}/open-mpi < ./test_cases/2048.txt > ${TEST_FOLDER}/open-mpi.txt

# Run the MPI in local
mpi-local: 
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@time mpirun -n 4 ${OUTPUT_FOLDER}/open-mpi < test_cases/2048.txt > ${TEST_FOLDER}/open-mpi.txt

# Run the MPI debug in local
mpi-debug: 
	@echo Doing chmod
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@echo Running
	@mpirun -n 4 valgrind --track-origins=yes ${OUTPUT_FOLDER}/open-mpi < test_cases/2048.txt > ${TEST_FOLDER}/open-mpi.txt

# OPEN MP
# Compile the OPEN MP
compile-openmp: 
	@g++ src/open-mp/open-mp.cpp --openmp -o $(OUTPUT_FOLDER)/open-mp -lm

# Run the OPEN MP in local
openmp-local: 
	@chmod +x ${OUTPUT_FOLDER}/open-mp
	@time ${OUTPUT_FOLDER}/open-mp < test_cases/2048.txt > ${TEST_FOLDER}/open-mp.txt

# Run the OPEN MP in server
openmp: 
	@chmod +x ${OUTPUT_FOLDER}/open-mp
	@time ${OUTPUT_FOLDER}/open-mp < test_cases/2048.txt > ${TEST_FOLDER}/open-mp.txt

# CUDA
# Compile the CUDA
compile-cuda: 
	@nvcc src/cuda/cuda.cu -o $(OUTPUT_FOLDER)/cuda -lm

cuda:
	@time ${OUTPUT_FOLDER}/cuda < test_cases/2048.txt > ${TEST_FOLDER}/cuda.txt