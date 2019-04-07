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
int
no_compression () 
{
	string decomp_inst_str;
	uint32_t decomp_inst;

	decomp_inst_str = get_full_inst();
	/*
	 * if extra zero's are padded at the end of
	 * compressed output than its not required to decode
	 */
	if (decomp_inst_str.length() < 32)
		return -1;

	decomp_inst 	= stol(decomp_inst_str, nullptr, 2);

	decomp.push_back (decomp_inst);

	return 0;

}

/*
 * RLE Decompression
 */
int
rle_decode () 
{	
	
	string rle_len_str;
	uint32_t decomp_inst;
	int decomp_size = 0;
	int rle_len = 0;
	
	/*
	 * decomp_size helps in finding out the last vector element index
	 */	
	decomp_size 	= decomp.size();

	/*
	 * Error checking whether RLE is the first instruction in the compressed string
	 */
	if (decomp_size == 0) 
		return -1;

	/*
	 * Getting the last instruction from the decompression vector
	 */
	decomp_inst 	= decomp[(decomp_size - 1)];
	
	rle_len_str	= get_rle ();
	rle_len		= b2d (rle_len_str);

	/*
	 * rle stores 1 less than the actual times it stores the repetition
	 */
	rle_len += 1;

	/*
	 * Error checking for rle length greater then 8
	 */
	if (rle_len > 8) 
		return -1;
	
	/*
	 * Inserting the decompressed instruction in to 
	 * the decomp vector rle_length times
	 */
	while (rle_len) {
		decomp.push_back (decomp_inst);
		rle_len--;
	}

	return 0;

}

/*
 * Bitmask Decompression
 */
int
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
	if (loc_str.length() != 5)
		return -1;

	bitmask_str 	= get_bitmask();
	if (bitmask_str.length() != 4)
		return -1;

	dict_ind_str 	= get_dict_ind();
	if (dict_ind_str.length() != 4)
		return -1;

	loc 		= stol (loc_str, nullptr, 2);
	bitmask 	= stol (bitmask_str, nullptr, 2);
	dict_ind 	= stol (dict_ind_str, nullptr, 2);
	
	/*assume bitmask  = 00000000 00000000 00000000 00001111
	 * loc = 5
	 * bitmask << 28  = 11110000 00000000 00000000 00000000
	 * bitmask >> loc = 00001111 00000000 00000000 00000000
	 * This left shifting by 28 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */
	bitmask		= bitmask << 28;
	bitmask		= bitmask >> loc;

	decomp_inst 	= dict[dict_ind];
	decomp_inst 	= (decomp_inst) ^ (bitmask);
	
	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * 1 Bit Mismatch
 */
int
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
	if (loc_str.length() != 5)
		return -1;

	dict_ind_str 	= get_dict_ind();
	if (dict_ind_str.length() != 4)
		return -1;

	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);

	/*assume mismatch  = 00000000 00000000 00000000 00000001
	 * loc = 5
	 * mismatch << 31  = 10000000 00000000 00000000 00000000
	 * mismatch >> loc = 00001000 00000000 00000000 00000000
	 * This left shifting by 31 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */
	mismatch 	= 0x01;
	mismatch	= mismatch << 31;
	mismatch	= mismatch >> loc;

	decomp_inst 	= dict[dict_ind];
	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * 2 Bit Consecutive mismatch
 */
int
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
	if (loc_str.length() != 5)
		return -1;

	dict_ind_str 	= get_dict_ind();
	if (dict_ind_str.length() != 4)
		return -1;

	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	/*assume mismatch  = 00000000 00000000 00000000 00000011
	 * loc = 5
	 * mismatch << 30  = 11000000 00000000 00000000 00000000
	 * mismatch >> loc = 00001100 00000000 00000000 00000000
	 * This left shifting by 30 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */
	mismatch 	= 0x03;
	mismatch	= mismatch << 30;
	mismatch	= mismatch >> loc;

	decomp_inst 	= dict[dict_ind];
	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * 4 Bit Consecutive mismatch
 */
