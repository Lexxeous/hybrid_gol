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
void pop_def_gol_map(ifstream &init_data, int **def_mat);
void pop_ext_gol_map(int **def_mat, int def_rows, int def_cols, int **ext_mat);
int get_def_gol_rows(ifstream &init_data);
int get_def_gol_cols(ifstream &init_data);

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
  
  int def_gol_rows = get_def_gol_rows(in_file);
  int def_gol_cols = get_def_gol_cols(in_file);
  int ext_gol_rows = def_gol_rows + 2;
  int ext_gol_cols = def_gol_cols + 2;

  cout << "ROWS:COLS " << def_gol_rows << ":" << def_gol_cols << endl;


  int **def_gol_map, **ext_gol_map;
  def_gol_map = (int**)malloc(def_gol_rows * sizeof(int*));
  ext_gol_map = (int**)malloc(ext_gol_rows * sizeof(int*));

	for(int d = 0; d < def_gol_rows; d++)
	{
		def_gol_map[d] = (int*)malloc(def_gol_cols * sizeof(int));
	}

	for(int r = 0; r < ext_gol_rows; r++)
	{
		ext_gol_map[r] = (int*)malloc(ext_gol_cols * sizeof(int));
	}

	cout << "Default Game of Life Map (BEFORE population):" << endl;
	for(int i = 0; i < def_gol_rows; i++)
	{
		for(int j = 0; j < def_gol_cols; j++)
		{
			def_gol_map[i][j] = -1;
			cout << def_gol_map[i][j] << "\t";
		}
		cout << endl;
	}

	cout << "Extended Game of Life Map (BEFORE population):" << endl;
	for(int i = 0; i < ext_gol_rows; i++)
	{
		for(int j = 0; j < ext_gol_cols; j++)
		{
			ext_gol_map[i][j] = -2;
			cout << ext_gol_map[i][j] << "\t";
		}
		cout << endl;
	}


	pop_def_gol_map(in_file, def_gol_map);

	cout << "Default Game of Life Map (AFTER population):" << endl;
	for(int i = 0; i < def_gol_rows; i++)
	{
		for(int j = 0; j < def_gol_cols; j++)
		{
			cout << def_gol_map[i][j] << "\t";
		}
		cout << endl;
	}


	pop_ext_gol_map(def_gol_map, def_gol_rows, def_gol_cols, ext_gol_map);

	cout << "Extended Game of Life Map (AFTER population):" << endl;
	for(int i = 0; i < ext_gol_rows; i++)
	{
		for(int j = 0; j < ext_gol_cols; j++)
		{
			cout << ext_gol_map[i][j] << "\t";
		}
		cout << endl;
	}



  in_file.close();
  out_file.close();

  return 0;
}



void pop_def_gol_map(ifstream &init_data, int **def_mat)
{
	int def_rows, def_cols, val;

	if(init_data)
	{
		init_data.clear();
		init_data.seekg(0, ios::beg);
		init_data >> def_rows >> def_cols;
	}

	for(int i = 0; i < def_rows; i++)
	{
		for(int j = 0; j < def_cols; j++)
		{
			init_data >> val;
			def_mat[i][j] = val;
		}
	}
}



void pop_ext_gol_map(int **def_mat, int def_rows, int def_cols, int **ext_mat)
{
	// populat the corners of the ext_gol_map, in order, clockwise
	ext_mat[0][0] = def_mat[def_rows-1][def_cols-1]; // first corner
	ext_mat[0][def_cols+1] = def_mat[def_rows-1][0]; // second corner
	ext_mat[def_rows+1][def_cols+1] = def_mat[0][0]; // third corner
	ext_mat[def_rows+1][0] = def_mat[0][def_cols-1]; // fourth corner

	// copy all values from "def_gol_map" to the center of "ext_gol_map"
	for(int i = 0; i < def_rows; i++)
	{
		for(int j = 0; j < def_cols; j++)
		{
			ext_mat[i+1][j+1] = def_mat[i][j];
		}
	}

	// copy the top and bottom rows of "def_gol_map", swap them, an assign them to "ext_gol_map"
	for(int j = 0; j < def_cols; j++)
	{
		ext_mat[0][j+1] = def_mat[def_rows-1][j];
		ext_mat[def_rows+1][j+1] = def_mat[0][j];
	}

	// copy the left and right cols of "def_gol_map", swap them, an assign them to "ext_gol_map"
	for(int i = 0; i < def_rows; i++)
	{
		ext_mat[i+1][0] = def_mat[i][def_cols-1];
		ext_mat[i+1][def_cols+1] = def_mat[i][0];
	}
}




int get_def_gol_rows(ifstream &init_data)
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

int get_def_gol_cols(ifstream &init_data)
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

