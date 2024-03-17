OUTPUT_FOLDER = bin
TEST_FOLDER = result

# Run all
all: serial mpi

# Copy to server
copy-server:
	@echo Inserting executable to server...
	@scp -r bin k02-05@4.145.183.206:/home/k02-05
	@scp hostname k02-05@4.145.183.206:/home/k02-05
	@scp -r test_cases k02-05@4.145.183.206:/home/k02-05
	@scp makefile k02-05@4.145.183.206:/home/k02-05
	@ssh k02-05@4.145.183.206

# Compile the serial
compile-serial: 
	@g++ src/serial/serial.cpp -o $(OUTPUT_FOLDER)/serial -lm

# Run the serial in server
serial:
	@time ./${OUTPUT_FOLDER}/serial < ./test_cases/32.txt > ${TEST_FOLDER}/serial.txt

# Compile the MPI
compile-mpi: 
	@mpicc src/open-mpi/mpi.c -o $(OUTPUT_FOLDER)/mpi -lm

# Run the MPI in server
mpi: 
	@echo Doing chmod
	@chmod +x ${OUTPUT_FOLDER}/mpi
	@echo Running
	@mpirun --hostfile hostname ${OUTPUT_FOLDER}/mpi < ./test_cases/32.txt > ${TEST_FOLDER}/mpi.txt
