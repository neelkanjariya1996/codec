/* On my honor, I have neihter given nor received unauthorized aid on this assignment */

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <set>
#include <algorithm>
#include <functional>
#include <vector>
#include <assert.h>
#include <string>
#include <bits/stdc++.h> 
using namespace std;

#define INST_BITS 	32		// Instructions are 32 bits long
#define DICT_SIZE 	16		// Dictionary size is 16
#define RLE_MAX   	8		// Max number of instruction allowed to encode using RLE scheme is 8
#define TYPE_SZ   	3		// The 8 encoding schemes can be represented using 3 bits 
#define RLE_SZ   	3		// This 3 bits represent the number of instructions encoded using RLE scheme (MAx: 8)
#define MASK_SZ   	4		// The size of the bitmask used in bitmask based encoding scheme is 4 bits
#define INDEX_SZ  	4		// This represents the dicitionary instruction at each dictionary index using 4 bits 
#define LOC_SZ    	5		// A location in the 32 bit instruction can be represented using 5 bits

/***********************************
 *        Global storage           *
 ***********************************/

/*
 * Vector of 32 bit instructions
 */
vector <uint32_t> inst_vec;

/*
 * Dictonary of instructions
 * since only 16 inst are present, it is okay to keep it as a vector
 */
vector <uint32_t> dict;

/*
 * Compressed instruction string
 */
string compressed_inst_str;


/***********************************
 *        Helper classes           *
 ***********************************/

/*
 * A storage class to keep data for calculating
 * number of occurances of an instruction
 */
class FreqData {
  public:
    int count;	//num of occurences
    int seq;	//insertion order

    FreqData (int count1, int seq1) {
      count = count1;
      seq = seq1;
    }

    ~FreqData() {
    }
};


/**********************************************
 * Functions related to instruction dictonary *
 **********************************************/

/*
 * helper function to print compressed inst string
 */
void
print_compressed_inst ()
{
  cout << "Compressed instructions as a bit stream:\n"; 
  cout << compressed_inst_str << "\n";
}

/*
 * helper function to print all the instructions
 */
void
print_inst_vec ()
{
  int i = 0;

  for (auto &elem : inst_vec) {
    
    bitset<32> bits(elem);
    cout << "inst: " << bits << "\n";
  }
}

/*
 * helper function to print the dictonary
 */
void
print_dict ()
{
  int i = 0;

  for (auto &elem : dict) {
    
    bitset<32> bits(elem);
    cout << "inst: " << bits << " decimal: "<< elem << " index: " << i++ << "\n";
  }
}

/*
 *  Given an instruction,
 *  get its corresponding index in dict
 *
 *  @input inst the 32 bit instruction
 *  @input index the index to be filled
 *
 *  @return bool true  - if inst present in dict
 *               false - otherwise
 */
bool
get_index_from_inst (uint32_t inst, uint8_t *index)
{
  int i = 0;

  if (!index) {
    return false;
  }

  for (i = 0; i < DICT_SIZE; i++ ) {

    if (inst == dict[i]) {
      
      *index = i;
      return true;
    }
  }
  
  return false;
}

/*
 * Given a dict index,
 * get the corresponding inst
 *
 * @input index a dict index
 * 
 * @return 32 bit instruction from the dict
 */
uint32_t
get_inst_from_index (uint8_t index)
{
  return (dict[index]);
}

/*
 * parse_input_for_compression
 * function to create a dictonary of instructions
 *
 * @input input_file input file name
 * @return 1 is success, -1 otherwise
 */
