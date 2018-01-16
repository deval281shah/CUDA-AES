/*********************************************************************/
/*                       INCLUDES AND DEFINES                        */
/*********************************************************************/

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <omp.h>

#include "Helper.hpp"
#include "Mode.hpp"
#include "AES.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;

/*********************************************************************/
/*                     COUNTER MODE FUNCTIONS                        */
/*********************************************************************/

// Increment Counter TODO!
ByteArray increment_counter(const ByteArray &start_counter,
							const unsigned int &round)
{
  int i;
  ByteArray new_counter = start_counter;
  
  for (i = new_counter.size() - 1; i >= 0; --i)
    {
      
      if (new_counter[i] != 0xFF) {	
	++new_counter[i];
	return new_counter;
	
      } else {	
	new_counter[i] = 0x00;
	
      }
      
    }
  
  return new_counter;
}

// Generate Counters for all Rounds
void generate_counters(vector<ByteArray> &ctrs, const ByteArray &IV)
{
	ByteArray start_counter(KEY_BLOCK - IV.size(), 0x00);
	ByteArray ctr_i(KEY_BLOCK - IV.size(), 0x00);
	ByteArray res(KEY_BLOCK, 0x00);
	size_t i = 0;
	
	//	#pragma omp parallel private(i, res, ctr_i) shared(ctrs, start_counter, IV) num_threads(2)
	{
	  //#pragma omp for 
		for (i = 0; i < ctrs.size(); ++i)
		{	       

		  res = IV;
		  
			if (i > 0)
			{
				ctr_i = increment_counter(start_counter, i);
			} 

			res.insert(res.end(), ctr_i.begin(), ctr_i.end());
			ctrs[i] = res;
		}
	}
}

// Execute the Counter Mode for all Message Blocks
const vector<ByteArray> counter_mode(const vector<ByteArray> &messages,
									const ByteArray &key,
									const ByteArray &IV)
{
	AES aes(key);
	vector<ByteArray> encrypted_messages(messages.size(), vector<unsigned char>(KEY_BLOCK, 0x00));
	vector<ByteArray> ctrs(messages.size(), vector<unsigned char>(KEY_BLOCK, 0x00));
	generate_counters(ctrs, IV);
	size_t i = 0;

	#pragma omp parallel private(i) shared(aes, encrypted_messages, ctrs, messages, key) num_threads(2)
	{
#pragma omp for 
		for (i = 0; i < messages.size(); ++i)
		{
			encrypted_messages[i] = XOR(aes.encrypt(ctrs[i]), messages[i]);
		}
	       
	}

	return encrypted_messages;
}

// Execute the Inverse Counter Mode for all Decrypted Message Blocks
const vector<ByteArray> counter_mode_inverse(const vector<ByteArray> &encrypted_messages,
											const ByteArray &key,
											const ByteArray &IV)
{
	AES aes(key);
	vector<ByteArray> decrypted_messages(encrypted_messages.size(), vector<unsigned char>(KEY_BLOCK, 0x00));
	vector<ByteArray> ctrs(encrypted_messages.size(), vector<unsigned char>(KEY_BLOCK, 0x00));
	generate_counters(ctrs, IV);
	size_t i = 0;

	#pragma omp parallel private(i) shared(aes, decrypted_messages, ctrs, encrypted_messages, key) num_threads(4)
	{
#pragma omp for 
		for (i = 0; i < encrypted_messages.size(); ++i)
		{
		  decrypted_messages[i] = XOR(aes.encrypt(ctrs[i]), encrypted_messages[i]);
		}
	}

	return decrypted_messages;
}