int
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
	if (loc_str.length() != 5)
		return -1;

	dict_ind_str 	= get_dict_ind();
	if (dict_ind_str.length() != 4)
		return -1;

	loc		= stol (loc_str, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	/*assume mismatch  = 00000000 00000000 00000000 00001111
	 * loc = 5
	 * mismatch << 28  = 11110000 00000000 00000000 00000000
	 * mismatch >> loc = 00001111 00000000 00000000 00000000
	 * This left shifting by 28 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */
	mismatch 	= 0x0F;
	mismatch	= mismatch << 28;
	mismatch	= mismatch >> loc;

	decomp_inst 	= dict[dict_ind];
	decomp_inst 	= decomp_inst ^ mismatch;

	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * 2 Bit Anywhere mismatch
 */
int
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
	if (loc_str_1.length() != 5)
		return -1;

	loc_str_2 	= get_loc();
	if (loc_str_2.length() != 5)
		return -1;

	dict_ind_str 	= get_dict_ind();
	if (dict_ind_str.length() != 4)
		return -1;

	loc_1		= stol (loc_str_1, nullptr, 2);
	loc_2		= stol (loc_str_2, nullptr, 2);
	dict_ind	= stol (dict_ind_str, nullptr, 2);
	
	/*assume mismatch  = 00000000 00000000 00000000 00000001
	 * loc = 5
	 * mismatch << 31  = 10000000 00000000 00000000 00000000
	 * mismatch >> loc = 00001000 00000000 00000000 00000000
	 * This left shifting by 31 and than right shifting by loc
	 * will bring the bitmask to exact location from left
	 */
	mismatch_1 	= 0x01;
	mismatch_1	= mismatch_1 << 31;
	mismatch_1	= mismatch_1 >> loc_1;
	mismatch_2 	= 0x01;
	mismatch_2	= mismatch_2 << 31;
	mismatch_2	= mismatch_2 >> loc_2;

	decomp_inst 	= dict[dict_ind];
	decomp_inst 	= decomp_inst ^ mismatch_1;
	decomp_inst 	= decomp_inst ^ mismatch_2;

	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * Direct Mapping 
 */
int
direct_mapping_decode () 
{

	string dict_ind_str;
	string decomp_inst_str;
	uint32_t dict_ind;
	uint32_t decomp_inst;

	dict_ind_str 	= get_dict_ind ();
	if (dict_ind_str.length() != 4)
		return -1;

	dict_ind	= stol (dict_ind_str, nullptr, 2);

	decomp_inst	= dict[dict_ind];
	
	decomp.push_back(decomp_inst);

	return 0;

}

/*
 * DECODE Function
 */
void
decode () 
{
	string type;
	int comp_inst_len 		= 0;
	int no_comp			= 0;
	int rle_dec 			= 0;
	int bitmask_dec			= 0;
	int one_bit_dec			= 0;
	int two_bit_consecutive_dec	= 0;
	int four_bit_consecutive_dec	= 0;
	int two_bit_anywhere_dec	= 0;
	int direct_mapping_dec		= 0;

	comp_inst_len = comp_inst.length();

	while (cp_comp_inst < comp_inst_len) {
	
		type = get_type();
		/*
		 * if extra zero's are padded at 
		 * the end of compressed output
		 * but are less than 3
		 */
		if (type.length() < 3) {
			break;
		}

		if (type.compare("000") == 0) {
			
			/* No Compression */
			no_comp = no_compression();

			/* Error Checking */
			if (no_comp == -1) {
				break;
			}

		} else if (type.compare("001") == 0) {
			
			/* RLE based Decompression */
			rle_dec = rle_decode();

			/* Error Checking */
			if (rle_dec == -1) {
				break;
			}

		} else if (type.compare("010") == 0) {

			/* Bitmask based DEcompression */
			bitmask_dec = bitmask_decode();	

			/* Error Checking */
			if(bitmask_dec == -1) {
				break;
			}

		} else if (type.compare("011") == 0) {

			/* One Bit Mismatch based Decompression*/
			one_bit_dec = one_bit_decode();

			/* Error Checking */
			if (one_bit_dec == -1) {
				break;
			}

		} else if (type.compare("100") == 0) {

			/* Two Bit Consecutive Mismatch based Decompression */
			two_bit_consecutive_dec = two_bit_consecutive_decode();	

			/* Error Checking */
			if (two_bit_consecutive_dec == -1) {
				break;
			}

		} else if (type.compare("101") == 0) {

			/* Four Bit Consecutive Mismatch based Decompression */
			four_bit_consecutive_dec = four_bit_consecutive_decode();

			/* Error Checking */
			if (four_bit_consecutive_dec == -1) {
				break;
			}

		} else if (type.compare("110") == 0) {

			/* Two Bit Anywhere based Decompression */
			two_bit_anywhere_dec = two_bit_anywhere_decode();	

			/* Error Checking */
			if (two_bit_anywhere_dec == -1) {
				break;
			}

		} else if (type.compare("111") == 0) {

			/* Direct Mapping based Decompression */
			direct_mapping_dec = direct_mapping_decode();

			/* Error Checking */
			if (direct_mapping_dec == -1) {
				break;
			}

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

//	comp_inst = "011010110000" ; 

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

	/*
	 * Printing the Decompressed Output
	 */
//#if 0
	cout << "Decompressed text: " << endl;
	for (auto &elem : decomp) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}
//#endif

	return 0;

}
