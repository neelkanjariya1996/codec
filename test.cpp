#include <bits/stdc++.h>
#include <string>

using namespace std;

int 
main () 
{
	bitset<32> bset1;
	bitset<32> bset2(string("1111"));
	bitset<32> bset3(string("11"));
	bitset<32> bset4(string("1"));

	cout << "original 0's:" << bset1 << endl;
	cout << "Four one's left shift by 27:" << (bset2 << 27) << endl;
	cout << "Four one's left shift by 28:" << (bset2 << 28) << endl;
	cout << "Two one's left shift by 29:" << (bset3 << 29) << endl;
	cout << "Two one's left shift by 30:" << (bset3 << 30) << endl;
	cout << "oneone's left shift by 30:" << (bset4 << 30) << endl;
	cout << "one one's left shift by 31:" << (bset4 << 31) << endl;

	return 0;
}
