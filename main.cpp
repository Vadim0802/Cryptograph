#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include "json.hpp"
using json = nlohmann::json;

class ICiphers
{
public:
	virtual void KeyGenerator(std::string pathAlp, std::string pathSave) = 0;
	virtual void Encrypt(std::string pathKey, std::string pathSave, std::string pathTextFile) = 0;
	virtual void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText) = 0;
};

class Replacement : ICiphers //check
{
public:
	void KeyGenerator(std::string pathAlp, std::string pathSave)
	{
		if (pathAlp.find(".alph") != -1)
		{
			std::ifstream readFile(pathAlp);
			json alp;
			readFile >> alp;
			readFile.close();
			auto alpCorrect = alp.find("alp");
			if (alpCorrect != alp.end()) 
			{
				json Key = { {"Alg_Type", "Replacement"}, {"Key", {}} };
				std::string alpToString;
				json alpData = alp.at("alp");
				json::iterator it = alpData.begin();
				for (int i = 0; i < alp.at("alp").size(); i++)
				{
					std::string tmp = *it;
					alpToString.push_back(tmp[0]);
					it++;
				}
				std::string KeyReplacement = "";
				srand(time(NULL));
				int tmp_key;
				for (int i = 0; i < alpToString.size(); i++)
				{	
					while (1)
					{
						bool isTrueRandom = true;
						tmp_key = rand() % alpToString.size();
						for (int j = 0; j < i; j++)
						{
							if (alpToString[tmp_key] == KeyReplacement[j])
							{
								isTrueRandom = false;
								break;
							}
						}
						if (isTrueRandom)
							break;
					}
					KeyReplacement.push_back(alpToString[tmp_key]);
				}
				for (int i = 0; i < alpToString.size(); i++)
				{
					std::string tmp_alpToString;
					std::string tmp_KeyReplacement;
					tmp_alpToString.push_back(alpToString[i]);
					tmp_KeyReplacement.push_back(KeyReplacement[i]);
					Key.at("Key").push_back(json::array({ tmp_alpToString , tmp_KeyReplacement}));
				}
				std::ofstream inputFile(pathSave);
				inputFile << Key;
				inputFile.close();
			}
		}
		else
		{
			std::cout << "This is not .alph file\n";
		}
	}
	void Encrypt(std::string pathKey, std::string pathSave, std::string pathTextFile)
	{
		if ((pathTextFile.find(".txt") != -1) && (pathKey.find(".key") != -1) && (pathSave.find(".encrypt")))
		{
			json key;
			std::ifstream readFile(pathKey);
			readFile >> key;
			readFile.close();
			std::string keyToString;
			std::string textForEncrypt;
			std::string result;
			if (key.at("Alg_Type") == "Replacement")
			{
				for (int i = 0; i < key.at("Key").size(); i++)
				{
					for (int j = 0; j < key.at("Key").at(i).size(); j++)
					{
						keyToString += key.at("Key").at(i).at(j);
					}
				}
				std::ifstream getText(pathTextFile);
				while (!getText.eof())
				{
					std::getline(getText, textForEncrypt);
				}
				getText.close();
				for (int i = 0; i < textForEncrypt.size(); i++)
				{
					for (int j = 0; j < keyToString.size(); j += 2)
					{
						if (textForEncrypt[i] == keyToString[j])
						{
							result.push_back(keyToString[j + 1]);
						}
					}
					if (textForEncrypt[i] == ' ')
					{
						result.push_back(' ');
					}
				}
				json resultEncrypt = { {"Alg_Type", "Replacement"}, {"TextEncrypt", result} };
				std::ofstream inputFile(pathSave);
				inputFile << resultEncrypt;
			}
			else
			{
				std::cout << "Not correct type!\n";
			}
		}
		else
		{
			std::cout << "Invalid format file!\n";
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt")))
		{
			json EncryptText;
			std::ifstream getEncryptText(pathEncryptText);
			getEncryptText >> EncryptText;
			getEncryptText.close();
			if (EncryptText.at("Alg_Type") == "Replacement")
			{
				json key;
				std::string keyToString;
				std::string result;
				std::string textForDecrypt = EncryptText.at("TextEncrypt");
				std::ifstream readFile(pathKey);
				readFile >> key;
				readFile.close();
				for (int i = 0; i < key.at("Key").size(); i++)
				{
					for (int j = 0; j < key.at("Key").at(i).size(); j++)
					{
						keyToString += key.at("Key").at(i).at(j);
					}
				}
				for (int i = 0; i < textForDecrypt.size(); i++)
				{
					for (int j = 1; j < keyToString.size(); j += 2)
					{
						if (textForDecrypt[i] == keyToString[j])
						{
							result.push_back(keyToString[j - 1]);
						}
					}
					if (textForDecrypt[i] == ' ')
					{
						result.push_back(' ');
					}
				}
				std::ofstream inputFile(PathSave);
				inputFile << result;
			}
			else
			{
				std::cout << "Incorrect type!\n";
			}
		}
		else
		{
			std::cout << "Invalid input!\n";
		}
	}
};

class Shift : ICiphers
{
public:
	void KeyGenerator(std::string pathAlp, std::string pathSave)
	{
		if (pathAlp.find(".alph") != -1 && pathSave.find(".key") != -1)
		{
			int keySize;
			std::cout << "Enter keySize: "; std::cin >> keySize;
			if (keySize > 0)
			{
				json key = { {"Alg_Type", "Shift"}, {"Key", {}} };
				int* tmp_key = new int[keySize];
				int tmp_digit;
				srand(time(NULL));
				for (int i = 0; i < keySize; i++)
				{
					while (1)
					{
						bool isTrueRandom = true;
						tmp_digit = 1 + rand() % keySize;
						for (int j = 0; j < i; j++)
						{
							if (tmp_key[j] == tmp_digit)
							{
								isTrueRandom = false;
								break;
							}
						}
						if (isTrueRandom)
							break;
					}
					tmp_key[i] = tmp_digit;
				}
				for (int i = 0; i < keySize; i++)
				{
					key.at("Key").push_back(tmp_key[i]);
				}
				std::ofstream inputFile(pathSave);
				inputFile << key;
				inputFile.close();
				delete[] tmp_key;
			}
			else
			{
				std::cout << "Error!\n";
			}
		}
		else
		{
			std::cout << "Error!\n";
		}

	}
	void Encrypt(std::string pathKey, std::string pathSave, std::string pathTextFile)
	{
		if (pathKey.find(".key") != -1 && pathSave.find(".encrypt") != -1 && pathTextFile.find(".txt") != -1)
		{
			json key;
			std::ifstream readFile(pathKey);
			readFile >> key;
			readFile.close();
			if (key.at("Alg_Type") == "Shift")
			{
				std::string text;
				std::string result;
				std::ifstream getText(pathTextFile);
				while (!getText.eof())
				{
					std::getline(getText, text);
				}
				getText.close();
				int keySize = key.at("Key").size();
				int evenLength = text.size() % keySize;
				srand(time(NULL));
				if (evenLength != 0)
				{
					int _tmp = keySize - evenLength;
					for (int i = 0; i < _tmp; i++)
					{
						char _tmp_char = 33 + rand() % 13;
						text.push_back(_tmp_char);
					}
				}
				char* res = new char[text.size()];
				int _tmp_div = text.size() / keySize;
				int iterator = 0;
				for (int i = 0; i < _tmp_div; i++)
				{
					for (int j = 0; j < keySize; j++)
					{
						int keyElem = key.at("Key").at(j) - 1;
						res[keyElem + i * keySize] = text[iterator];
						iterator++;
					}
				}
				for (int i = 0; i < text.size(); i++)
				{
					result.push_back(res[i]);
				}
				json resultEncrypt = { {"Alg_Type", "Shift"}, {"TextEncrypt", result} };
				delete[] res;
				std::ofstream inputFile(pathSave);
				inputFile << resultEncrypt;
				inputFile.close();

			}
			else
			{
				std::cout << "Error!\n";
			}
		}
		else
		{
			std::cout << "Error!\n";
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt")))
		{
			json getText;
			std::ifstream readFile(pathEncryptText);
			readFile >> getText;
			readFile.close();
			if (getText.at("Alg_Type") == "Shift")
			{
				json key;
				std::string result;
				std::ifstream readFile(pathKey);
				readFile >> key;
				readFile.close();
				if (key.at("Alg_Type") == "Shift")
				{
					int KeySize = key.at("Key").size();
					std::string text = getText.at("TextEncrypt");
					char* res = new char[text.size()];
					int _tmp_div = text.size() / KeySize;
					int iterator = 0;
					for (int i = 0; i < _tmp_div; i++)
					{
						for (int j = 0; j < KeySize; j++)
						{
							int keyElem = key.at("Key").at(j) - 1;
							res[j + i * KeySize] = text[i * KeySize + keyElem];
							iterator++;
						}
					}
					for (int i = 0; i < text.size(); i++)
					{
						if (res[i] >= 33 && res[i] <= 46) continue;
						result.push_back(res[i]);
					}
					delete[] res;
					std::ofstream inputFile(PathSave);
					inputFile << result;
					inputFile.close();
				}
				else
				{
					std::cout << "Error!\n";
				}
			}
			else
			{
				std::cout << "Error\n";
			}
		}
		else
		{
			std::cout << "Error!\n";
		}
	}
};

class Gamma
{
	void KeyGenerator(std::string pathAlp, std::string pathSave)
	{

	}
	void Encrypt(std::string pathKey, std::string pathSave, std::string pathTextFile)
	{

	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{

	}
};

int main()
{
	Shift a;
	//a.KeyGenerator("my_alphabet.alph", "my_key.key");
	//a.Encrypt("my_key.key", "my_text.encrypt", "my_text.txt");
	//a.Decrypt("my_key.key", "my_text.decrypt.txt", "my_text.encrypt");
	a.KeyGenerator("my_alphabet.alph", "my_keyShift.key");
	a.Encrypt("my_keyShift.key", "my_textShift.encrypt", "my_text.txt");
	a.Decrypt("my_keyShift.key", "my_textShift.decrypt.txt", "my_textShift.encrypt");
}