int
parse_input_for_compression (const char *input_file)
{
  int seq = 0;
  string line;
  unordered_map<uint32_t, FreqData> inst_freq;

  ifstream myfile (input_file);

  if (!myfile.is_open()) {

    cout << "Unable to open input file\n";
    return -1;
  }
  
  while ( getline (myfile, line) ) {
    
    int i = INST_BITS - 1;
    uint32_t inst = 0;
   
    /*
     * only 32 bit instructions are supported
     */
    if (line.length() != INST_BITS) {
      
      cout << "Error: only 32 bit instructions are permitted\n";
      myfile.close();
      return -1;
    }

    /*
     * each inst is stored as a uint32_t in
     * our database
     */
    for (char &c : line) {

      if (c == '1') {
        inst |= 1 << i;
      }
      i--;
    }

    /*
     * add the inst to inst_list
     */
    inst_vec.push_back(inst);

    /*
     * if the instruction is not present in the map
     * create one and add the insertion sequence
     * if it already present update the count
     */
    if (inst_freq.find(inst) == inst_freq.end()) {
      
      inst_freq.insert(make_pair(inst, FreqData(1, seq)));
      seq++;
    } else {
    
      FreqData data = inst_freq.at(inst);
      data.count++;
      FreqData new_data = FreqData(data.count, data.seq);
      inst_freq.erase(inst);
      inst_freq.insert(make_pair(inst, FreqData(data.count, data.seq)));
    }
  }

  /*
   * sort the freq map
   * the inst which appeared most num of times is kept at 0th index
   * if there is a tie, the inst whuch came first, is kept at lower index
   * We will only keep the first 16 inst as dictionary size is 16
   */
  typedef function <bool (pair <uint32_t, FreqData>, pair <uint32_t, FreqData>)> comparator;

  comparator cmp = [] (pair <uint32_t, FreqData> elem1, pair <uint32_t, FreqData> elem2) {
    
    FreqData d1 = elem1.second;
    FreqData d2 = elem2.second;
    
    if (d1.count == d2.count) {
      return d1.seq < d2.seq;
    }

    return d1.count > d2.count;
  };

  vector <pair <uint32_t, FreqData>> elems (inst_freq.begin(), inst_freq.end());
  sort(elems.begin(), elems.end(), cmp);

  /*
   * create the dictonary
   */
  for (int i = 0; i < DICT_SIZE; i++) { 
    dict.push_back(elems[i].first);
  }
 
  /*
   * close the file
   */
  myfile.close();
  return 1;
}

/*
 * create the dict from compressed input
 * for decoding.
 * Also, create the compressed instruction string
 *
 * @ip compressed_file compressed input
 * @return -1 if error
 *          0 if success
 */
int
create_decode_dict (const char *compressed_file)
{
  string line;
  bool is_line_for_dict = false;

  ifstream myfile (compressed_file);
  if (!myfile.is_open()) {
    cout << "Error: opening compressed file\n";
    return -1;
  }

  while ( getline (myfile, line) ) {
    
    if (line.compare("xxxx") == 0) {

      is_line_for_dict = true;
    } else {

      if (is_line_for_dict) {
        
        int i = INST_BITS - 1;
        uint32_t inst = 0;
        
        /*
         * each inst is stored as a uint32_t in
         * our database
         */
        for (char &c : line) {

          if (c == '1') {
            inst |= 1 << i;
          }
          i--;
        }

        dict.push_back(inst);
      } else {
        
        compressed_inst_str.append(line);
      }
    }
  }

  /*
   * close the file
   */
  myfile.close();
  
  return 0;
}

/*************************
 *    Decoding           *
 *************************/

/*
 * Get the RLE length
 * Example:
 *
 * compressed_inst_str = "001111"
 * return = "111"
 *
 * @ip: cp current pointer in the compressed inst string
 *
 * @return: string for RLE length
 */
string
get_rle (size_t *cp)
{
  string rle;
  size_t len = RLE_SZ;

  rle = compressed_inst_str.substr(*cp, len);
  *cp += len;

  return rle;
}

/*
 * Get the type of instruction
 * Example:
 * compressed_inst_str = "1110000"
 * return = "111"
 */
string
get_type (size_t *cp)
{
  string type;
  size_t len = TYPE_SZ;

  type = compressed_inst_str.substr(*cp, len);
  *cp += len;

  return type;
}

/*
 * Get the ful instruction from current pointer in the compressed inst string
 * Example:
 *
 * compressed inst string: "000 32-bit-inst"
 * return: "32-bit-inst"
 */
string
get_full_inst (size_t *cp)
{
  string inst;
  size_t len = INST_BITS;

  inst = compressed_inst_str.substr(*cp, len);
  *cp += len;
  
  return inst;
}

/*
 * Get the location from the current pointer in compressed inst string
 * Example:
 *
 * compressed inst string: "010 10000 1101 0000"
 * return = "10000"
 *
 */
string
get_location (size_t *cp)
{
  string loc;
  size_t len = LOC_SZ;

  loc = compressed_inst_str.substr(*cp, len);
  *cp += len;
  
  return loc;
}

