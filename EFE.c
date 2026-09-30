#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define DIMENTION 3
#define MOD 16
#define HEX_NUM_STR "0123456789ABCDEF"
#define PATH_MAX_LEN 50
#define BUF_SIZE 4096
typedef unsigned char hex_t;

char positions[DIMENTION] = {0};
const char ROTERS[DIMENTION][MOD] = {
	"3F9A0C7E1B5D2846",
	"A7C1E90B4D2F8365",
	"5E2B8D0F6A3C9147"
};
const char REFLECTOR[MOD] = "FEDCBA9876543210";

hex_t IDX_ROTERS[DIMENTION][MOD];
hex_t IDX_REFLECTOR[MOD];
hex_t REV_MAPS[DIMENTION][MOD];

int stridx(char str[], char c){
	const char *pos = strchr(str, c);
	if(pos != NULL){
		return pos - str;
	}
	else{
		return -1;
	}
}

void init(){
	// 初始化转子下标
	for(int i = 0; i < DIMENTION; i++){
		for(int j = 0; j < MOD; j++){
			IDX_ROTERS[i][j] = stridx(HEX_NUM_STR, ROTERS[i][j]);
		}
	}
	// 初始化反射器下标
	for(int i = 0; i < MOD; i++){
		IDX_REFLECTOR[i] = stridx(HEX_NUM_STR, REFLECTOR[i]);
	}
	// 初始化反向映射表
	for(int i = 0; i < DIMENTION; i++){
		for(int j = 0; j < MOD; j++){
			REV_MAPS[i][
				IDX_ROTERS[DIMENTION - i - 1][j]
			] = j;
		}
	}
}

void roll(){
	for(int i = 0; i < DIMENTION; i++){
		if(++positions[i] < MOD) break;
		positions[i] = 0;
	}
}

hex_t encrypt(hex_t data){
	if(data >= 0 && data <= 15){
		roll();
		for(int i = 0; i < DIMENTION; i++){
			data = IDX_ROTERS[i][
				(data - positions[i] + MOD) % MOD
			];
		}
		data = IDX_REFLECTOR[data];
		for(int i = 0; i < DIMENTION; i++){
			data = (REV_MAPS[i][data] + positions[DIMENTION - i - 1]) % MOD;
		}
		return data;
	}
	else{
		return 0;
	}
}

void encrypt_data(unsigned char* buf, int _count){
	for(int i = 0; i < _count; i++){
		buf[i] = (encrypt(buf[i] >> 4) << 4) + 
			encrypt(buf[i] & ((1U << 4) - 1));
	}
}

char* strlnk(char* s1, char* s2){
	int len = strlen(s1) + strlen(s2);
	if(len < PATH_MAX_LEN){
		static char s[PATH_MAX_LEN];
		int i = 0;
		while(s1[i] != '\0'){
			s[i] = s1[i];
			i++;
		}
		int j = 0;
		while(s2[j] != '\0'){
			s[i] = s2[j];
			i++; j++;
		}
		return s;
	}
	else{
		return NULL;
	}
}

void process_file(char* file_name){
	//打开指定文件，创建输出文件
	char *fout_name = strlnk(file_name, ".ngm");
	if(!fout_name){
		fprintf(stderr, "path exceeds the limit.\n");
		return;
	}
	FILE* fin = fopen(file_name, "rb");
	if(fin != NULL){
		FILE* fout = fopen(fout_name, "wb");
		if(fout != NULL){
			unsigned char buf[BUF_SIZE];
			int read_count;
			do{
				read_count = fread(buf, 1, BUF_SIZE, fin);
				encrypt_data(buf, read_count);
				fwrite(buf, 1, read_count, fout);

			}while(read_count == BUF_SIZE);





			fclose(fin);
			fclose(fout);
		}
		else{
			fprintf(stderr, "Fail to create file.\n");
			perror("error");
		}
	}
	else{
		fprintf(stderr, "Fail to open file.\n");
		perror("error");
	}
	return;
}

int main(int argc, char* argv[]){
	if(argc > 1){
		init();
		process_file(argv[1]);
	}
	else{
		fprintf(stderr, "No arguments.\n");
	}
	return 0;
}
