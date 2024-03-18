# DIRECTORY
OUTPUT_FOLDER = bin
TEST_FOLDER = result

# Run all
all: serial mpi

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
	@time ./${OUTPUT_FOLDER}/serial < ./test_cases/32.txt > ${TEST_FOLDER}/serial.txt

# OPEN MPI
# Compile the MPI
compile-mpi: 
	@mpicc src/open-mpi/open-mpi.cpp -g -o $(OUTPUT_FOLDER)/open-mpi -lm -lstdc++ -lmpi_cxx -lmpi

# Run the MPI in server
mpi: 
	@echo Doing chmod
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@echo Running
	@mpirun --hostfile hostname ./${OUTPUT_FOLDER}/open-mpi < ./test_cases/32.txt > ${TEST_FOLDER}/open-mpi.txt

# Run the MPI in local
mpi-local: 
	@echo Doing chmod
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@echo Running
	@time mpirun -n 4 ${OUTPUT_FOLDER}/open-mpi < test_cases/32.txt > ${TEST_FOLDER}/open-mpi.txt

# Run the MPI debug in local
mpi-debug: 
	@echo Doing chmod
	@chmod +x ${OUTPUT_FOLDER}/open-mpi
	@echo Running
	@mpirun -n 4 valgrind --track-origins=yes ${OUTPUT_FOLDER}/open-mpi < test_cases/32.txt > ${TEST_FOLDER}/open-mpi.txt