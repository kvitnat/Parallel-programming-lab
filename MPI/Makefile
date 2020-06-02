all: mpi 
	mpiexec -n 2 ./mpi

mpi: mpi.cpp
	mpic++ -o mpi mpi.cpp

clean: 
	rm mpi
