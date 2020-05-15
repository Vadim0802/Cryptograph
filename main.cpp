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

class Replacement : ICiphers 
{
public:
	void KeyGenerator(std::string pathAlp, std::string pathSave)
	{
		if (pathAlp.find(".alph") != -1 && pathSave.find(".key") != -1)
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
		if ((pathTextFile.find(".txt") != -1) && (pathKey.find(".key") != -1) && (pathSave.find(".encrypt") != -1))
		{
			json key;
			json resultEncrypt = { {"Alg_Type", "Replacement"}, {"TextEncrypt", {}} };
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				readFile >> key;
				readFile.close();
				std::string keyToString;
				std::string textForEncrypt;
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
					if (getText.is_open()) {
						std::ofstream inputFile(pathSave);
						while (!getText.eof())
						{
							std::string result;
							std::getline(getText, textForEncrypt);
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
							resultEncrypt.at("TextEncrypt").push_back(result);
						}
						inputFile << resultEncrypt;
						getText.close();
						inputFile.close();
					}
					else {
						std::cout << "TextFile dont open!" << std::endl;
					}
				}
				else
				{
					std::cout << "Not correct Alg_Type!\n";
				}
			}
			else {
				std::cout << "keyFile dont open!" << std::endl;
			}
		}
		else
		{
			std::cout << "Error! pathText,pathSave,pathKey must contain .txt, .encrypt, .key!\n";
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1))
		{
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				json key;
				readFile >> key;
				std::string keyToString;
				for (int i = 0; i < key.at("Key").size(); i++)
				{
					for (int j = 0; j < key.at("Key").at(i).size(); j++)
					{
						keyToString += key.at("Key").at(i).at(j);
					}
				}
				std::ifstream getEncryptText(pathEncryptText);
				if (getEncryptText.is_open()) {
					json textEncrypt;
					getEncryptText >> textEncrypt;
					getEncryptText.close();
					if (textEncrypt.at("Alg_Type") == "Replacement") {
						std::ofstream inputFile(PathSave);
						for (int k = 0; k < textEncrypt.at("TextEncrypt").size(); k++) {
							std::string result;
							std::string text = textEncrypt.at("TextEncrypt").at(k);
							for (int i = 0; i < text.size(); i++)
							{
								for (int j = 1; j < keyToString.size(); j += 2)
								{
									if (text[i] == keyToString[j])
									{
										result.push_back(keyToString[j - 1]);
									}
								}
								if (text[i] == ' ')
								{
									result.push_back(' ');
								}
							}
							inputFile << result;
							inputFile << "\n";
						}
						inputFile.close();
					}
					else {
						std::cout << "Incorrect Alg_Type! Must be Replacement" << std::endl;
					}
				}
				else
				{
					std::cout << "keyFile dont open!\n";
				}
			}
			else {
				std::cout << "EncryptTextFile dont open!" << std::endl;
			}
		}
		else {
			std::cout << "Error! pathEncryptText,pathSave,pathKey must contain .encrypt, .decrypt.txt, .key!\n" << std::endl;
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
			json resultEncrypt = { {"Alg_Type", "Shift"}, {"TextEncrypt", {}} };;
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				readFile >> key;
				readFile.close();
				if (key.at("Alg_Type") == "Shift")
				{
					std::ifstream getText(pathTextFile);
					if (getText.is_open()) {
						std::ofstream inputFile(pathSave);
						while (!getText.eof()) {
							std::string text;
							std::string result;
							std::getline(getText, text);
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
							delete[] res;
							resultEncrypt.at("TextEncrypt").push_back(result);
						}
						inputFile << resultEncrypt;
						inputFile.close();
					}
					else
					{
						std::cout << "TextFile dont open!\n";
					}
					getText.close();
				}
				else {
					std::cout << "not correct Alg_Type!" << std::endl;
				}
			}
			else {
				std::cout << "pathKey dont open!" << std::endl;
			}
		}
		else
		{
			std::cout << "Error! pathText,pathSave,pathKey must contain .txt, .encrypt, .key!\n";
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1))
		{
			json key;
			json textEncrypt;
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				readFile >> key;
				readFile.close();
				if (key.at("Alg_Type") == "Shift")
				{
					std::ifstream readFile(pathEncryptText);
					if (readFile.is_open()) {
						readFile >> textEncrypt;
						readFile.close();
						if (textEncrypt.at("Alg_Type") == "Shift") {
							std::ofstream inputFile(PathSave);
							for (int k = 0; k < textEncrypt.at("TextEncrypt").size(); k++) {
								std::string text = textEncrypt.at("TextEncrypt").at(k);
								std::string result;
								int KeySize = key.at("Key").size();
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
								inputFile << result;
								inputFile << "\n";
							}
							inputFile.close();
						}
						else {
							std::cout << "Incorrect Alg_Type! Must be Shift" << std::endl;
						}
					}
				}
				else {
						std::cout << "Invalid Alg_Type in keyFile!\n" << std::endl;
				}
			}
			else {
				std::cout << "keyFile dont open!" << std::endl;
			}
		}
		else
		{
			std::cout << "Error! pathEncryptText,pathSave,pathKey must contain .encrypt, .decrypt.txt, .key!\n";
		}
	}
};

