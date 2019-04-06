#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <bits/stdc++.h>

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
	for (auto &elem : dict) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}

	comp_inst_len = comp_inst.length();
	//cout << "comp_inst length: " << comp_inst.length() << endl;
	//cout << "location outside while: " << loc << "\n" << endl;
	while (loc < comp_inst_len) {
		
//		cout << "iteration: " << i << "\n" << endl;
		i++;
		for_cmp = comp_inst.substr(loc, 3);
		loc = loc + 3;
//		cout << "format instruction: " << for_cmp << endl;
//		cout << "location inside while: " << loc << endl;
		
		if (for_cmp.compare("000") == 0) {
			cout << "inside no compression: ";
			//no compression
			decomp_inst = comp_inst.substr(loc, 32);
			loc = loc + 32;
//			cout << "Location: " << loc << endl;
			decomp_inst_l = stol(decomp_inst, nullptr, 2);
			decomp.push_back (decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("001") == 0) {
			cout <<"Inside Run Lenght Encoding: ";
//			cout << "iteration: " << i << "\n" << endl;
			//Run Lenght Encoding
			decomp_inst = comp_inst.substr(loc, 3);
			loc = loc + 3;
//			cout << "Location: " << loc << endl;
			rle = stoi(decomp_inst);
//			cout << "RLE: " << rle << endl;
			decomp_size = decomp.size();
			decomp_inst_l = decomp[decomp_size];
			int base = 1;
			int rle_dec = 0;
			for (int i = (decomp_inst.length() - 1); i >= 0; i--) {
				if (decomp_inst[i] == '1')
					rle_dec += base;
				base = base * 2;
			}
			rle_dec = rle_dec + 1;
			while (rle_dec) {
				rle_dec--;
				decomp.push_back(decomp_inst_l);
			}
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("010") == 0) {
			cout << "inside Bitmask Based: ";
			//Bitmask Based Compression
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			bitmask = stol(comp_inst.substr(loc, 4), nullptr, 2);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] |
					(bitmask << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("011") == 0) {
			cout << "inside 1 Bit Mismatch: ";
			//1 Bit Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | 
					(1 << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("100") == 0) {
			cout << "inside 2 Bit Consecutive Mismatch: ";
			//2 Bit Consecutive Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] |
					(1 << mismatch_loc_1) |
					(1 << (mismatch_loc_1 - 1)));
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("101") == 0) {
			cout << "inside 4 Bit Consecutive compression: ";
			//4 Bit Consecutive Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | 
					(1 << mismatch_loc_1) |
					(1 << (mismatch_loc_1 - 1)) |
					(1 << (mismatch_loc_1 - 2)) |
					(1 << (mismatch_loc_1 - 3)) );
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("110") == 0) {
			cout << "inside 2 Bit anywhere: ";
			//2 Bit Anywhere Mismatch
			mismatch_loc_1 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			mismatch_loc_2 = stoi(comp_inst.substr(loc, 5));
			loc = loc + 5;
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			decomp_inst_l = (dict[dict_ind] | (1 << mismatch_loc_1) | (1 << mismatch_loc_1));
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		} else if (for_cmp.compare("111") == 0) {
			cout << "inside Direct Mapping: ";
			//Direct Matching
//			cout << "Location: " << loc << endl;
			decomp_inst = comp_inst.substr(loc, 4);
//			cout << "Decompressed Instruction: " << decomp_inst << endl;
			loc = loc + 4;
//			cout << "Location: " << loc << endl;
			dict_ind = stoi(decomp_inst);
			//decomp_inst_l = stol(dict[dict_ind], nullptr, 2);
			decomp_inst_l = dict[dict_ind];
			decomp.push_back(decomp_inst_l);
			cout << "decomp: " << decomp_inst_l << endl;
		}
	}

	cout << "Decompressed Output: \n" << endl;
	for (auto &elem : decomp) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}

		fp.close();

	return 0;

}
