#include "crypto.h"

namespace rui {


bool crypto::encode(const std::vector<char>& v_in,
					std::vector<char>& v_out, const int i_num) const
{
	v_out.clear();
	if(v_in.empty())
		return true;

	const size_t size = v_in.size();

	// 右移位数因子对照表
	static const byte ARR[9] = {0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
	const int num = i_num % 8;  //0-7的整形

	if(num < 0 || num > 8)
		return false;

	v_out.clear();
	copy(v_in.begin(), v_in.end(), back_inserter(v_out));
	const char *sz = &v_out[0];

	byte *p_data = (byte*)sz;
	byte a = 0;
	byte b = 0;

	int ixor = mi_xor;
	for(size_t i = 0; i < size; ++i)
	{
		a = p_data[i] & ARR[num];
		p_data[i] >>= num;
		b = a << (8 - num);
		p_data[i] |= b;
		ixor = abs((ixor + 153) * (ixor - 78) + 3) % 256;
		p_data[i] ^= ixor;
	}

	return true;
}

bool crypto::decode(const std::vector<char>& v_in,
					std::vector<char>& v_out,
					const int i_num) const
{
	v_out.clear();

	if(v_in.empty())
		return true;

	const size_t size = v_in.size();

	// 右移位数因子对照表
	static const byte ARR[9] = {0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
	const int num = i_num % 8;  //0-7的整形
	if(num < 0 || num > 8)
		return false;

	v_out.clear();
	copy(v_in.begin(), v_in.end(), back_inserter(v_out));
	const char *sz = &v_out[0];

	byte *p_data = (byte*)sz;
	byte a = 0;
	byte b = 0;
	int ixor = mi_xor;
	for(size_t i = 0; i < size; ++i)
	{
		ixor = abs((ixor + 153) * (ixor - 78) + 3) % 256;
		p_data[i] ^= ixor;
		a = p_data[i] & ARR[num];
		p_data[i] >>= num;
		b = a << (8 - num);
		p_data[i] |= b;
	}

	return true;
}

bool crypto::encode_to_string(const std::vector<char>& v_in,
							  std::string& s_out,
							  const int i_num) const
{
	s_out.clear();
	std::vector<char> v_data;
	bool rb = encode(v_in, v_data, i_num);
	hex_to_string(s_out, v_data);
	return rb;
}

bool crypto::decode_from_string(const std::string& s_in,
								std::vector<char>& v_out,
								const int i_num) const
{
	v_out.clear();
	std::vector<char> v_data;
	string_to_hex(v_data, s_in);
	bool rb = decode(v_data, v_out, i_num);
	return rb;
}

std::string& crypto::hex_to_string(std::string& s_data,
								   const std::vector<char>& v_data) const
{
	s_data.clear();
	for(unsigned int i = 0; i < v_data.size(); ++i)
	{
		char szBuff[3] = "";
		sprintf(szBuff, "%02x", *reinterpret_cast<const unsigned char *>(&v_data[i]));
		s_data.push_back(szBuff[0]);
		s_data.push_back(szBuff[1]);
	}
	return s_data;
}

void crypto::string_to_hex(std::vector<char>& v_data,
						   const std::string& s_data) const
{
	v_data.clear();

	unsigned int ui = 0L;
	for(unsigned int i = 0; i < s_data.size(); ++i)
	{
		unsigned int localui = 0L;
		const char c = s_data[i];
		if('0' <= c && c <= '9')
		{
			localui = c - '0';
		}
		else if('A' <= c && c <= 'F')
		{
			localui = c - 'A' + 10;
		}
		else if('a' <= c && c <= 'f')
		{
			localui = c - 'a' + 10;
		}

		if(i % 2 == 0)
		{
			ui = localui * 16L;
		}
		else
		{
			ui += localui;
			v_data.push_back(ui);
		}
	}
}
}
