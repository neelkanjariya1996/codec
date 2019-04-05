#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>

using namespace std;

int
main () {

	ifstream fp;
	string line;
	bool is_dict = 0;
	string comp_inst;
	string dict_inst;
	int loc = 0;
	int comp_inst_len;
	string for_cmp;
	string decomp_inst;
	long decomp_inst_l = 0;
	int dict_ind = 0;
	int mismatch_loc_1 = 0;
	int mismatch_loc_2 = 0;
	long bitmask;
	int rle;
	int decomp_size;
	int i = 0;

	vector<uint32_t> dict;
	vector<uint32_t> decomp;

	fp.open("compressed.txt");
	

	while (getline (fp, line)) {
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
	
	cout << "comp_inst" << comp_inst << endl;
	cout << "dict_inst" << dict_inst << endl;

	for (auto i = dict.begin(); i != dict.end(); ++i)
		cout << *i << "\n";

	comp_inst_len = comp_inst.length();
	cout << "comp_inst length: " << comp_inst.length() << endl;
	cout << "location outside while: " << loc << "\n" << endl;
	while (loc <= comp_inst_len) {
		
		cout << "iteration: " << i << "\n" << endl;
		i++;
		for_cmp = comp_inst.substr(loc, 3);
		loc = loc + 3;
		cout << "format instruction: " << for_cmp << endl;
		cout << "location inside while: " << loc << endl;
		
		if (for_cmp.compare("000") == 0) {
			cout << "inside no compression: " << endl;
			//no compression
			decomp_inst = comp_inst.substr(loc, 32);
			loc = loc + 32;
			cout << "Location: " << loc << endl;
			decomp_inst_l = stol(decomp_inst, nullptr, 2);
			decomp.push_back (decomp_inst_l);
		} else if (for_cmp.compare("001") == 0) {
			cout <<"Inside Run Lenght Encoding: " << endl;
			//Run Lenght Encoding
			decomp_inst = comp_inst.substr(loc, 3);
			loc = loc + 3;
			cout << "Location: " << loc << endl;
			rle = stoi(decomp_inst);
			rle = rle + 1; // the rle index starts from 0 so adding 1
			decomp_size = decomp.size();
			decomp_inst_l = decomp[decomp_size];
			while (rle) {
				rle--;
				decomp.push_back(decomp_inst_l);
			}
		} else if (for_cmp.compare("010") == 0) {
			cout << "inside Bitmask Based: " << endl;
			//Bitmask Based Compression
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			bitmask = stol(comp_inst.substr(loc, 4), nullptr, 2);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] |
					(bitmask << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
		} else if (for_cmp.compare("011") == 0) {
			cout << "inside 1 Bit Mismatch: " << endl;
			//1 Bit Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | 
					(1 << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
		} else if (for_cmp.compare("100") == 0) {
			cout << "inside 2 Bit Consecutive Mismatch: " << endl;
			//2 Bit Consecutive Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] |
					(1 << mismatch_loc_1) |
					(1 << (mismatch_loc_1 - 1)));
			decomp.push_back(decomp_inst_l);
		} else if (for_cmp.compare("101") == 0) {
			cout << "inside 4 Bit Consecutive compression: " << endl;
			//4 Bit Consecutive Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | 
					(1 << mismatch_loc_1) |
					(1 << (mismatch_loc_1 - 1)) |
					(1 << (mismatch_loc_1 - 2)) |
					(1 << (mismatch_loc_1 - 3)) );
			decomp.push_back(decomp_inst_l);
		} else if (for_cmp.compare("110") == 0) {
			cout << "inside 2 Bit anywhere: " << endl;
			//2 Bit Anywhere Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			mismatch_loc_2 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | (1 << mismatch_loc_1) | (1 << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
		} else if (for_cmp.compare("111") == 0) {
			cout << "inside Direct Mapping: " << endl;
			//Direct Matching
			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			cout << "Decompressed Instruction: " << decomp_inst << endl;
			loc = loc + 4;
			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			//decomp_inst_l = stol(dict[dict_ind], nullptr, 2);
			decomp_inst_l = dict[dict_ind];
			decomp.push_back(decomp_inst_l);
		}
	}

	cout << "Decompressed Output: \n" << endl;
	for (auto i = decomp.begin(); i != decomp.end(); ++i)
		cout << *i << "\n";

		fp.close();

	return 0;

}
