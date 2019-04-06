#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;

#define TYPE_SZ		3	// Type is 3 bits
#define FULL_INST_SZ	32	// Full Instruction is of 32 bits
#define RLE_SZ		3	// RLE uses 3 bits to represent the number of repetitions 
#define LOC_SZ		5	// To represent any location in the 32 bit instruction it takes 5 bits
#define BITMASK_SZ	4	// Size of the bitmask used is 4 bits
#define DICT_IND_SZ	4	// Size of the dictionary index is 4 bits

vector <uint32_t> dict;		// Vector of the dictionary
vector <uint32_t> decomp;	// Vector of decompressed instructions

string comp_inst;		// Compressed string
long cp_comp_inst = 0;		// Current pointer of the compressed string

/*
 * Helper Function for Decompression Begins
 */

/*
 * This function returns the 
 * type of encoding used for compression and
 * increments current pointer of compressed string by 3
 */
static string
get_type ()
{
	string type;

	type = comp_inst.substr (cp_comp_inst, TYPE_SZ);
	cp_comp_inst += TYPE_SZ;

	return type;

}

/* This function returns the
 * full instruction from the compresed instruction string 
 * for no compression type and 
 * increments the current pointer of compressed string by 32
 */
static string
get_full_inst ()
{

	string inst;

	inst = comp_inst.substr (cp_comp_inst, FULL_INST_SZ);
	cp_comp_inst += FULL_INST_SZ;

	return inst;

}

/*
 * This function returns the 
 * RLE number i.e. the number of times the previous 
 * instruction has been repeated and 
 * increments the crrent pointer of compressed string by 3
 */
static string
get_rle ()
{
	string rle;

	rle = comp_inst.substr (cp_comp_inst, RLE_SZ);
	cp_comp_inst += RLE_SZ;

	return rle;

}

/*
 * This function returns the 
 * locations from the MSB where their is a mismatch and 
 * increments the current pointer of compressed string by 5
 */
static string
get_loc ()
{

	string loc;

	loc = comp_inst.substr (cp_comp_inst, LOC_SZ);
	cp_comp_inst += LOC_SZ;

	return loc;

}

/*
 * This function returns the 
 * bitmask used to compress the instruction and
 * increments the current pointer of compressed string by 4
 */
static string 
get_bitmask ()
{

	string bitmask;

	bitmask = comp_inst.substr (cp_comp_inst, BITMASK_SZ);
	cp_comp_inst += BITMASK_SZ;

	return bitmask;

}

/*
 * This function return the 
 * 4 bit dictionary index used for compression and 
 * increments the current pointer of compressed string by 4
 */
static string
get_dict_ind () 
{

	string dict_ind;

	dict_ind = comp_inst.substr (cp_comp_inst, DICT_IND_SZ);
	cp_comp_inst += DICT_IND_SZ;

	return dict_ind;

}

/*
 * This function converts the binary string to decimal number
 */
static int
b2d (string s)
{
	
	int base 	= 0;
	int dec_num 	= 0;
	
	base = 1;

	for (int i = (s.length() - 1); i >= 0; i--) {
		if (s[i] == '1')
			dec_num += base;
		base = base * 2;
	}

	return dec_num;

}

/*
 * Helper Functions for Decompression end
 */

/*
 * No Decompression 
 */
void
no_compression () 
{
	string decomp_inst_str;
	uint32_t decomp_inst;

	decomp_inst_str = get_full_inst();
	decomp_inst 	= stol(decomp_inst_str, nullptr, 2);

	decomp.push_back (decomp_inst);

}

/*
 * RLE Decompression
 */
void
rle_decode () 
{	
	
	string rle_len_str;
	uint32_t decomp_inst;
	int decomp_size = 0;
	int rle_len = 0;
	
	/*
	 * Getting the last instruction from the decompression vector
	 */
	decomp_size = decomp.size();
	decomp_inst = decomp[decomp_size];

	rle_len_str	= get_rle ();
	rle_len		= b2d (rle_len_str);

	/*
	 * rle stores 1 less than the actual times it stores the repetition
	 */

	rle_len += 1;
	while (rle_len) {
		decomp.push_back (decomp_inst);
		rle_len--;
	}

}

/*
 * Bitmask Decompression
 */
void
bitmask_decode () 
{

	string loc_str;
	string bitmask_str;
	string dict_ind_str;
	string decomp_inst_str;
	uint32_t loc;
	uint32_t bitmask;
	uint32_t dict_ind;
	uint32_t decomp_inst;

	loc_str 	= get_loc();
	bitmask_str 	= get_bitmask();
	dict_ind_str 	= get_dict_ind();

	loc 		= stol (loc_str, nullptr, 2);
	bitmask 	= stol (bitmask_str, nullptr, 2);
	dict_ind 	= stol (dict_ind_str, nullptr, 2);
	
	/*assume bitmask  = 00000000 00000000 00000000  00001111
	 * loc = 5
	 * bitmask << 27  = 11110000 00000000 00000000 00000000
	 * bitmask >> loc = 00001111 00000000 00000000 00000000
	 * This left shifting by 27 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */

	bitmask		= bitmask << 27;
	bitmask		= bitmask >> loc;

	decomp_inst_str = dict[dict_ind];
	decomp_inst	= stol (decomp_inst_str, nullptr, 2);

	decomp_inst 	= (decomp_inst) ^ (bitmask);
	
	decomp.push_back(decomp_inst);

}

/*
 * 1 Bit Mismatch
 */
