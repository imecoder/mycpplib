#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>

namespace rui {
	typedef unsigned char byte;

	class crypto
	{
	public:
		crypto(const int ixor = 28 ) : mi_xor(ixor) {}

		bool encode(const std::vector<char>& v_in,
					std::vector<char>& v_out,
					const int i_num = 7) const;
		bool decode(const std::vector<char>& v_in,
					std::vector<char>& v_out,
					const int i_num = 1) const;

		bool encode_to_string(const std::vector<char>& v_in,
							  std::string& s_out,
							  const int i_num = 7) const;
		bool decode_from_string(const std::string& s_in,
								std::vector<char>& v_out,
								const int i_num = 1) const;

		std::string& hex_to_string(std::string& s_data,
							  const std::vector<char>& v_data) const;
		void string_to_hex(std::vector<char>& v_data,
						   const std::string& s_data) const;

	public:
		const int mi_xor;
	};
}
#endif