class Gamma : ICiphers
{
public:
	void KeyGenerator(std::string pathAlp, std::string pathSave)
	{
		if (pathAlp.find(".alph") != -1) {
			std::ifstream readFile(pathAlp);
			json alph;
			if (readFile.is_open()) {
				readFile >> alph;
				if (alph.find("alp") != alph.end()) {
					json key = { {"Alg_Type", "Gamma"}, {"Key", {}} };
					int length;
					std::cout << "Enter length key: " << std::endl;
					std::cin >> length;
					if (length > 0) {
						int* arr = new int[length];
						srand(time(NULL));
						int tmp_digit;
						for (int i = 0; i < length; i++) {
							while (1) {
								bool random = true;
								tmp_digit = 1 + rand() % alph.at("alp").size();
								for (int j = 0; j < i; j++) {
									if (arr[i] == tmp_digit) {
										random = false;
										break;
									}
								}
								if (random) {
									break;
								}
							}
							arr[i] = tmp_digit;
						}
						for (int i = 0; i < length; i++) {
							key.at("Key").push_back(arr[i]);
						}
						std::ofstream inputFile(pathSave);
						inputFile << key;
						inputFile.close();
						delete[] arr;
					}
					else {
						std::cout << "Length key must be > 0" << std::endl;
					}
				}
				else {
					std::cout << "Incorrect format AlpFile" << std::endl;
				}
			}
			else {
				std::cout << "Don open alpFile" << std::endl;
			}
		}
		else {
			std::cout << "Error type file" << std::endl;
		}

	}
	void Encrypt(std::string pathKey, std::string pathSave, std::string pathTextFile)
	{
		if ((pathTextFile.find(".txt") != -1) && (pathKey.find(".key") != -1) && (pathSave.find(".encrypt") != -1)) {
			json key;
			json resultEncrypt = { {"Alg_Type", "Gamma"}, {"TextEncrypt", {}} };
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				readFile >> key;
				readFile.close();
				std::string pathAlph;
				std::cout << "Enter path to alph: " << std::endl;
				std::cin >> pathAlph;
				json alph;
				std::ifstream readFile(pathAlph);
				if (readFile.is_open()) {
					readFile >> alph;
					readFile.close();
					if (alph.find("alp") != alph.end()) {
						std::ifstream getText(pathTextFile);
						if (getText.is_open()) {
							std::ofstream inputFile(pathSave);
							while (!getText.eof()) {
								std::string text;
								std::getline(getText, text);
								for (int i = 0; i < text.size(); i++) {
									bool isAlpSymbol = false;
									std::string symbol;
									symbol.push_back(text[i]);
									int positionSymbol;
									for (int j = 0; j < alph.at("alp").size(); j++) {
										if (symbol == alph.at("alp").at(j)) {
											positionSymbol = j;
											isAlpSymbol = true;
											break;
										}
									}
									if (isAlpSymbol) {
										if (i >= key.at("Key").size()) {
											int tmp_i = i % key.at("Key").size();
											int positionResultSymbol = (positionSymbol + key.at("Key").at(tmp_i)) % alph.at("alp").size();
											std::string result = alph.at("alp").at(positionResultSymbol);
											text[i] = result[0];
										}
										else {
											int positionResultSymbol = (positionSymbol + key.at("Key").at(i)) % alph.at("alp").size();
											std::string result = alph.at("alp").at(positionResultSymbol);
											text[i] = result[0];
										}
									}
								}
								resultEncrypt.at("TextEncrypt").push_back(text);
							}
							inputFile << resultEncrypt;
							inputFile.close();
						}
						else {
							std::cout << "Dont open TextFile" << std::endl;
						}
						getText.close();
					}
					else {
						std::cout << "In alphFile not 'alp' indeficate" << std::endl;
					}
				}
				else {
					std::cout << "Dont open AlphFile" << std::endl;
				}
			}
			else {
				std::cout << "Dont open keyFile!" << std::endl;
			}
		}
		else {
			std::cout << "Error! pathText,pathSave,pathKey must contain .txt, .encrypt, .key!\n";
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1)) {
			json key;
			std::ifstream readFile(pathKey);
			if (readFile.is_open()) {
				readFile >> key;
				readFile.close();
				std::string pathAlph;
				std::cout << "Enter path to alph: " << std::endl;
				std::cin >> pathAlph;
				json alph;
				json textEncrypt;
				std::ifstream readFile(pathAlph);
				if (readFile.is_open()) {
					readFile >> alph;
					readFile.close();
					if (alph.find("alp") != alph.end()) {
						std::ifstream getText(pathEncryptText);
						if (getText.is_open()) {
							getText >> textEncrypt;
							getText.close();
							if (textEncrypt.at("Alg_Type") == "Gamma") {
								std::ofstream inputFile(PathSave);
								for (int k = 0; k < textEncrypt.at("TextEncrypt").size(); k++) {
									std::string text = textEncrypt.at("TextEncrypt").at(k);
									for (int i = 0; i < text.size(); i++) {
										bool isAlpSymbol = false;
										std::string symbol;
										symbol.push_back(text[i]);
										int positionSymbol;
										for (int j = 0; j < alph.at("alp").size(); j++) {
											if (symbol == alph.at("alp").at(j)) {
												positionSymbol = j;
												isAlpSymbol = true;
											}
										}
										if (isAlpSymbol) {
											if (i >= key.at("Key").size()) {
												int tmp_i = i % key.at("Key").size();
												int positionResultSymbol = (positionSymbol - key.at("Key").at(tmp_i) + alph.at("alp").size()) % alph.at("alp").size();
												std::string result = alph.at("alp").at(positionResultSymbol);
												text[i] = result[0];
											}
											else {
												int positionResultSymbol = (positionSymbol - key.at("Key").at(i) + alph.at("alp").size()) % alph.at("alp").size();
												std::string result = alph.at("alp").at(positionResultSymbol);
												text[i] = result[0];
											}
										}
									}
									inputFile << text;
									inputFile << "\n";
								}
								inputFile.close();
							}
							else {
								std::cout << "Incorrect Alg_Type! Must be Gamma" << std::endl;
							}
						}
						else {
							std::cout << "Dont open TextFile" << std::endl;
						}
					}
					else {
						std::cout << "In alphFile not 'alp' indeficate" << std::endl;
					}
				}
				else {
					std::cout << "Dont open AlphFile" << std::endl;
				}
			}
			else {
				std::cout << "Dont open keyFile!" << std::endl;
			}
		}
		else {
			std::cout << "Error! pathEncryptText,pathSave,pathKey must contain .encrypt, .decrypt.txt, .key!\n";
		}
	}
};



