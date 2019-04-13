//include necessary libraries
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <math.h>


// declare namespace
using namespace std;

// prototype function(s)
void pop_org_gol_map(ifstream &init_data, int **org_mat);
void const_ext_gol_map(int **curr_mat, int org_rows, int org_cols, int **ext_mat);
void print_matrix(int **mat, int r, int c);
void fill_matrix(int **mat, int r, int c, int val);
void write_matrix_inline(int **mat, int r, int c, ofstream &f);

int pop_nxt_gol_map(int** ext_mat, int ext_rows, int ext_cols, int **nxt_mat, int th_cnt);
int get_org_gol_rows(ifstream &init_data);
int get_org_gol_cols(ifstream &init_data);

// start main
int main(int argc, char* argv[])
{
	if(argc != 5 && world_rank == 0)
  {
    cout << "HYBRID_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"./<executable> <in_file> <threads> <gens> <out_file>\" as format.\n";
    return -5;
  }


  // initialize the MPI environment
  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size); // get the number of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // get the ranks for each individual process


  double start_time, init_time, gol_time; // scope the time variables


  if(world_rank == 0)
  {
  	start_time = MPI_Wtime(); // wall time at the start of the program	
  }


	char* cmd_in_file = argv[1]; // input file string
	int thread_count = atoi(argv[2]); // user specified number of threads
	int gens = atoi(argv[3]); // user specified number of generations
	char* cmd_out_file = argv[4]; // output file string


	ifstream in_file; // define input file
	ofstream out_file; // define output file

  in_file.open(cmd_in_file);
  out_file.open(cmd_out_file, ofstream::out | ofstream::trunc); // erase and overwrite output file
  

  int org_gol_rows = get_org_gol_rows(in_file); // get the number of rows for original matrix
  int org_gol_cols = get_org_gol_cols(in_file); // get the number of columns for original matrix
  int org_mat_size = org_gol_rows*org_gol_cols; // define total original matrix size

  int ext_gol_rows = org_gol_rows + 2;
  int ext_gol_cols = org_gol_cols + 2;
  int ext_mat_size = ext_gol_rows*ext_gol_cols;

  int nxt_gol_rows = org_gol_rows;
  int nxt_gol_cols = org_gol_cols;
  int nxt_mat_size = nxt_gol_rows*nxt_gol_cols;


  // dont let either of the chunk sizes be equal to 0
  // if(mpi_chunk_sz == 0)
  // 	mpi_chunk_sz = 1;
  // if(omp_chunk_sz == 0)
  // 	omp_chunk_sz = 1;


  if(world_rank == 0)
  {
	  cout << "ROWS:COLS:GENS " << org_gol_rows << ":" << org_gol_cols << ":" << gens<< endl;
	}


  // dynamically allocate the original, extended, and next generation matrices
  int **org_gol_map, **ext_gol_map, **nxt_gol_map;

  org_gol_map = (int**)malloc(org_gol_rows * sizeof(int*));
  ext_gol_map = (int**)malloc(ext_gol_rows * sizeof(int*));
  nxt_gol_map = (int**)malloc(nxt_gol_rows * sizeof(int*));
	
	for(int d = 0; d < org_gol_rows; d++)
	{
		org_gol_map[d] = (int*)malloc(org_gol_cols * sizeof(int));
	}
	for(int r = 0; r < ext_gol_rows; r++)
	{
		ext_gol_map[r] = (int*)malloc(ext_gol_cols * sizeof(int));
	}
	for(int n = 0; n < org_gol_rows; n++)
	{
		nxt_gol_map[n] = (int*)malloc(nxt_gol_cols * sizeof(int));
	}


	// sequentially initialize and print matrices before any population
	if(world_rank == 0)
	{
		cout << "Original Game of Life Map (BEFORE population):" << endl;
		fill_matrix(org_gol_map, org_gol_rows, org_gol_cols, -1);
		print_matrix(org_gol_map, org_gol_rows, org_gol_cols);


		cout << "Extended Game of Life Map (BEFORE population):" << endl;
		fill_matrix(ext_gol_map, ext_gol_rows, ext_gol_cols, -2);
		print_matrix(ext_gol_map, ext_gol_rows, ext_gol_cols);


		cout << "Next Generation Game of Life Map (BEFORE population):" << endl;
		fill_matrix(nxt_gol_map, nxt_gol_rows, nxt_gol_cols, -3);
		print_matrix(nxt_gol_map, nxt_gol_rows, nxt_gol_cols);


		init_time = MPI_Wtime() - start_time; // sequential matrices creation and initialization time

	
		pop_org_gol_map(in_file, org_gol_map); // populate the original generation matrix with the values given in <in_file>
		cout << "Original Game of Life Map (Generation 1):" << endl;
		print_matrix(org_gol_map, org_gol_rows, org_gol_cols);
	}


	MPI_Barrier(MPI_COMM_WORLD); // wait for all processors before extended matrix construction and next matrix population

	if(world_rank == 0)
	{
		const_ext_gol_map(org_gol_map, org_gol_rows, org_gol_cols, ext_gol_map); // construct extended generation matrix based on original matrix
		cout << "Extended Game of Life Map (Generation 1):" << endl;
		print_matrix(ext_gol_map, ext_gol_rows, ext_gol_cols);
	}


	// populate next generation matrix in parallel
	pop_nxt_gol_map(ext_gol_map, ext_gol_rows, ext_gol_cols, nxt_gol_map, thread_count);


	if(world_rank == 0)
	{
		cout << "Next Generation Game of Life Map (Generation 1):" << endl;
		print_matrix(nxt_gol_map, nxt_gol_rows, nxt_gol_cols);
	}
	

	MPI_Barrier(MPI_COMM_WORLD); // wait for all processors after initial generation


	// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
	// START GENERATION LOOP(S):

	for(int g = 2; g <= gens; g++)
	{
		if(world_rank == 0)
		{
			const_ext_gol_map(nxt_gol_map, nxt_gol_rows, nxt_gol_cols, ext_gol_map); //construct the next extended generation matrix based on current generation matrix
			cout << "Extended Game of Life Map: (Generation " << g << ")" << endl;
			print_matrix(ext_gol_map, ext_gol_rows, ext_gol_cols);
		}


		int dead_cells = pop_nxt_gol_map(ext_gol_map, ext_gol_rows, ext_gol_cols, nxt_gol_map, thread_count);

		if(world_rank == 0)
		{
			cout << "Next Generation Game of Life Map: (Generation " << g << ")" << endl;
			print_matrix(nxt_gol_map, nxt_gol_rows, nxt_gol_cols);
		}

		if(dead_cells == org_gol_rows * org_gol_cols)
		{
			if(world_rank == 0)
				cout << "All cells died at generation " << g << "." << endl;
			break;
		}

		MPI_Barrier(MPI_COMM_WORLD); // wait for all processors before continuing to next generation
	}


	// after all generations have been run OR everyone dies, sequentially write the result(s) to the output file
	if(world_rank == 0)
	{
		out_file << org_gol_rows << " " << org_gol_cols << endl;
		write_matrix_inline(nxt_gol_map, nxt_gol_rows, nxt_gol_cols, out_file);
	}

  in_file.close();
  out_file.close();

  MPI_Finalize(); // finalize the MPI environment

  return 0;
}



// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// PROTOTYPED FUNCTIONS:


void const_ext_gol_map(int **curr_mat, int org_rows, int org_cols, int **ext_mat)
{
	// populat the corners of the ext_gol_map, in order, clockwise
	ext_mat[0][0] = curr_mat[org_rows-1][org_cols-1]; // first corner
	ext_mat[0][org_cols+1] = curr_mat[org_rows-1][0]; // second corner
	ext_mat[org_rows+1][org_cols+1] = curr_mat[0][0]; // third corner
	ext_mat[org_rows+1][0] = curr_mat[0][org_cols-1]; // fourth corner

	// copy all values from "org_gol_map" to the center of "ext_gol_map"
	for(int i = 0; i < org_rows; i++)
	{
		for(int j = 0; j < org_cols; j++)
		{
			ext_mat[i+1][j+1] = curr_mat[i][j];
		}
	}

	// copy the top and bottom rows of "org_gol_map", swap them, an assign them to "ext_gol_map"
	for(int j = 0; j < org_cols; j++)
	{
		ext_mat[0][j+1] = curr_mat[org_rows-1][j];
		ext_mat[org_rows+1][j+1] = curr_mat[0][j];
	}

	// copy the left and right cols of "org_gol_map", swap them, an assign them to "ext_gol_map"
	for(int i = 0; i < org_rows; i++)
	{
		ext_mat[i+1][0] = curr_mat[i][org_cols-1];
		ext_mat[i+1][org_cols+1] = curr_mat[i][0];
	}
}