/*
 * Get the string representation
 * of 4-bit mask value from compressed
 * instruction string starting at loc = cp
 */
string
get_mask (size_t *cp)
{
  string mask;
  size_t len = MASK_SZ;

  mask = compressed_inst_str.substr(*cp, len);
  *cp += len;
  
  return mask;
}

/*
 * Get the string representation
 * of 4-bit index value from compressed
 * instruction string starting at loc = cp
 */
string
get_index (size_t *cp)
{
  string index;
  size_t len = INDEX_SZ;

  index = compressed_inst_str.substr(*cp, len);
  *cp += len;
  
  return index;
}

/*
 * Master Decode function
 */
void
decode ()
{
  size_t cp 		= 0;
  uint32_t length 	= 0;
  uint32_t inst 	= 0;
  uint32_t prev_inst 	= 0;

  length = compressed_inst_str.length();

  while (cp < length) {
    
    /*
     * get the type of encoding
     */
    string type;
    type = get_type(&cp);
    if (type.length() < 3) {
      break;
    }
    
    /*
     * checking if the first instruction to be 
     * decoded has a RLE format or not
     * Since, RLE requires atleast 1 instruction before it,
     * the first instruction cannot be RLE
     */
    
    if (cp == 3 && type == "001") {
      cout << "First instruction to be decoded cannot have a RLE format" << endl;
      break;
    }

    /*
     * no encoding
     */
    if (type.compare("000") == 0) {
	
      /*
       * get the full instruction
       */
      string inst_str = get_full_inst(&cp);

      /*
       * handle trailing zeros
       */
      if (inst_str.length() != INST_BITS) {
        break;
      }
      
      bitset<32> bits(inst_str);
      
      inst = bits.to_ulong();
      inst_vec.push_back(inst);
    }

    /*
     * RLE
     */
    if (type.compare("001") == 0) {
      
      string repeat_str = get_rle(&cp);
      int n = stoi(repeat_str, nullptr, 2);

      /*
       * n implies n + 1 repeatations
       */
      for (int i = 0; i <= n; i++) {
        inst_vec.push_back(prev_inst);
      }
    }
    
    /*
     * bit-mask
     */
    if (type.compare("010") == 0) {
      
      /*
       * get the location
       */
      string loc_str = get_location(&cp);
      int loc = stoi(loc_str, nullptr, 2);

      /*
       * get the mask
       * left shift the mask 28 times (to bring it to MSB)
       * then right shift the mask loc times (to bring it to actual loc)
       */
      string mask_str = get_mask(&cp);
      uint32_t mask = (uint32_t)stoi(mask_str, nullptr, 2);
      mask = mask << 28;
      mask = mask >> loc;
     
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);

      /*
       * get the Dictonary instruction
       */
      inst = dict[index];

      /*
       * original instruction is calculated as
       * dict_inst ^ mask
       */
      inst = inst ^ mask;
      inst_vec.push_back(inst);
    }
    
    /*
     * 1-bit mismatch
     */
    if (type.compare("011") == 0) {
      
      /*
       * get the location
       * Here the mask is 1-bit
       */
      string loc_str = get_location(&cp);
      int loc = stoi(loc_str, nullptr, 2);
     
     /*
      * create the 1 bit mask
      */ 
      uint32_t mask = 1 << 31;
      mask = mask >> loc;
      
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);
      
      /*
       * get the Dictonary instruction
       */
      inst = dict[index];
      
      /*
       * original instruction is calculated as
       * dict_inst with bit at loc from MSB flipped
       */
      inst = inst ^ mask;
      inst_vec.push_back(inst);
    }
    
    /*
     * 2-bit consecutive mismatch
     */
    if (type.compare("100") == 0) {
      
      /*
       * get the location
       * Here the mask is 2-bit
       */
      string loc_str = get_location(&cp);
      int loc = stoi(loc_str, nullptr, 2);
      
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);
      
      /*
       * create the mask for 2 bit consecutive mismatch
       */
      uint32_t mask = 0x03 << 30; //11000000 00000000 00000000 00000000
      mask = mask >> loc;
      
      /*
       * get the Dictonary instruction
       */
      inst = dict[index];
      
      /*
       * original instruction is calculated as
       * dict_inst with bit at loc from MSB flipped
       */
      inst = inst ^ mask;
      inst_vec.push_back(inst);
    }
    
    /*
     * 4-bit consecutive mismatch
     */
    if (type.compare("101") == 0) {
      
      /*
       * get the location
       * Here the mask is 4-bit
       */
      string loc_str = get_location(&cp);
      int loc = stoi(loc_str, nullptr, 2);
      
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);
      
      /*
       * create the mask for 4 bit consecutive mismatch
       */
      uint32_t mask = 0x0F << 28; //11110000 00000000 00000000 00000000
      mask = mask >> loc;
      
      /*
       * get the Dictonary instruction
       */
      inst = dict[index];
      
      /*
       * original instruction is calculated as
       * dict_inst with bit at loc from MSB flipped
       */
      inst = inst ^ mask;
      inst_vec.push_back(inst);
    }
    
    /*
     * 2-bit anywhere
     */
    if (type.compare("110") == 0) {
      
      /*
       * get the first location
       */
      string loc_str1 = get_location(&cp);
      int loc1 = stoi(loc_str1, nullptr, 2);
      
      /*
       * get the second location
       */
      string loc_str2 = get_location(&cp);
      int loc2 = stoi(loc_str2, nullptr, 2);
      
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);
      
      /*
       * get the Dictonary instruction
       */
      inst = dict[index];
      
      /*
       * original instruction is calculated as
       * dict_inst with bit at loc1 and loc2 from MSB flipped
       */
      uint32_t mask;

      /* flip the bit at first loc */
      mask = 1 << 31;
      mask = mask >> loc1;
      inst = inst ^ mask;

      /* flip the bit at 2nd loc */
      mask = 1 << 31;
      mask = mask >> loc2;
      inst = inst ^ mask;

      inst_vec.push_back(inst);
    }
    
    /*
     * direct
     */
    if (type.compare("111") == 0) {
      
      /*
       * get the index
       */
      string index_str = get_index(&cp);
      int index = stoi(index_str, nullptr, 2);
      
      /*
       * get the Dictonary instruction
       */
      inst = dict[index];

      inst_vec.push_back(inst);
    }

    prev_inst = inst;
  }

  /*
   * write the output to file
   */
  ofstream dout_file;
  dout_file.open ("dout.txt", ios::trunc);
  for (auto &elem : inst_vec) {
    
    bitset<32> bits(elem);
    dout_file << bits;
    dout_file << "\n";
  }
  dout_file.close();

  return;
}

