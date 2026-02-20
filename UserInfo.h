#include "stdint.h"
#include "stdio.h"
#include <cstdlib>
#include <string>


#define MAX_USERNAME 256
#define MAX_PASSWORD 256


struct UserInfo
{
	char username[MAX_USERNAME];
	char password[MAX_PASSWORD];
};

UserInfo* HashTable[];

unsigned int hash(char* username, int TableSize)
{
	int length = strnlen(username, MAX_USERNAME);
	unsigned int hash_value = 0;
	for (int i = 0; i < length; i++)
	{
		hash_value += username[i];
		hash_value = (hash_value * username[i]) % TableSize;
	}
	return hash_value;
}

int InitHashTable(int TableSize)
{
	for (int i = 0; i < TableSize; i++)
	{
		HashTable[i] = NULL;
	}
}

void PrintTable(int TableSize)
{
	for (int i = 0; i < TableSize; i++)
	{
		if (HashTable[i] == NULL)
		{
			printf("\t%i\t---\n", i);
		}
		else
		{
			printf("\t%i\t%s\n", i, HashTable[i]->username);
		}
	}
}

bool HashTableInsert(UserInfo* info, int TableSize)
{
	if (info == NULL) return false;
	int index = hash(info->username, TableSize);
	if (HashTable[index] != NULL)
	{
		return false;
	}
	HashTable[index] = info;
	return true;
}