int pop_nxt_gol_map(int** ext_mat, int ext_rows, int ext_cols, int **nxt_mat, int th_cnt)
{
	int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size); // get the number of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // get the ranks for each individual process


  // dont let <th_cnt_per_proc> exceed <mpi_chunk_sz>
  int mpi_max = floor(org_rows / world_size);
  if(th_cnt > mpi_max)
  	th_cnt = mpi_max;


	// public variables per thread
	int dead_cnt = 0; // counter for how many dead cells will be in the next generation
	int org_rows = ext_rows-2;


	if(world_rank == 0)
		cout "MPI_MAX:TH_CNT" << mpi_max << ":" << th_cnt << endl;


  #pragma omp parallel num_threads(th_cnt)
  {
  	// private variables per thread
  	int curr_sum = 0; // initialize the current neighbor sum
  	int thread_ID, th_cnt_per_proc;
		thread_ID = omp_get_thread_num(); // unique thread
		th_cnt_per_proc = omp_get_num_threads(); // quantity of threads per processor

		int mpi_chunk_sz = floor(org_rows / world_size);
	  int omp_chunk_sz = floor(mpi_chunk_sz / th_cnt_per_proc);

		if(world_rank == world_size - 1)
		{
			int btm_mpi_chunk_sz = org_rows - ((world_size - 1) * mpi_chunk_sz);
			omp_chunk_sz = btm_mpi_chunk_sz / th_cnt_per_proc;
		}

		omp_thread_row_start = (mpi_chunk_sz * world_rank) + (omp_chunk_sz * thread_ID);
		omp_thread_row_end = omp_thread_row_start + (omp_chunk_sz - 1);

		if(world_rank == 0 || world_rank == 1)
		{
			cout "thread_ID: " << thread_ID << endl;
		}



		for(int i = omp_thread_row_start + 1; i <= omp_thread_row_end; i++)
		{
			for(int j = 1; j <= ext_cols-2; j++)
			{
				curr_sum = ext_mat[i+1][j] + ext_mat[i-1][j] + ext_mat[i][j+1] + ext_mat[i][j-1] + ext_mat[i-1][j+1] + ext_mat[i+1][j-1] + ext_mat[i+1][j+1] + ext_mat[i-1][j-1];

				if((curr_sum < 2) && (ext_mat[i][j] == 1)) // if live cell has less than 2 neighbors
				{
					nxt_mat[i-1][j-1] = 0; // current cell dies, due to under population
					dead_cnt++;
				}
				else if((curr_sum >= 2 && curr_sum <= 3) && (ext_mat[i][j] == 1)) // if live cell has 2~3 neighbors
				{
					nxt_mat[i-1][j-1] = 1; // current cell lives, thriving to the next generation
				}
				else if(curr_sum > 3 && (ext_mat[i][j] == 1)) // if live cell has more than 3 neighbors
				{
					nxt_mat[i-1][j-1] = 0; // current cell dies, due to overpopulation
					dead_cnt++;
				}
				else if(curr_sum == 3 && (ext_mat[i][j] == 0)) // if dead cell has exactly 3 neighbors
				{
					nxt_mat[i-1][j-1] = 1; // current cell lives, due to repopulation and migration
				}
				else
				{
					nxt_mat[i-1][j-1] = 0; // default to dead cell
					dead_cnt++;
				}

				curr_sum = 0; // reset the current neighbor sum
			}
		}
		if(world_rank == 0 && thread_ID == 0)
			return dead_cnt;
	}
}



void print_matrix(int **mat, int r, int c)
{
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			cout << mat[i][j] << "\t";
		}
		cout << endl;
	}	
}


void fill_matrix(int **mat, int r, int c, int val)
{
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			mat[i][j] = val;
		}
	}	
}


void write_matrix_inline(int **mat, int r, int c, ofstream &f)
{
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			if((i+1)*(j+1) == (r*c))
			{
				f << mat[i][j];
				break;
			}
			f << mat[i][j] << " ";
		}
	}
}


void pop_org_gol_map(ifstream &init_data, int **org_mat)
{
	int org_rows, org_cols, val;

	if(init_data)
	{
		init_data.clear();
		init_data.seekg(0, ios::beg);
		init_data >> org_rows >> org_cols;
	}

	for(int i = 0; i < org_rows; i++)
	{
		for(int j = 0; j < org_cols; j++)
		{
			init_data >> val;
			org_mat[i][j] = val;
		}
	}
}



int get_org_gol_rows(ifstream &init_data)
{
	int r, c;

	if(init_data)
	{
		init_data.clear();
		init_data.seekg(0, ios::beg);
		init_data >> r >> c;
	}

	return r;
}

int get_org_gol_cols(ifstream &init_data)
{
	int r, c;

	if(init_data)
	{
		init_data.clear();
		init_data.seekg(0, ios::beg);
		init_data >> r >> c;
	}

	return c;
}

