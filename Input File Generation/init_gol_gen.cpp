//include necessary libraries
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// declare namespace
using namespace std;


// prototype function(s)
int gen_rand_bit();
char gen_rand_char_bit();


// start main
int main(int argc, char* argv[])
{
	if(argc != 4)
  {
    cout << "GEN_ERR_ARG:ARGC => Wrong number of command line arguments.\nUse \"./<executable> <gen_rows> <gen_cols> <out_file>\" as format.\n";
    return -4;
  }


  srand(time(NULL));


	int gen_rows_int = atoi(argv[1]); // command line input rows
	int gen_cols_int = atoi(argv[2]); // command line input columns
	char* gen_rows_str = argv[1];
	char* gen_cols_str = argv[2];
	char* cmd_out_file = argv[3]; // command line output file string


	int len = gen_rows_int*gen_cols_int;
	char gen_line_1[3];
	string gen_line_2;

	cout << "ROWS:COLS:LEN:LEN2 " << gen_rows_int << ":" << gen_cols_int << ":" << len << ":" << len*2 << endl;

	strcpy(gen_line_1, gen_rows_str);
	strcat(gen_line_1, " ");
	strcat(gen_line_1, gen_cols_str);
	strcat(gen_line_1, "\n");

	cout << gen_line_1 << endl;

	gen_line_2 = gen_rand_char_bit();
	gen_line_2 += ' ';

	cout << gen_line_2 << endl;

	for(int i = 0; i < (gen_rows_int*gen_cols_int)-1; i++)
	{
		gen_line_2 += gen_rand_char_bit();
		if(i == (gen_rows_int*gen_cols_int)-2)
			break;
		gen_line_2 += ' ';
	}

	ofstream out_file;

  out_file.open(cmd_out_file);

  out_file << gen_line_1;
  out_file << gen_line_2;
  
  out_file.close();
  return 0;
}




int gen_rand_int_bit()
{
	return rand() % 2;
}


char gen_rand_char_bit()
{
	char bits[] = {'0', '1'};
	int bit_index = rand() % 2;

	return bits[bit_index];
}