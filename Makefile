# define default varaibles
procs?=3
in_file?=init_gol.txt
threads?=3
gens?=20
out_file?=result_gol.out


comp:
	mpiCC -o hybrid_gol hybrid_gol.cpp

run:
	mpirun -np $(procs) ./hybrid_gol $(in_file) $(threads) $(gens) $(out_file)

debug:
	# mpiCC -g hybrid_gol.cpp

gdb:
	# mpiexec -d -np $(procs) ./hybrid_gol $(in_file) $(threads) $(gens) $(out_file)

clean:
	rm hybrid_gol