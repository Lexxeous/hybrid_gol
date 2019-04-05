//include necessary libraries
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>


// declare namespace
using namespace std;

// prototype function(s)
void pop_org_gol_map(ifstream &init_data, int **org_mat);
void const_ext_gol_map(int **curr_mat, int org_rows, int org_cols, int **ext_mat);
int pop_nxt_gol_map(int** ext_mat, int ext_rows, int ext_cols, int **nxt_mat);
int get_org_gol_rows(ifstream &init_data);
int get_org_gol_cols(ifstream &init_data);

// start main
int main(int argc, char* argv[])
{
	if(argc != 4)
  {
    cout << "SEQ_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"./<executable> <in_file> <gens> <out_file>\" as format.\n";
    return -4;
  }


	char* cmd_in_file = argv[1]; // input file string
	int gens = atoi(argv[2]); // user specified number of generations
	char* cmd_out_file = argv[3]; // output file string


	cout << "cmd_in_file: " << cmd_in_file << " of type " << typeid(cmd_in_file).name() << endl;
	cout << "gens: " << gens << " of type " << typeid(gens).name() << endl;
	cout << "cmd_out_file: " << cmd_out_file << " of type " << typeid(cmd_out_file).name() << endl;

  
	// char in_file_loc[256];
	// strcpy(in_file_loc, "/../");
	// strcat(in_file_loc, cmd_in_file);

	// char out_file_loc[256];
	// strcpy(out_file_loc, "/../");
	// strcat(out_file_loc, cmd_out_file);


	// cout << "in_file_loc: " << in_file_loc << " of type " << typeid(in_file_loc).name() << endl;
	// cout << "out_file_loc: " << out_file_loc << " of type " << typeid(out_file_loc).name() << endl;

	ifstream in_file;
	fstream out_file;

  in_file.open(cmd_in_file);
  out_file.open(cmd_out_file);
  
  int org_gol_rows = get_org_gol_rows(in_file);
  int org_gol_cols = get_org_gol_cols(in_file);
  int ext_gol_rows = org_gol_rows + 2;
  int ext_gol_cols = org_gol_cols + 2;


  cout << "ROWS:COLS " << org_gol_rows << ":" << org_gol_cols << endl;


  // dynamically allocate the original, extended, and next generation matrices
  int **org_gol_map, **ext_gol_map, **nxt_gol_map;
  org_gol_map = (int**)malloc(org_gol_rows * sizeof(int*));
  ext_gol_map = (int**)malloc(ext_gol_rows * sizeof(int*));
  nxt_gol_map = (int**)malloc(org_gol_rows * sizeof(int*));

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
		nxt_gol_map[n] = (int*)malloc(org_gol_cols * sizeof(int));
	}


	cout << "Original Game of Life Map (BEFORE population):" << endl;
	for(int i = 0; i < org_gol_rows; i++)
	{
		for(int j = 0; j < org_gol_cols; j++)
		{
			org_gol_map[i][j] = -1; // initialize the original generation matrix with all -1s
			cout << org_gol_map[i][j] << "\t";
		}
		cout << endl;
	}

	cout << "Extended Game of Life Map (BEFORE population):" << endl;
	for(int i = 0; i < ext_gol_rows; i++)
	{
		for(int j = 0; j < ext_gol_cols; j++)
		{
			ext_gol_map[i][j] = -2; // initialize the extended generation matrix with all -2s
			cout << ext_gol_map[i][j] << "\t";
		}
		cout << endl;
	}

	cout << "Next Generation Game of Life Map (BEFORE population):" << endl;
	for(int i = 0; i < org_gol_rows; i++)
	{
		for(int j = 0; j < org_gol_cols; j++)
		{
			nxt_gol_map[i][j] = -3; // initialize the next generation matrix with all -3s
			cout << nxt_gol_map[i][j] << "\t";
		}
		cout << endl;
	}


	pop_org_gol_map(in_file, org_gol_map); // populate the original generation matrix with the values given in <in_file>

	cout << "Original Game of Life Map (Generation 1):" << endl;
	for(int i = 0; i < org_gol_rows; i++)
	{
		for(int j = 0; j < org_gol_cols; j++)
		{
			cout << org_gol_map[i][j] << "\t";
		}
		cout << endl;
	}


	const_ext_gol_map(org_gol_map, org_gol_rows, org_gol_cols, ext_gol_map); // construct extended generation matrix based on original matrix

	cout << "Extended Game of Life Map (Generation 1):" << endl;
	for(int i = 0; i < ext_gol_rows; i++)
	{
		for(int j = 0; j < ext_gol_cols; j++)
		{
			cout << ext_gol_map[i][j] << "\t";
		}
		cout << endl;
	}


	pop_nxt_gol_map(ext_gol_map, ext_gol_rows, ext_gol_cols, nxt_gol_map);

	cout << "Next Generation Game of Life Map (Generation 1):" << endl;
	for(int i = 0; i < org_gol_rows; i++)
	{
		for(int j = 0; j < org_gol_cols; j++)
		{
			cout << nxt_gol_map[i][j] << "\t";
		}
		cout << endl;
	}

	// ––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––


	for(int g = 2; g <= gens; g++)
	{
		const_ext_gol_map(nxt_gol_map, org_gol_rows, org_gol_cols, ext_gol_map); //construct the next extended generation matrix based on current generation matrix

		cout << "Extended Game of Life Map: (Generation " << g << ")" << endl;
		for(int i = 0; i < ext_gol_rows; i++)
		{
			for(int j = 0; j < ext_gol_cols; j++)
			{
				cout << ext_gol_map[i][j] << "\t";
			}
			cout << endl;
		}


		int dead_cells = pop_nxt_gol_map(ext_gol_map, ext_gol_rows, ext_gol_cols, nxt_gol_map);

		cout << "Next Generation Game of Life Map: (Generation " << g << ")" << endl;
		for(int i = 0; i < org_gol_rows; i++)
		{
			for(int j = 0; j < org_gol_cols; j++)
			{
				cout << nxt_gol_map[i][j] << "\t";
			}
			cout << endl;
		}

		if(dead_cells == org_gol_rows * org_gol_cols)
		{
			cout << "All cells died at generation " << g << "." << endl;
			break;
		}
	}



	// after all generations have been run OR everyone dies, write the result(s) to the output file
	out_file << org_gol_rows << " " << org_gol_cols << endl;
	for(int i = 0; i < org_gol_rows; i++)
	{
		for(int j = 0; j < org_gol_cols; j++)
		{
			out_file << nxt_gol_map[i][j] << " ";
		}
	}


  in_file.close();
  out_file.close();

  return 0;
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


int pop_nxt_gol_map(int** ext_mat, int ext_rows, int ext_cols, int **nxt_mat)
{
	int curr_sum = 0; // initialize the current neighbor sum
	int dead_cnt = 0; // counter for how many dead cells will be in the next generation

	for(int i = 1; i <= ext_rows-2; i++)
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
	return dead_cnt;
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

