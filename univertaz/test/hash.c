//#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define HASHSIZE 255
static size_t hashlen;

typedef struct nm {
	uint32_t hash;
	char name[128];
	size_t len;
} Nm[256];

static Nm Hash;

void initHash(void) {
	memset(Hash, 0, sizeof(struct nm) * HASHSIZE);
	hashlen = 0;
}

uint32_t getHash(char * key){
	uint32_t i = 5381;
//	uint64_t i = 0;
	while (*key++)
		i = ((i << 5) + i) ^ (unsigned int)*key; //(djb2a 32)
//		i = *key + (i << 6) + (i << 16) - i; //(sdbm 64)
	return i;
}

//void initHash(void);
//uint32_t getHash(char *);
//char * searchHash(char *);

//int main() {
////	memset(Hash, 0, sizeof(struct nm) * HASHSIZE);
////	hashlen = 0;
//	initHash();
//	int i;
////	for(i = 0; i < 10; i++)
////		Hash[i] = (Nm *)calloc(1, sizeof(struct name));
//	while(1) {
//		char buf[128];
//		printf("%s", "Enter path to dir: ");
//		scanf("%127[^\n]%*c", buf);
//		searchHash(buf);	
//		for(i = 0; i < hashlen; i++)
//			printf("%u %s %zu\n", Hash[i].hash, Hash[i].name, Hash[i].len);
//		if(buf[0] == 'q')
//			break;
//	}
//}

char * searchHash(char *str) {
	int i;
	size_t len = strlen(str);
	Nm tmp;
	uint32_t hash = getHash(str);
	for(i = 0; i < hashlen; i++) {
		if(Hash[i].hash == hash && Hash[i].len == len) {
			memcpy(Hash[i].name, str, strlen(str));
			memcpy(&tmp, &Hash[i], sizeof(struct nm));
			for(; i > 0; i--) {
				memset(&Hash[i], 0, sizeof(struct nm));
				memcpy(&Hash[i], &Hash[i-1], sizeof(struct nm));
			}
			memcpy(&Hash[0], &tmp, sizeof(struct nm));
			return Hash[0].name;
		}
	}
	if(hashlen < HASHSIZE)
		hashlen++;
	for(i = hashlen; i > 0; i--) {
		memset(&Hash[i], 0, sizeof(struct nm));
		memcpy(&Hash[i], &Hash[i-1], sizeof(struct nm));
	}
	memset(&Hash[0], 0, sizeof(struct nm));
	Hash[0].hash = hash;
	memcpy(Hash[0].name, str, strlen(str));
	Hash[i].len = len;
	return Hash[0].name;
}
