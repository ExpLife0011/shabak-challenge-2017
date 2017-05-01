#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <map>
#include <vector>

/*
	MakeMeCompile:

	Here we an unfinished project, We belive that by making it comile & run correctly it will give us the password.
	Do what ever you can to compile this project
*/

std::vector<unsigned char> SomeFunction145();	// GetEncryptedPassword
std::vector<unsigned char> SomeFunction1362();	// GetEncryptionKey

std::vector<unsigned char> Encrypt(std::vector<unsigned char> const& plainText, std::vector<unsigned char> const& key)
{
	const unsigned char xorKey = key[0];
	const unsigned char numberOfBitsToRotate = key[1];

	std::vector<unsigned char> result;

	std::transform(plainText.begin(), plainText.end(), std::back_inserter(result),
		[&](const unsigned char byte) -> int
	{
		const unsigned char xored = byte ^ xorKey;
		const unsigned char shifted = _rotl8(xored, numberOfBitsToRotate);
			
		return shifted;
	});

	return result;
}

std::vector<unsigned char> Decrypt(std::vector<unsigned char> const& cipherText, std::vector<unsigned char> const& key)
{
	const unsigned char xorKey = key[0];
	const unsigned char numberOfBitsToRotate = key[1];

	std::vector<unsigned char> result;

	std::transform(cipherText.cbegin(), cipherText.cend(), std::back_inserter(result),
		[&](const unsigned char byte) -> int
	{
		const unsigned char shifted = _rotr8(byte, numberOfBitsToRotate);
		const unsigned char xored = shifted ^ xorKey;

		return xored;
	});

	return result;
}

int main()
{
	std::vector<unsigned char> const encryptedBuffer = SomeFunction145();
	std::vector<unsigned char> const encryptionKey = SomeFunction1362();
	std::vector<unsigned char> const passwordBinary = Decrypt(encryptedBuffer, encryptionKey);

	/* RoadRage */
	std::cout << std::string(passwordBinary.cbegin(), passwordBinary.cend()) << std::endl;
	
	std::cin.ignore();
	std::cin.get();
	return 0;
}