/**************************
 *  Bit Magic             *
 **************************/

/*
 * count the number of set bits
 * example:
 *  ip: 1010
 *  output: 2
 */
static int
count_set_bits (uint32_t x)
{
  int count = 0;

  while (x) {
    x = x & (x - 1);
    count++;
  }

  return (count);
}

/*
 * return the hamming distance between two numbers
 * example:
 *  a = 1010
 *  b = 0101
 *  output: 4
 */
static int
hamming_distance (uint32_t a, uint32_t b)
{
  return (count_set_bits(a ^ b));
}

/*
 * indexing starts with 0 from MSB
 * exammple:
 *  ip: 1010
 *  output: 0 
 */
static int
get_msb_loc (uint32_t x)
{
  return ((INST_BITS - 1) - (int)(log2(x)));
}

/*
 * indexing starts at 0 from MSB
 * example:
 *  ip: 1010
 *  output: 3 
 */
static int
get_lsb_loc (uint32_t x)
{
  return ((INST_BITS - 1) - (ffsl((long int) x) - 1));
}

/*
 * return the length of longest consecutive set bits
 * ip: 00111100
 * output: 4
 */
static int
len_longest_consecutive_ones (uint32_t x)
{
  int count = 0;

  while (x) {
    x = (x & (x << 1)); 
    count++;
  }

  return count;
}

/*
 * check if two instruction differ by 1 bit
 * @ip a first instruction
 * @ip b second instruction
 * @ip loc location of the mismatch bit
 *
 * @return: true - if there is a one bit mismatch
 *          false - otherwise
 */
static bool
is_one_bit_mismmatch (uint32_t a, uint32_t b, uint32_t *loc)
{
  assert(loc);

  *loc = 0;

  if (hamming_distance(a, b) != 1) {
    return (false);
  }

  *loc = get_msb_loc(a ^ b);
  return (true);
}

