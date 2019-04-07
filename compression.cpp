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

#define INST_BITS 	32
#define DICT_SZ		16

class freq_data 
{

	public:
	int count;
	int order;

	freq_data (int count1, int order1) {
		count = count1;
		order = order1;
	}

};

unordered_map<uint32_t, freq_data> inst_freq;
//vector <pair <uint32_t, freq_data>> dict (inst_freq.begin(), inst_freq.end());
vector <uint32_t> dict;
vector <uint32_t> instruction;

int
inst_and_dict () 
{

	ifstream fp;
	string line;
	int order = 0;

	fp.open ("original.txt");

	if (!fp.is_open()) {
		cout << "Unable to open file" <<endl;
		return -1;
	}

	while (getline (fp, line)) {

		int i 		= INST_BITS - 1;
		uint32_t inst 	= 0;

		if (line.length() != INST_BITS) {
			cout << "Only 32 bits instruction are allowed" << endl;
			return -1;
		}

		instruction.push_back(stol(line, nullptr, 2));
		
		for (char &c : line) {
			
			if(c == '1') {
				inst = inst | (1 << i);
			}
			i--;
		}

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

#if 0
	unordered_map <uint32_t, freq_data> :: iterator p;
	for (p = inst_freq.begin(); p != inst_freq.end(); p++) {
		bitset<32> bits(p->first);
		freq_data data = p->second;
		cout << "inst: " << bits << " count: " << data.count << " order: " << data.order << endl;
	}

#endif

	typedef function <bool (pair <uint32_t, freq_data>, pair <uint32_t, freq_data>)> comparator;

	comparator cmp = [] (pair <uint32_t, freq_data> elem1, pair <uint32_t, freq_data> elem2) {
		freq_data d1 = elem1.second;
		freq_data d2 = elem2.second;

		if (d1.count == d2.count) {
			return d1.order < d2.order;
		}

		return d1.count > d2.count;
	};
	
	vector <pair <uint32_t, freq_data>> elems (inst_freq.begin(), inst_freq.end());
	sort(elems.begin(), elems.end(), cmp);
	elems.erase (elems.begin() + 16, elems.end());
	
	cout << "sorted order: " << endl;
	for (auto &elem : elems) {
		bitset<32> bits(elem.first);
		cout << "key: " << bits << "   count: " << elem.second.count << endl;
	}

	for (int i = 0; i < DICT_SZ; i++) {
		dict.push_back(elems[i].first);
	}

	int i = 0;
	for (auto &elem : dict) {
		bitset<32> bits(elem);
		cout << "Inst: " << bits << " index: " << i++ << endl;
	}

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

	cout << "Original text: " << endl;
	for (auto &elem : instruction) {
		bitset<32> bits(elem);
		cout << bits << endl;
	}

	return 0;

}
