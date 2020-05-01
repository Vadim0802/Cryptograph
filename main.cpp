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
		if ((pathTextFile.find(".txt") != -1) && (pathKey.find(".key") != -1) && (pathSave.find(".encrypt") != -1))
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
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1))
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
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1))
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

class Gamma : ICiphers
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
				json Key = { {"Alg_Type", "Gamma"}, {"Key", {}} };
				std::string alpToString;
				json alpData = alp.at("alp");
				json::iterator it = alpData.begin();
				for (int i = 0; i < alp.at("alp").size(); i++)
				{
					std::string tmp = *it;
					alpToString.push_back(tmp[0]);
					it++;
				}
				std::string KeyGamma = "";
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
							if (alpToString[tmp_key] == KeyGamma[j])
							{
								isTrueRandom = false;
								break;
							}
						}
						if (isTrueRandom)
							break;
					}
					KeyGamma.push_back(alpToString[tmp_key]);
				}
				for (int i = 0; i < alpToString.size(); i++)
				{
					std::string tmp_alpToString;
					std::string tmp_KeyReplacement;
					tmp_alpToString.push_back(alpToString[i]);
					tmp_KeyReplacement.push_back(KeyGamma[i]);
					Key.at("Key").push_back(json::array({ tmp_alpToString , tmp_KeyReplacement }));
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
			std::ifstream getKey(pathKey);
			getKey >> key;
			getKey.close();
			if (key.at("Alg_Type") == "Gamma")
			{
				std::ifstream getText(pathTextFile);
				std::string text;
				while (!getText.eof())
				{
					std::getline(getText, text);
				}
				getText.close();
				for (int i = 0; i < text.size(); i++)
				{
					bool isSymbolFromAlph = false;
					std::string symbol = "";
					symbol.push_back(text[i]);
					int positionSymbol;
					for (int j = 0; j < key.at("Key").size(); j++)
					{
						if (symbol == key.at("Key").at(j).at(0))
						{
							positionSymbol = j;
							isSymbolFromAlph = true;
							break;
						}
					}
					if (isSymbolFromAlph)
					{
						int positionKey;
						if (i >= key.at("Key").size())
						{
							int _tmp = i % key.at("Key").size();
							std::string symbolKey = key.at("Key").at(_tmp).at(1);
							for (int j = 0; j < key.at("Key").size(); j++)
							{
								if (symbolKey == key.at("Key").at(j).at(0))
								{
									positionKey = j;
									break;
								}
							}
						}
						else
						{
							std::string symbolKey = key.at("Key").at(i).at(1);
							for (int j = 0; j < key.at("Key").size(); j++)
							{
								if (symbolKey == key.at("Key").at(j).at(0))
								{
									positionKey = j;
									break;
								}
							}
						}
						int resultPosition = (positionSymbol + positionKey) % key.at("Key").size();
						std::string resultCrypto = key.at("Key").at(resultPosition).at(0);
						text[i] = resultCrypto[0];
					}
					json resultEncrypt = { {"Alg_Type", "Gamma"}, {"TextEncrypt", text} };
					std::ofstream inputFile(pathSave);
					inputFile << resultEncrypt;
					inputFile.close();
				}
			}
			else
			{
				std::cout << "Error" << std::endl;
			}
		}
		else
		{
			std::cout << "Error" << std::endl;
		}
	}
	void Decrypt(std::string pathKey, std::string PathSave, std::string pathEncryptText)
	{
		if ((pathEncryptText.find(".encrypt") != -1) && (pathKey.find(".key") != -1) && (PathSave.find(".decrypt.txt") != -1))
		{
			json Encrypt;
			std::ifstream readFile(pathEncryptText);
			readFile >> Encrypt;
			json Key;
			std::ifstream getKey(pathKey);
			getKey >> Key;
			if (Encrypt.at("Alg_Type") == "Gamma" && Key.at("Alg_Type") == "Gamma")
			{
				std::string text = Encrypt.at("TextEncrypt");
				for (int i = 0; i < text.size(); i++)
				{
					bool isSymbolFromAlp = false;
					std::string symbol = "";
					symbol.push_back(text[i]);
					int positionSymbol;
					for (int j = 0; j < Key.at("Key").size(); j++)
					{
						if (symbol == Key.at("Key").at(j).at(0))
						{
							positionSymbol = j;
							isSymbolFromAlp = true;
							break;
						}
					}
					if (isSymbolFromAlp)
					{
						int positionKey;
						if (i >= Key.at("Key").size())
						{
							int _tmp = i % Key.at("Key").size();
							std::string symbolKey = Key.at("Key").at(_tmp).at(1);
							for (int j = 0; j < Key.at("Key").size(); j++)
							{
								if (symbolKey == Key.at("Key").at(j).at(0))
								{
									positionKey = j;
									break;
								}
							}
						}
						else
						{
							std::string symbolKey = Key.at("Key").at(i).at(1);
							for (int j = 0; j < Key.at("Key").size(); j++)
							{
								if (symbolKey == Key.at("Key").at(j).at(0))
								{
									positionKey = j;
									break;
								}
							}
						}
						int resultPosition = (positionSymbol - positionKey + Key.at("Key").size()) % Key.at("Key").size();
						std::string resultCrypto = Key.at("Key").at(resultPosition).at(0);
						text[i] = resultCrypto[0];
					}
				}
				std::ofstream inputFile(PathSave);
				inputFile << text;
				inputFile.close();
			}
			else
			{
				std::cout << "Error" << std::endl;
			}
		}
		else
		{
			std::cout << "Error" << std::endl;
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