/*
 * check if two instruction differ at 2 consecutive locations
 * @ip a first instruction
 * @ip b second instruction
 * @ip loc location of the first mismatch bit from MSB
 *
 * @return: true  - if there is a two bit consecutive mismatch
 *          false - otherwise
 */
static bool
is_two_bit_consecutive_mismatch (uint32_t a, uint32_t b, uint32_t *loc)
{
  uint32_t x = 0;

  assert(loc);

  *loc = 0;
  x = a ^ b;
  
  if (hamming_distance(a, b) != 2) {
    return (false);
  }

  if (len_longest_consecutive_ones(x) != 2) {
    return (false);
  }

  *loc = get_msb_loc(x);
  return (true);
}

/*
 * check if two instruction differ at 4 consecutive locations
 * @ip a first instruction
 * @ip b second instruction
 * @ip loc location of the first mismatch bit fromm MSB
 *
 * @return: true  - if there is a four bit consecutive mismatch
 *          false - otherwise
 */
static bool
is_four_bit_consecutive_mismatch (uint32_t a, uint32_t b, uint32_t *loc)
{
  uint32_t x = 0;

  assert(loc);
  x = a ^ b;

  if (hamming_distance(a, b) != 4) {
    return (false);
  }

  if (len_longest_consecutive_ones(x) != 4) {
    return (false);
  }

  *loc = get_msb_loc(x);
  return (true);
}

/*
 * check if two instruction differ at 2 locations anywhere
 * @ip a first instruction
 * @ip b second instruction
 * @ip loc1 1st mismatch location
 * @ip loc2 2nd mismatch location
 *
 * @return: true - if there is a 2 bit mismatch anywhere
 *          false - otherwise
 */
static bool
is_two_bit_anywhere_mismatch (uint32_t a, uint32_t b,
                              uint32_t *loc1, uint32_t *loc2)
{
  uint32_t x = 0;

  assert(loc1);
  assert(loc2);
  x = a ^ b;

  if (hamming_distance(a, b) != 2) {
    return (false);
  }

  *loc1 = get_msb_loc(x);
  *loc2 = get_lsb_loc(x);
  return (true);
}

/*
 * check if the instruction can be encoded using bitmask encode
 * @ip a first instruction
 * @ip b second instruction
 * @ip loc location
 * @ip mask bitmask to be applied
 *
 * @return true  - if the inst can be encoded using bitmask encode
 *         false - otherwise
 */
static bool
is_bitmask_encode (uint32_t a, uint32_t b, uint32_t *loc, uint32_t *mask)
{
  int pos1 = 0;
  int pos2 = 0;

  assert(loc);
  assert(mask);

  *mask = a ^ b;

  if (hamming_distance(a, b) > 4) {
    return (false);
  }

  *loc = pos1 = get_msb_loc(*mask);
  pos2 = get_lsb_loc(*mask);

  /*
   * mask has to be a 4 bit consecutive mask
   */
  if ((pos2 - pos1) > 3) {
    return (false);
  }
  
  *mask = *mask << pos1;
  *mask = *mask >> (INST_BITS - 1 - 3);
  return (true);
}


/***********************************************
 *          Encoding schemes                   *
 ***********************************************/

/*
 * run length encoding
 * output-format:
 * | 001 | RLE (3bits) | 
 */
string
run_length_encode (int n)
{
  bitset<6> encoded_inst;
  bitset<3> n_bits(n - 1); // n = 8 is encoded as 111

  encoded_inst[5] = 0;
  encoded_inst[4] = 0;
  encoded_inst[3] = 1;
  encoded_inst[2] = n_bits[2];
  encoded_inst[1] = n_bits[1];
  encoded_inst[0] = n_bits[0];

  return encoded_inst.to_string();
}

