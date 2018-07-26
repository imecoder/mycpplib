#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>
using namespace std ;

namespace rui {
	typedef unsigned char byte;

	class crypto
	{
	public:
		crypto(const int ixor = 28 ) : mi_xor(ixor) {}

		bool encode(const vector<char>& v_in, vector<char>& v_out, const int i_num = 7) const;
		bool decode(const vector<char>& v_in, vector<char>& v_out, const int i_num = 1) const;

		bool encode_to_string(const vector<char>& v_in, string& s_out, const int i_num = 7) const;
		bool decode_from_string(const string& s_in, vector<char>& v_out, const int i_num = 1) const;

		string& hex_to_string(string& s_data, const vector<char>& v_data) const;
		void string_to_hex(vector<char>& v_data, const string& s_data) const;

	public:
		const int mi_xor;
	};
}
#endif

