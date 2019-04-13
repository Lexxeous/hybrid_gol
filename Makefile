# define default varaibles
in_file?=init_gol.txt
out_file?=result_gol.out
gens?=20


comp:
	g++ -o seq_gol seq_gol.cpp

run:
	./seq_gol $(in_file) $(gens) $(out_file)

debug:
	# mpiCC -g mpi_test.cpp

gdb:
	# mpiexec -d -np $(procs) ./mpi_bcast $(ruut)

clean:
	rm seq_gol