/*
 * Bitmask based encoding
 * output-format:
 * | 010 | start-location(5 bits) | bitmask(4 bits) | index(4 bits) |
 * 
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by bitmask encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
bitmask_encode (uint32_t inst, bool *encoded)
{
  int index 	= 0;
  uint32_t loc 	= 0;
  bool found 	= false;
  uint32_t mask = 0;
  bitset<16> encoded_inst;

  assert(encoded);
  *encoded = false;

  for (auto &elem : dict) {
    
    if (is_bitmask_encode(inst, elem, &loc, &mask)) {
      found = true;
      break;
    }
    index++;
  }

  if (found) {
    bitset<4> index_bits(index);
    bitset<5> loc_bits(loc);
    bitset<4> mask_bits(mask);

    encoded_inst[15] 	= 0;
    encoded_inst[14] 	= 1;
    encoded_inst[13] 	= 0;
    encoded_inst[12] 	= loc_bits[4];
    encoded_inst[11] 	= loc_bits[3];
    encoded_inst[10] 	= loc_bits[2];
    encoded_inst[9] 	= loc_bits[1];
    encoded_inst[8] 	= loc_bits[0];
    encoded_inst[7] 	= mask_bits[3];
    encoded_inst[6] 	= mask_bits[2];
    encoded_inst[5] 	= mask_bits[1];
    encoded_inst[4] 	= mask_bits[0];
    encoded_inst[3] 	= index_bits[3];
    encoded_inst[2] 	= index_bits[2];
    encoded_inst[1] 	= index_bits[1];
    encoded_inst[0] 	= index_bits[0];
    
    *encoded = true;
  }

  return encoded_inst.to_string();
}

/*
 * 1-bit mismatch
 * output-format:
 * | 011 | mismatch location (5 bits) | index (4 bits)
 * 
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by 1-bit mismatch encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
one_bit_mismatch_encode (uint32_t inst, bool *encoded)
{
  int index 	= 0;      //dict index
  uint32_t loc 	= 0;   //location of first mismatch bit
  bool found 	= false;
  bitset<12> encoded_inst;

  assert(encoded);
  *encoded = false;

  for (auto &elem : dict) {
    
    if (is_one_bit_mismmatch(inst, elem, &loc)) {
      found = true;
      break;
    }
    index++;
  }

  if (found) {
    
    bitset<4> index_bits(index);
    bitset<5> loc_bits(loc);
    encoded_inst[11] = 0;
    encoded_inst[10] = 1;
    encoded_inst[9]  = 1;
    encoded_inst[8]  = loc_bits[4];
    encoded_inst[7]  = loc_bits[3];
    encoded_inst[6]  = loc_bits[2];
    encoded_inst[5]  = loc_bits[1];
    encoded_inst[4]  = loc_bits[0];
    encoded_inst[3]  = index_bits[3];
    encoded_inst[2]  = index_bits[2];
    encoded_inst[1]  = index_bits[1];
    encoded_inst[0]  = index_bits[0];
    
    *encoded = true;
  }

  return encoded_inst.to_string();
}

/*
 * 2-bit consecutive mismatch
 * output-format:
 * | 100 | start-location (5 bits) | index (4 bits)
 * 
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by 2-bit consecutive encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
two_bit_consecutive_mismatch_encode (uint32_t inst, bool *encoded)
{
  bitset<12> encoded_inst;
  int index 	= 0;
  uint32_t loc 	= 0;
  bool found 	= false;

  assert(encoded);

  *encoded = false;
  for (auto &elem : dict) {
    
    if (is_two_bit_consecutive_mismatch(inst, elem, &loc)) {
      found = true;
      break;
    }
    index++;
  }

  if (found) {
    
    bitset<4> index_bits(index);
    bitset<5> loc_bits(loc);
    encoded_inst[11] = 1;
    encoded_inst[10] = 0;
    encoded_inst[9]  = 0;
    encoded_inst[8]  = loc_bits[4];
    encoded_inst[7]  = loc_bits[3];
    encoded_inst[6]  = loc_bits[2];
    encoded_inst[5]  = loc_bits[1];
    encoded_inst[4]  = loc_bits[0];
    encoded_inst[3]  = index_bits[3];
    encoded_inst[2]  = index_bits[2];
    encoded_inst[1]  = index_bits[1];
    encoded_inst[0]  = index_bits[0];
    
    *encoded = true;
  }

  return encoded_inst.to_string();
}

/*
 * 4-bit consecutive mismatch
 * output-format:
 * | 101 | start-location (5 bits) | index (4 bits)
 * 
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by 4-b2-bit consecutive encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
four_bit_consecutive_mismatch_encode (uint32_t inst, bool *encoded)
{
  bitset<12> encoded_inst;
  int index 	= 0;
  uint32_t loc 	= 0;
  bool found 	= false;

  assert(encoded);

  *encoded = false;
  for (auto &elem : dict) {
    
    if (is_four_bit_consecutive_mismatch(inst, elem, &loc)) {
      found = true;
      break;
    }
    index++;
  }

  if (found) {
    
    bitset<4> index_bits(index);
    bitset<5> loc_bits(loc);
    encoded_inst[11] = 1;
    encoded_inst[10] = 0;
    encoded_inst[9]  = 1;
    encoded_inst[8]  = loc_bits[4];
    encoded_inst[7]  = loc_bits[3];
    encoded_inst[6]  = loc_bits[2];
    encoded_inst[5]  = loc_bits[1];
    encoded_inst[4]  = loc_bits[0];
    encoded_inst[3]  = index_bits[3];
    encoded_inst[2]  = index_bits[2];
    encoded_inst[1]  = index_bits[1];
    encoded_inst[0]  = index_bits[0];
    
    *encoded = true;
  }

  return encoded_inst.to_string();
}

/*
 * 2-bit anywhere mismatch
 * output-format:
 * | 110 | ML1 (5 bits) | ML2 (5 bits) | index (4 bits)
 * 
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by 2-bit anywhere encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
two_bit_anywhere_mismatch_encode (uint32_t inst, bool *encoded)
{
  bitset<17> encoded_inst;
  int index 	= 0;
  uint32_t loc1 = 0;
  uint32_t loc2 = 0;
  bool found 	= false;

  assert(encoded);
  
  *encoded = false;
  for (auto &elem : dict) {
    
    if (is_two_bit_anywhere_mismatch(inst, elem, &loc1, &loc2)) {
      found = true;
      break;
    }
    index++;
  }

  if (found) {
    
    bitset<5> loc1_bits(loc1);
    bitset<5> loc2_bits(loc2);
    bitset<4> index_bits(index);

    encoded_inst[16] = 1;
    encoded_inst[15] = 1;
    encoded_inst[14] = 0;
    encoded_inst[13] = loc1_bits[4];
    encoded_inst[12] = loc1_bits[3];
    encoded_inst[11] = loc1_bits[2];
    encoded_inst[10] = loc1_bits[1];
    encoded_inst[9]  = loc1_bits[0];
    encoded_inst[8]  = loc2_bits[4];
    encoded_inst[7]  = loc2_bits[3];
    encoded_inst[6]  = loc2_bits[2];
    encoded_inst[5]  = loc2_bits[1];
    encoded_inst[4]  = loc2_bits[0];
    encoded_inst[3]  = index_bits[3];
    encoded_inst[2]  = index_bits[2];
    encoded_inst[1]  = index_bits[1];
    encoded_inst[0]  = index_bits[0];

    *encoded = true;
  }

  return encoded_inst.to_string();
}

/*
 * direct matching
 * output format:
 * | 111 | index (4 bits)
 *
 * @input inst 32 bit instruction
 * @input encoded set to true if the inst can be encoded by direct encoding
 *               false otherwise
 *
 * @output: encoded instruction in bits
 */
