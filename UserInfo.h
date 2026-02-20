#include "stdint.h"
#include "stdio.h"
#include <cstdlib>
#include <string>


#define MAX_USERNAME 256
#define MAX_PASSWORD 256
#define TABLE_SIZE 10

struct UserInfo
{
	char username[MAX_USERNAME];
	char password[MAX_PASSWORD];
};

static UserInfo* HashTable[TABLE_SIZE];

inline unsigned int hash(char* username)
{
	int length = strnlen(username, MAX_USERNAME);
	unsigned int hash_value = 0;
	for (int i = 0; i < length; i++)
	{
		hash_value += username[i];
		hash_value = (hash_value * username[i]) % TABLE_SIZE;
	}
	return hash_value;
}


inline void InitHashTable()
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		HashTable[i] = NULL;
	}
}

inline void PrintTable(int TableSize)
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

inline bool HashTableInsert(UserInfo* info)
{
	if (info == NULL) return false;
	int index = hash(info->username);
	if (HashTable[index] != NULL)
	{
		return false;
	}
	HashTable[index] = info;
	return true;
}