void
one_bit_decode () 
{

	string loc_str;
	string dict_ind_str;
	string decomp_inst_str;
	uint32_t loc;
	uint32_t dict_ind;
	uint32_t mismatch;
	uint32_t decomp_inst;

	loc_str 	= get_loc();
	dict_ind_str 	= get_dict_ind();
	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);

	mismatch 	= 1;
	mismatch	= mismatch << 30;
	mismatch	= mismatch >> loc;

	decomp_inst_str = dict[dict_ind];
	decomp_inst 	= stol (decomp_inst_str, nullptr, 2);

	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

}

/*
 * 2 Bit Consecutive mismatch
 */
void
two_bit_consecutive_decode () 
{

	string loc_str;
	string dict_ind_str;
	string decomp_inst_str;
	uint32_t loc;
	uint32_t dict_ind;
	uint32_t mismatch;
	uint32_t decomp_inst;

	loc_str 	= get_loc();
	dict_ind_str 	= get_dict_ind();
	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	mismatch 	= 11;
	mismatch	= mismatch << 29;
	mismatch	= mismatch >> loc;

	decomp_inst_str = dict[dict_ind];
	decomp_inst 	= stol (decomp_inst_str, nullptr, 2);

	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

}

/*
 * 4 Bit Consecutive mismatch
 */
void
four_bit_consecutive_decode () 
{

	string loc_str;
	string dict_ind_str;
	string decomp_inst_str;
	uint32_t loc;
	uint32_t dict_ind;
	uint32_t mismatch;
	uint32_t decomp_inst;

	loc_str 	= get_loc();
	dict_ind_str 	= get_dict_ind();
	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	mismatch 	= 1111;
	mismatch	= mismatch << 27;
	mismatch	= mismatch >> loc;

	decomp_inst_str = dict[dict_ind];
	decomp_inst 	= stol (decomp_inst_str, nullptr, 2);

	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

}

/*
 * 2 Bit Anywhere mismatch
 */
void
two_bit_anywhere_decode () 
{

	string loc_str_1;
	string loc_str_2;
	string dict_ind_str;
	string decomp_inst_str;
	uint32_t loc_1;
	uint32_t loc_2;
	uint32_t dict_ind;
	uint32_t mismatch_1;
	uint32_t mismatch_2;
	uint32_t decomp_inst;

	loc_str_1 	= get_loc();
	loc_str_2 	= get_loc();
	dict_ind_str 	= get_dict_ind();
	loc_1		= stol (loc_str_1, nullptr, 2);
	loc_2		= stol (loc_str_2, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	mismatch_1 	= 1;
	mismatch_1	= mismatch_1 << 30;
	mismatch_1	= mismatch_1 >> loc_1;
	mismatch_2 	= 1;
	mismatch_2	= mismatch_2 << 30;
	mismatch_2	= mismatch_2 >> loc_2;

	decomp_inst_str = dict[dict_ind];
	decomp_inst 	= stol (decomp_inst_str, nullptr, 2);

	decomp_inst 	= decomp_inst ^ mismatch_1;
	decomp_inst 	= decomp_inst ^ mismatch_2;

	decomp.push_back(decomp_inst);

}

/*
 * Direct Mapping 
 */
void
direct_mapping_decode () 
{

	string dict_ind_str;
	string decomp_inst_str;
	uint32_t dict_ind;
	uint32_t decomp_inst;

	dict_ind_str 	= get_dict_ind ();
	dict_ind	= stol (dict_ind_str, nullptr, 2);

	decomp_inst_str = dict[dict_ind];
	decomp_inst 	= stol (decomp_inst_str, nullptr, 2);

	decomp.push_back(decomp_inst);

}

/*
 * DECODE Function
 */
void
decode () 
{
	int comp_inst_len = 0;
	string type;

	type = get_type();
	comp_inst_len = comp_inst.length();
	
	while (cp_comp_inst < comp_inst_len) {
	
		if (type.compare("000") == 0) {
			no_compression();
		} else if (type.compare("001") == 0) {
			rle_decode();	
		} else if (type.compare("010") == 0) {
			bitmask_decode();	
		} else if (type.compare("011") == 0) {
			one_bit_decode();	
		} else if (type.compare("100") == 0) {
			two_bit_consecutive_decode();	
		} else if (type.compare("101") == 0) {
			four_bit_consecutive_decode();	
		} else if (type.compare("110") == 0) {
			two_bit_anywhere_decode();	
		} else if (type.compare("111") == 0) {
			direct_mapping_decode();
		}	
	}	
}

/*
 * Dictionary forming and compressed string 
 */

void
dict_and_comp_inst ()
{
	
	ifstream fp;
	string line;
	bool is_dict = 0;

	fp.open ("compressed.txt");
		
	while (getline (fp, line)) {
		
		/*
		 * In compressed.txt,
		 * The beginning binary bits are the compressed bits
		 * The last 16 32 bits binary forms the dictionary
		 * xxxx is used the distinction between compressed bits
		 * and dictionary used for compressing the original text
		 */

		if (line.compare("xxxx") == 0) {
			is_dict = 1;
			continue;
		}

		if (!is_dict) {
			comp_inst += line;
		} else if (is_dict) {
			dict.push_back(stol(line, nullptr, 2));
		}

	}
	
	fp.close();

}

int
main () 
{

	dict_and_comp_inst ();

	/*
	 * Printing compressed text and Dictonary
	 */
#if 0
	cout << "Compressed Text: " << endl;
	cout << comp_inst << endl;
	
	cout << "Dictionary: " << endl;
	for (auto &elem : dict) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}
	
#endif

	decode();

	cout << "Decompressed text: " << endl;
	for (auto &elem : decomp) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}

	return 0;

}