string
direct_encode (uint32_t inst, bool *encoded)
{
  uint8_t index = 0;
  bitset<7> encoded_inst;

  if (!get_index_from_inst(inst, &index)) {

    *encoded = false;
    return encoded_inst.to_string();
  }

  bitset<4> bit_index(index);
  
  encoded_inst[6] = 1;
  encoded_inst[5] = 1;
  encoded_inst[4] = 1;
  encoded_inst[3] = bit_index[3];
  encoded_inst[2] = bit_index[2];
  encoded_inst[1] = bit_index[1];
  encoded_inst[0] = bit_index[0];
 
  *encoded = true;

  return encoded_inst.to_string();
}

/*
 * encode an instruction
 * We will try to encode the inst in following order
 * 1. RLE scheme (6-bits)
 * 2. Direct scheme (7-bits)
 * 3. 1-bit mismatch scheme (12-bits)
 * 4. 2-bit consecutive mismatch scheme (12-bits)
 * 5. 4-bit consecutive mismatch scheme (12-bits)
 * 6. bit-mask encoding scheme (16-bits)
 * 7. 2-bit anywhere mismatch scheme (17-bits)
 */
string
encode_inst (uint32_t inst)
{
  string encoded_inst;
  bool encoded = false;

  /*
   * try direct encoding
   */
  encoded_inst = direct_encode(inst, &encoded);
  if (encoded) {
    return (encoded_inst);
  }

  /*
   * try one bit mismatch
   */
  encoded_inst = one_bit_mismatch_encode(inst, &encoded);
  if (encoded) {
    return encoded_inst;
  }

  /*
   * try two bit consecutive mmismatch
   */
  encoded_inst = two_bit_consecutive_mismatch_encode(inst, &encoded);
  if (encoded) {
    return (encoded_inst);
  }

  /*
   * try 4 bit consecutive mismmatch
   */
  encoded_inst = four_bit_consecutive_mismatch_encode(inst, &encoded);
  if (encoded) {
    return (encoded_inst);
  }

  /*
   * try bitmask encoding
   */
  encoded_inst = bitmask_encode(inst, &encoded);
  if (encoded) {
    return (encoded_inst);
  }

  /*
   * try two bit anywhere
   */
  encoded_inst = two_bit_anywhere_mismatch_encode(inst, &encoded);
  if (encoded) {
    return (encoded_inst);
  }

  /*
   * none of the schemes worked, return full instruction
   */
  bitset<32> bits(inst);
  encoded_inst = string(3, '0').append(bits.to_string());
  
  return (encoded_inst);
}

