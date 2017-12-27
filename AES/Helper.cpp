/*********************************************************************/
/*                       INCLUDES AND DEFINES                        */
/*********************************************************************/

#include <vector>
#include <iostream>

#include "Helper.hpp"

using std::cout;
using std::endl;
using std::vector;

/*********************************************************************/
/*                          HELPER FUNCTIONS                         */
/*********************************************************************/

// Cout whole ByteArray
void print_byte_array(ByteArray &arr)
{
	for (size_t i = 0; i != arr.size(); ++i)
	{
		cout << std::hex << (int)arr[i] << "\t";
	}
	cout << endl << endl;
}

// Checks if two ByteArrays has same values
bool check_byte_arrays(ByteArray &arr1, ByteArray &arr2)
{
	if (arr1.size() != arr2.size())
		return false;

	for (size_t i = 0; i != arr1.size(); ++i)
	{
		if (arr1[i] != arr2[i])
			return false;
	}

	return true;
}

// Cout hex byte
void print_byte(const unsigned char &byte)
{
	cout << endl << "Byte: " << std::hex << (int)byte;
}