int main()
{
	while (1)
	{
		int choice = 0;
		std::cout << "\n----Menu----\n";
		std::cout << "1. Encrypt/Decrypt\n";
		std::cout << "2. Generate key\n";
		std::cout << ">>"; std::cin >> choice;
		if (choice == 1)
		{
			choice = 0;
			std::cout << "\n----Menu->Encrypt/Decrypt----\n";
			std::cout << "1. Encrypt\n";
			std::cout << "2. Decrypt\n";
			std::cout << ">>"; std::cin >> choice;
			if (choice == 1)
			{
				choice = 0;
				std::cout << "\n----Menu->Encrypt/Decrypt->Encrypt----\n";
				std::cout << "1. Replacement\n";
				std::cout << "2. Shift\n";
				std::cout << "3. Gamma\n";
				std::cout << ">>"; std::cin >> choice;
				if (choice == 1)
				{
					Replacement a;
					std::string pathKey;
					std::string pathSave;
					std::string pathTextFile;
					std::cout << "\npathKey >> "; std::cin >> pathKey;
					std::cout << "\npathTextFile >> "; std::cin >> pathTextFile;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Encrypt(pathKey, pathSave, pathTextFile);
					system("PAUSE");
					system("cls");
				}
				else if (choice == 2)
				{
					Shift a;
					std::string pathKey;
					std::string pathSave;
					std::string pathTextFile;
					std::cout << "\npathKey >> "; std::cin >> pathKey;
					std::cout << "\npathTextFile >> "; std::cin >> pathTextFile;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Encrypt(pathKey, pathSave, pathTextFile);
					system("PAUSE");
					system("cls");
				}
				else if (choice == 3)
				{
					Gamma a;
					std::string pathKey;
					std::string pathSave;
					std::string pathTextFile;
					std::cout << "\npathKey >> "; std::cin >> pathKey;
					std::cout << "\npathTextFile >> "; std::cin >> pathTextFile;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Encrypt(pathKey, pathSave, pathTextFile);
					system("PAUSE");
					system("cls");
				}
				else
				{
					std::cout << "\nIncorrect input!\n";
					system("PAUSE");
					system("cls");
				}
			}
			else if (choice == 2)
			{
				choice = 0;
				std::cout << "\n----Menu->Encrypt/Decrypt->Decrypt----\n";
				std::cout << "1. Replacement\n";
				std::cout << "2. Shift\n";
				std::cout << "3. Gamma\n";
				std::cout << ">>"; std::cin >> choice;
				if (choice == 1)
				{
					Replacement a;
					std::string pathKey;
					std::string pathSave;
					std::string pathEncryptText;
					std::cout << "\npathKey >> "; std::cin >> pathKey;
					std::cout << "\npathEncryptText >> "; std::cin >> pathEncryptText;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Decrypt(pathKey, pathSave, pathEncryptText);
					system("PAUSE");
					system("cls");
				}
				else if (choice == 2)
				{
					Shift a;
					std::string pathKey;
					std::string pathSave;
					std::string pathEncryptText;
					std::cout << "\npathKey >> "; std::cin >> pathKey;
					std::cout << "\npathEncryptText >> "; std::cin >> pathEncryptText;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Decrypt(pathKey, pathSave, pathEncryptText);
					system("PAUSE");
					system("cls");
				}
				else if (choice == 3)
				{
					Gamma a;
					std::string pathKey;
					std::string pathSave;
					std::string pathEncryptText;
					std::cout << "pathKey >> "; std::cin >> pathKey;
					std::cout << "\npathEncryptText >> "; std::cin >> pathEncryptText;
					std::cout << "\npathSave >> "; std::cin >> pathSave;
					a.Decrypt(pathKey, pathSave, pathEncryptText);
					system("PAUSE");
					system("cls");
				}
				else
				{
					std::cout << "\nIncorrect input!\n";
					system("PAUSE");
					system("cls");
				}
			}
			else
			{
				std::cout << "\nIncorrect input!\n";
				system("PAUSE");
				system("cls");
			}
		}
		else if (choice == 2)
		{
			choice = 0;
			std::cout << "\n----Menu->Generate key----\n";
			std::cout << "1. Replacement\n";
			std::cout << "2. Shift\n";
			std::cout << "3. Gamma\n";
			std::cout << ">>"; std::cin >> choice;
			if (choice == 1)
			{
				Replacement a;
				std::string pathAlph;
				std::string pathSave;
				std::cout << "\npathAlp >> "; std::cin >> pathAlph;
				std::cout << "\npathSave >> "; std::cin >> pathSave;
				a.KeyGenerator(pathAlph, pathSave);
				system("PAUSE");
				system("cls");
			}
			else if (choice == 2)
			{
				Shift a;
				std::string pathAlph;
				std::string pathSave;
				std::cout << "\npathAlp >> "; std::cin >> pathAlph;
				std::cout << "\npathSave >> "; std::cin >> pathSave;
				a.KeyGenerator(pathAlph, pathSave);
				system("PAUSE");
				system("cls");
			}
			else if (choice == 3)
			{
				Gamma a;
				std::string pathAlph;
				std::string pathSave;
				std::cout << "\npathAlp >> "; std::cin >> pathAlph;
				std::cout << "\npathSave >> "; std::cin >> pathSave;
				a.KeyGenerator(pathAlph, pathSave);
				system("PAUSE");
				system("cls");
			}
			else
			{
				std::cout << "\nIncorrect input!\n";
				system("PAUSE");
				system("cls");
			}
		}
		else
		{
			std::cout << "\nIncorrect input!\n";
			system("PAUSE");
			system("cls");
		}
	}
}