/*
 * Function to encode all the instructions
 */
void
encode ()
{
  long long prev_inst 	= -1;
  int i 		= 0;
  int len 		= 0;
  int repeat_n 		= 0;
  bool reset 		= false;
  string encoded_inst;

  len = inst_vec.size();

  while (i < len) {
    
    uint32_t curr_inst = inst_vec[i];
    if (curr_inst == prev_inst) {
      repeat_n++;
    } else {

      /*
       * check if we have any pending RLE to encode
       */
      if (repeat_n) {
        
        encoded_inst = run_length_encode(repeat_n);
        compressed_inst_str += encoded_inst;
        repeat_n = 0;
      }
      
      /*
       * Now encode the current inst
       */
      encoded_inst = encode_inst(curr_inst);
      compressed_inst_str += encoded_inst;
    }

    if (repeat_n == RLE_MAX) {
      
      encoded_inst = run_length_encode(repeat_n);
      compressed_inst_str += encoded_inst;
      repeat_n = 0;
      prev_inst = -1;
    } else {
   
      prev_inst = curr_inst;
    }

    i++;
  }
  
  /*
   * check if we have any pending RLE to encode
   */
  if (repeat_n) {
    
    encoded_inst = run_length_encode(repeat_n);
    compressed_inst_str += encoded_inst;
  }

  /*
   * write the output to compressed.txt
   */
  ofstream myfile;
  myfile.open("cout.txt", ios::trunc);

  int n = compressed_inst_str.length();
  uint32_t cp = 0;
  string dest;
  while (n >= 0) {

    string out = compressed_inst_str.substr(cp, INST_BITS);
    int out_len = out.length();
    if (out_len < INST_BITS) {
       dest = out.append(string(INST_BITS - out_len , '0'));
    } else {
      dest = out;
    }
    
    myfile << dest;
    myfile << "\n";

    cp += INST_BITS;
    n -= INST_BITS;
  }

  /*
   * copy the dict
   */
  myfile << "xxxx\n";
  for (auto &elem : dict) {
    bitset<32> bits(elem);
    myfile << bits;
    myfile << "\n";
  }

  myfile.close();

  return;
}

/**************************
 * Driver function (main) *
 **************************/
int
main (int argc, char* argv[])
{
  string option;

  if (argc > 2) {
    
    cout << "Error: unexpected command line argument passed";
    return -1;
  }

  option = argv[1];
  if (option.compare("1") == 0) {
    
    /*
     * read the input file and
     * create a dictionary of instructions
     */
    if (!parse_input_for_compression("original.txt")) {
      
      cout << "Error: fail to create dictionary\n";
      return -1;
    }
    
    /*
     * encode all the instructions
     * and store the output in cout.txt
     */
    encode();
  } else if (option.compare("2") == 0) {
   
    /*
     * parse the compressed output
     * and create dict and compressed string
     */
    create_decode_dict("compressed.txt");

    /*
     * decode all the instructions and
     * store the output in dout.txt
     */
    decode();
  } else {
    
    cout << "Error: unexpected command line argument passed";
    return -1;
  }

  return 0;
}
