#include <iostream>
#include <string>

namespace base64
{
	static char encoding_table[] =
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};

	static char decoding_table[128] = { 0 };

	bool decode(std::string input, std::string& output)
	{
		for (unsigned int i = 0, j = 0; i < input.length(); i += 4)
		{
			unsigned char c1 = decoding_table[input.at(i)];

			if (c1 == 0xFF)
				return false;
			
			unsigned char c2 = decoding_table[input.at(i + 1)];

			if (c2 == 0xFF)
				return false;

			output.push_back((c1 << 2) | ((c2 & 0x30) >> 4));

			if (output.length() >= input.length())
				break;

			unsigned char c3 = decoding_table[input.at(i + 2)];
			
			if (c3 == 0xFF)
				return false;

			output.push_back(((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2));

			if (output.length() >= input.length())
				break;

			unsigned char c4 = decoding_table[input.at(i + 3)];
			
			if (c4 == 0xFF)
				return false;
			
			output.push_back(((c3 & 0x03) << 6) | c4);
		}

		return true;
	}

	void generate_decode_table() 
	{
		memset(decoding_table, sizeof(decoding_table), 0xFF);
		
		for (int i = 0; i < 64; i++)
			decoding_table[encoding_table[i]] = i;
	}
}

int main()
{
	base64::generate_decode_table();
	
	std::string input = "MTAxMDAxMTAxMTAxMDAwMDExMDAwMDEwMTEwMDAxMDAxMTAwMDAxMDExMDEwMTEuY29t";
	std::string output = "";

	if (base64::decode(input, output))
	{
		/* 10100110110100001100001011000100110000101101011.com */
		std::cout << output.c_str() << std::endl;
	}

	std::cin.ignore();
	std::cin.get();
	return 0;
}