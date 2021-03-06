/*
Author: Nikolaos P, NCC Group
Purpose: Decrypts the config files of Chafer. 
Analysed sample: bc32bf55e841052a095c27cea558577fa947fdf8b7d95beca0c5725dbd00324d

Tested with the following config files:

 * 12F79030E73030E127AE3EC5AB16B51CD5F7812E786E6D23FC54B5820F5F3064
 * D3ECD0E5F6DD6B6FA1E8FDAD7E0EC9B1020E5BB0B9E64F76D2148F1B24CA7779

Usage: decrypter.exe config.dat

Released as open source by NCC Group, under the AGPL license.
*/


#include <Windows.h>
#include <iostream>
#include <vector>

int main(int argc,char** argv)
{
	DWORD  dwBytesRead = 0;
	if (argc == 2)
	{

		HANDLE hFile = CreateFileA(argv[1],                
			GENERIC_READ,
			FILE_SHARE_READ,                      
			NULL,                 
			OPEN_EXISTING,            
			FILE_ATTRIBUTE_NORMAL,   
			NULL);                   
		if (hFile == INVALID_HANDLE_VALUE)
		{
			std::cout << "Error - Could not get a handle of the file" << std::endl;
			return 0;
		}
		DWORD size = GetFileSize(hFile, 0);
		if (size <=0)
		{
			std::cout << "Error with the size of the file" << std::endl;
			return 0;
		}
		char *ReadBuffer  = new char[size];
		if (ReadBuffer == nullptr)
		{
			std::cout << "Failed allocating memory" << std::endl;
		}
		if (!ReadFile(hFile, ReadBuffer, size, &dwBytesRead, NULL))
		{
			std::cout << "Error - Could not read the file" << std::endl;
			delete[]ReadBuffer;
			return 0;
		}
		CloseHandle(hFile);

		int andvalue = 0;
		BYTE decrypted_chr = 0;
		std::vector<DWORD> seeds;
		if (*(DWORD*)ReadBuffer == 0xae2a6d || *(DWORD*)ReadBuffer == 0x31b9d63)
		{
			std::cout << "Valid config file found, getting the seeds.\n" << std::endl;
			DWORD seed1 = *(DWORD*)(ReadBuffer + 4);
			seeds.push_back(seed1);
			DWORD seed2 = *(DWORD*)(ReadBuffer + 8);
			seeds.push_back(seed2);
			DWORD seed3 = *(DWORD*)(ReadBuffer + 0xC);
			seeds.push_back(seed3);
			DWORD seed4 = *(DWORD*)(ReadBuffer + 0x10);
			seeds.push_back(seed4);
			DWORD seed5 = *(DWORD*)(ReadBuffer + 0x14);
			seeds.push_back(seed5);
			if (size < 0x6A) {
				return 0;
			}
			int nextseeds = size - 0x6A;
			DWORD seed6 = *(DWORD*)(ReadBuffer + nextseeds);
			seeds.push_back(seed6);
			DWORD seed7 = *(DWORD*)(ReadBuffer + nextseeds + 4);
			seeds.push_back(seed7);
			DWORD seed8 = *(DWORD*)(ReadBuffer + nextseeds + 8);
			seeds.push_back(seed8);
			DWORD seed9 = *(DWORD*)(ReadBuffer + nextseeds + 0xc);
			seeds.push_back(seed9);
			DWORD seed10 = *(DWORD*)(ReadBuffer + nextseeds + 0x10);
			seeds.push_back(seed10);

			int seed = 0;
			int randomkey = 0;
			if (size < 0x82)
			{
				return 0;
			}
			int sizetoread = size - 0x82;
			int vectorcounter = 0;
			unsigned char * decrypted_config = new  unsigned char[sizetoread];
			if (decrypted_config == nullptr)
			{
				std::cout << "Failed allocating memory" << std::endl;
				return 0;
			}
			for (int counter = 0; counter < sizetoread; counter++)
			{

				if (vectorcounter >= seeds.size())
				{
					vectorcounter = 0;
				}
				if (!(andvalue & 0x80000001))
				{
					seed = seeds.at(vectorcounter);
					srand(seed);
				}
				vectorcounter = vectorcounter + 1;


				randomkey = rand();
				decrypted_chr = *(BYTE*)(ReadBuffer + 0x18 + counter) ^ (BYTE)randomkey;
				decrypted_config[counter] = decrypted_chr;
				andvalue = andvalue + 1;
			}
			decrypted_config[sizetoread] = '\0';
			std::cout << "\nConfig: " << std::endl;
			std::cout << decrypted_config << std::endl;
			delete[] decrypted_config;
			delete[]ReadBuffer;

			return 1;
		}
		else {
			std::cout << "Invalid file" << std::endl;
			delete[]ReadBuffer;
			return 0;
		}
	}
	else {
		std::cout << "Error - Wrong parameters" << std::endl;
		return 0;
	}

}