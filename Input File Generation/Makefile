# define default varaibles
rows?=4
cols?=4
out_file?=init_gol.txt


comp:
	g++ -o init_gol_gen init_gol_gen.cpp

run:
	./init_gol_gen $(rows) $(cols) $(out_file)

clean:
	rm init_gol_gen