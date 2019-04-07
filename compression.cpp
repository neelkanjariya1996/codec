#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <bits/stdc++.h>
#include <map>
#include <iterator>
#include <utility>

using namespace std;

#define INST_BITS 	32	// Instructions are 32 bits 
#define DICT_SZ		16	// Dctionary size is 16	

class freq_data 
{

	public:
	int count;		// Number of occurences 
	int order;		// Insertion order	

	freq_data (int count1, int order1) {

		count = count1;
		order = order1;
	}

};

/*
 * Table to calculate the number of occurences 
 * of each instruction in the input file
 */
unordered_map<uint32_t, freq_data> inst_freq;

/*
 * Dictionary of Instruction
 */
vector <uint32_t> dict;

/*
 * The instructions from the file are
 * stored in instruction vector
 */
vector <uint32_t> instruction;

/*
 * This function helps in sorting 
 * the vector elems in descending order
 * of count and ascending order of 
 * insertion arrival
 */
typedef function <bool (pair <uint32_t, freq_data>, pair <uint32_t, freq_data>)> comparator;

comparator cmp = [] (pair <uint32_t, freq_data> elem1, pair <uint32_t, freq_data> elem2) {

	freq_data d1 = elem1.second;
	freq_data d2 = elem2.second;

	/*
	 * If the number of repetitions of 
	 * 2 instructions is same than sort them 
	 * on the basis of their insertion arrival
	 */
	if (d1.count == d2.count) {

		return d1.order < d2.order;
	}

	/*
	 * IF the count of instruction is 
	 * different than return the 
	 * instruction which has a bigger count
	 */
	return d1.count > d2.count;
};

/*
 * This function prints the instruction 
 * and the index of each instruction in 
 * the dictionary
 */
static void
print_dictionary () 
{
	
	cout << "Dictionary: " << endl;
	int i = 0;
	for (auto &elem : dict) {
		bitset<32> bits(elem);
		cout << "Inst: " << bits << " index: " << i++ << endl;
	}

}

/*
 * This function prints the instructions 
 * in the original text file
 */
static void
print_uncompressed_inst () 
{
	
	cout << "Original text: " << endl;
	for (auto &elem : instruction) {
	
		bitset<32> bits(elem);
		cout << bits << endl;
	}

}

/*
 * Forming the dictionary and storing the 
 * instructions from the text file into 
 * instruction vector
 */
int
inst_and_dict () 
{

	ifstream fp;
	string line;
	int order = 0;

	/*
	 * Opening the text file which has 
	 * the instrcutions that needs to be compresses
	 */
	fp.open ("original.txt");

	/*
	 * Error checking for opening of file
	 */
	if (!fp.is_open()) {
		
		cout << "Unable to open file" <<endl;
		return -1;
	}

	while (getline (fp, line)) {

		int i 		= INST_BITS - 1;
		uint32_t inst 	= 0;
	
		/*
		 * The instructions in the original.txt 
		 * should be 32 bits in length 
		 */
		if (line.length() != INST_BITS) {
			
			cout << "Only 32 bits instruction are allowed" << endl;
			return -1;
		}

		/*
		 * storing the instructions of the 
		 * original.txt file into the instruction vector
		 */
		instruction.push_back(stol(line, nullptr, 2));
		
		/*
		 * Dictionary forming
		 */

		/*
		 * Reading character by character of the line 
		 * to convert the string input of line into uint32_t
		 */
		for (char &c : line) {
			
			if(c == '1') {
				
				inst = inst | (1 << i);
			}
			i--;
		}

		/*
		 * If the instruction is no present in the 
		 * unordered map of inst_freq than create a new entry 
		 * in the map and store the insertion order
		 * i.e the arrival index in the sequence of instruction 
		 * and also the set the count of instruction to 1
		 */
		/*
		 * If the instruction is already present in 
		 * the unordered map of inst_freq than just increase the 
		 * count of the instruction
		 */
		if (inst_freq.find(inst) == inst_freq.end()) {
			
			inst_freq.insert(make_pair(inst, freq_data(1, order)));
			order++;

		} else {
			
			freq_data data = inst_freq.at(inst);
			data.count++;
			freq_data new_data = freq_data(data.count, data.order);
			inst_freq.erase(inst);
			inst_freq.insert(make_pair(inst, freq_data(data.count, data.order)));

		}

	}

	/*
	 * Creting a vector elems to sort the unordered map freq_data and store it
	 * in the vector elems and than storing it in the global dictioanry 
	 */
	vector <pair <uint32_t, freq_data>> elems (inst_freq.begin(), inst_freq.end());

	/*
	 * Sorting the vector elems using the compartor function
	 * on the basis of count and oder of arrival
	 */
	sort(elems.begin(), elems.end(), cmp);

	/*
	 * Erasing any element after the first 16 elements 
	 * of the vector elems since the dictionary size is 16
	 */
	elems.erase (elems.begin() + DICT_SZ, elems.end());
	
	/*
	 * Inserting the 16 most repeated instruction 
	 * in the dictionary vector
	 */
	for (int i = 0; i < DICT_SZ; i++) {

		dict.push_back(elems[i].first);
	}

#if 0	
	print_dictionary ();
	print_uncompressed_inst();
#endif

	fp.close();

	return 0;

}

int
main ()
{

	int inst_dict; 	
	
	inst_dict = inst_and_dict ();
	if (inst_dict == -1)
		return -1;

	return 0;

}
