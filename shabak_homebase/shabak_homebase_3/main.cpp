#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

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
					offset = (j + k - direction_delta);
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
}

/*
	BruteForce:

	You are given a file, Do what ever it takes to decrypt the file's content (with the decryption logic from the previous task).
	Consider the fact that the key consists of 3 steps, and that the encrypted messsage contains only letters, spaces and '.' and '?'
	you should be able to decrypt the message into a readable quote :-)

	This quote will be the answer to the challenge
*/

namespace BruteForce
{
	bool check_content(std::vector<unsigned char> const& product)
	{
		for (std::size_t i = 0; i < product.size(); i++)
		{
			if ((product.at(i) < 'a' || product.at(i) > 'z') && (product.at(i) < 'A' || product.at(i) > 'Z') && product.at(i) != ' ' && product.at(i) != '.' && product.at(i) != '?')
				return false;
		}

		return true;
	}

	bool test_key(std::vector<MegaDecryptor::operation_descriptor>& key, MegaDecryptor::operation_descriptor& key_part, 
		std::function<bool(std::vector<MegaDecryptor::operation_descriptor>& key)> test_function)
	{
		for (int code = 0; (code < 3); code++)
		{
			key_part.operation_code = static_cast<MegaDecryptor::OPERATION_CODE>(code);

			for (int parameter = 0; parameter < 256; parameter++)
			{
				key_part.operation_parameter = static_cast<unsigned char>(parameter);

				for (int length = 0; length < 256; length++)
				{
					key_part.operation_length = length;

					if (test_function(key))
						return true;
				}
			}
		}

		return false;
	}

	void print(std::vector<unsigned char>& temp)
	{
		for (auto x : temp)
		{
			if (isprint(x))
				printf("%c", x);
			else
				printf("_");
		}

		printf("\n");
	}

	bool attempt(std::vector<unsigned char> const& product)
	{
		std::vector<MegaDecryptor::operation_descriptor> key = { { MegaDecryptor::SUB, 0x23, 54 }, { MegaDecryptor::XOR, 0, 0}, { MegaDecryptor::XOR, 0, 0 }  };
		
		test_key(key, key.at(0), [&](std::vector<MegaDecryptor::operation_descriptor>& key) -> bool
		{
			return test_key(key, key.at(1), [&](std::vector<MegaDecryptor::operation_descriptor>& key) -> bool
			{
				return test_key(key, key.at(2), [&](std::vector<MegaDecryptor::operation_descriptor>& key) -> bool
				{
					std::vector<unsigned char> temp(product);
					MegaDecryptor::decrypt(temp, key);

					print(temp);

					if (check_content(temp))
					{
						for (int i = 0; i < 3; i++)
							printf("key %d: { %d, %d, %d }\n", i, key.at(i).operation_code, key.at(i).operation_parameter, key.at(i).operation_length);

						return true;
					}

					return false;
				});
			});
		});

		return false;
	}
}

int main()
{
	std::vector<unsigned char> cipher_product;

	if (MegaDecryptor::read_file("EncryptedMessage.bin", cipher_product))
	{
		if (BruteForce::attempt(cipher_product))
			std::cout << std::string(cipher_product.begin(), cipher_product.end()) << std::endl;
	}

	std::cout << "End of program.\n" << std::endl;

	std::cin.ignore();
	std::cin.get();
	return 0;
}