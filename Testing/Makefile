# define default varaibles
procs?=3
in_file?=init_gol.txt
threads?=4
gens?=2
out_file?=result_gol.out


comp:
	# g++ -o test_gol test_gol.cpp
	mpic++ -fopenmp -o test_gol test_gol.cpp

run:
	# ./test_gol $(in_file) $(threads) $(gens) $(out_file)
	mpirun -pernode ./test_gol $(in_file) $(threads) $(gens) $(out_file)

debug:
	# mpiCC -g hybrid_gol.cpp

gdb:
	# mpiexec -d -np $(procs) ./hybrid_gol $(in_file) $(threads) $(gens) $(out_file)

clean:
	rm test_gol