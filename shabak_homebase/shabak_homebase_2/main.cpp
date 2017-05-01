#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/*
	MegaDecryptor
	--------------

	In this exercise you are required to decrypt the secret passphrase that was encrypted with the following scheme.
	The encrypted buffer ("CipherText.bin") is encrypted by an alternating stream whose behavior is defined by the key.
	The given key ("Key.bin") is an array of EncryptionStepDescriptors, where an EncryptionStepDescriptor is
	defined as follows:

	struct EncryptionStepDescriptor
	{
		UINT8 operationCode;
		UINT8 operationParameter;
		UINT32 lengthToOperateOn;
	};

	Each EncryptionStepDescriptor describes a single step for a specific operation on the stream.

	a) operationCode - Here are the operationCodes:
	1) Xor = 0
	2) Add = 1
	3) Subtract = 2

	b) operationParameter - Each operation code receives an operation parameter (operationParameter) and uses it with the current
	byte in the stream to compute a binary operation. 

	c) lengthToOperateOn - The amount of bytes in the stream to apply this binary operation to.

	For example:
	Given the stream(/"CipherText.bin"): "ccccccc" and the key: { {Add, 1, 4}, {Subtract, 2, 3} }
	we should end up with: "ddddaaa"

	If you have reached the end of the stream, but there are more operations to do according to the key
	you have to apply the key in reverse.

	For example:
	Given the stream(/"CipherText.bin"): "aaaaaaaaaaeeeeehhhhhhhhhgggghh" and the key: 
	{
			{
				Add,
				5,
				10
			},
			{
				Add,
				1,
				5
			},
			{
				Subtract,
				2,
				9
			},
			{
				Subtract,
				1,
				8
			}
		}
	we should end up with: "ffffffffffffffffffffffffffffff"
*/

namespace MegaDecryptor
{
	enum OPERATION_CODE : unsigned char
	{
		XOR = 0,
		ADD = 1,
		SUB = 2
	};

#pragma pack(push, 1)
	struct operation_descriptor
	{
		OPERATION_CODE operation_code;
		unsigned char operation_parameter;
		unsigned int operation_length;	// The amount of bytes in the stream to apply this binary operation to.
	};
#pragma pack(pop)

	unsigned char operation(unsigned char value, operation_descriptor const& descriptor)
	{
		switch (descriptor.operation_code)
		{
		case XOR:
			return (value ^ descriptor.operation_parameter);

		case ADD:
			return (value + descriptor.operation_parameter);

		case SUB:
			return (value - descriptor.operation_parameter);

		default:
			std::cout << "[operation] Incorrect parameter." << std::endl;
			break;
		}

		return 0;
	}
	
	void decrypt(std::vector<unsigned char>& input, std::vector<operation_descriptor> const& key)
	{
		for (std::size_t i = 0, j = 0, direction = 0; i < key.size(); j += key.at(i).operation_length, i++)
		{
			for (std::size_t k = 0; k < key.at(i).operation_length; k++)
			{
				if ((j + k) != 0 && (((j + k) % input.size()) == 0))
					direction++;
				
				std::size_t direction_delta = (direction * input.size());
				std::size_t offset = 0;

				if ((direction % 2) == 0)
				{	
					offset = (j + k - direction_delta);
				}
				else 
				{
					std::size_t reverse_delta = (((j + k) -  direction_delta) + 1);
					offset = (input.size() - reverse_delta);
				}
				
				input[offset] = operation(input.at(offset), key.at(i));
			}
		}
	}

	bool parse_key(std::vector<unsigned char> const& product, std::vector<operation_descriptor>& key)
	{
		if ((product.size() % sizeof(operation_descriptor)) != 0)
			return false;
		
		for (std::size_t i = 0; i < product.size(); i += sizeof(operation_descriptor))
			key.push_back({ static_cast<OPERATION_CODE>(product[i]), static_cast<unsigned char>(product[i + 1]), *reinterpret_cast<const unsigned int*>(&product[i + 2]) });

		return true;
	}

	bool read_file(std::string const& file_path, std::vector<unsigned char>& product)
	{
		std::ifstream file(file_path, std::ios::binary);
		
		if (!file.good())
			return false;

		product = std::vector<unsigned char>((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

		file.close();
		return true;
	}
	
	void test_short()
	{
		std::vector<unsigned char> short_cipher_product = { 'c', 'c', 'c', 'c', 'c', 'c', 'c' };
		MegaDecryptor::decrypt(short_cipher_product, { { MegaDecryptor::ADD, 1, 4 }, { MegaDecryptor::SUB, 2, 3 } });

		/* expected: ddddaaa */
		std::cout << std::string(short_cipher_product.begin(), short_cipher_product.end()) << std::endl;
	}

	void test_long()
	{
		std::vector<unsigned char> long_cipher_product = { 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'e', 'e', 'e', 'e', 'e', 'h', 'h', 'h', 'h', 'h', 'h', 'h', 'h', 'h', 'g', 'g', 'g', 'g', 'h', 'h' };

		std::string long_cipher_text = "aaaaaaaaaaeeeeehhhhhhhhhgggghh";
		MegaDecryptor::decrypt(long_cipher_product, { { MegaDecryptor::ADD, 5, 10 }, { MegaDecryptor::ADD, 1, 5 }, { MegaDecryptor::SUB, 2, 9 }, { MegaDecryptor::SUB, 1, 8 } });

		/* expected: ffffffffffffffffffffffffffffff */
		std::cout << std::string(long_cipher_product.begin(), long_cipher_product.end()) << std::endl;
	}
}

#include <Windows.h>
#include <winternl.h>

int main()
{
	std::vector<unsigned char> key_product;
	std::vector<MegaDecryptor::operation_descriptor> key;
	
	if (MegaDecryptor::read_file("Key.bin", key_product) && MegaDecryptor::parse_key(key_product, key))
	{
		std::vector<unsigned char> cipher_product;

		if (MegaDecryptor::read_file("EncryptedMessage.bin", cipher_product))
		{
			//MegaDecryptor::test_short();
			//MegaDecryptor::test_long();

			/* Great job! You are the 1337est hacker of all times! The password for this stage is: 'Look at me I'm Mister programmer'. Now go on and enjoy your coffee break */
			MegaDecryptor::decrypt(cipher_product, key);
			std::cout << std::string(cipher_product.begin(), cipher_product.end()) << std::endl;
		}
	}
	
	std::cout << "End of program.\n" << std::endl;

	std::cin.ignore();
	std::cin.get();
	return 0;
}