#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int sign;
int i,j,length;
int vinegere_cipher(char* text,char* key,int mode){
	sign = (mode) ? 1 : -1;
	for(i = 0, j = 0, length = strlen(text); i < length; i++, j++)
    {
		if (j >= strlen(key))
        {
            j = 0;
        }
        if (!isalpha(text[i]))
        {
            j = (j - 1);
        } else{
			text[i] = 'A' + (text[i] - 'A') + sign * (key[j] - 'A');
		}
	}
	
	return 0;
}

int main(){
	
	int fd;
	char text[100],key[100];
	
	printf("Key?: ");
	scanf("%s",key);
	
	printf("Text?: ");
	scanf("%s",text);
	
	vinegere_cipher(text,key,0);
	
	printf("Cipher text= %s\n",text);
	
	return 0;
}
