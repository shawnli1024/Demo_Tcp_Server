#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
 
#include "MoBase64.h"

char base64_table[] = {
     'A','B','C','D','E','F','G','H','I','J',
     'K','L','M','N','O','P','Q','R','S','T',
     'U','V','W','X','Y','Z','a','b','c','d',
     'e','f','g','h','i','j','k','l','m','n',
     'o','p','q','r','s','t','u','v','w','x',
     'y','z','0','1','2','3','4','5','6','7',
     '8','9','+', '/', '\0'
};
 
void base64_map(char *in_block, int len) {
    for(int i = 0; i < len; ++i) {
        in_block[i] = base64_table[in_block[i]];
        //printf("%d %c",in_block[i], base64_table[in_block[i]]);
    }
    if(len % 4 == 3)
        in_block[len] = '=';
    else if(len % 4 == 2)
        in_block[len] = in_block[len+1] = '=';
    return ;
}
 
void base64_unmap(char *in_block) {
    int i;
    char *c;
 
    for(int i = 0; i < 4; ++i) {
        c = in_block + i;
 
        if(*c>='A' && *c<='Z') {
            *c -= 'A';
            continue;
        }
 
        if(*c>='a' && *c<='z') {
            *c -= 'a';
            *c += 26;
            continue;
        }
 
        if(*c == '+') {
            *c = 62;
            continue;
        }
 
        if(*c == '/') {
            *c = 63;
            continue;
        }
 
        if(*c == '=') {
            *c = 0;
            continue;
        }
 
        *c -= '0';
        *c += 52;
    }
}
 
int base64_encode(char *in, int inlen, char *out) {
    char *in_block;
    char *out_block;
    char temp[3];
 
    out_block = out;
    in_block = in;
 
    for(int i = 0; i < inlen; i += 3) {
        memset(temp, 0, 3);
        memcpy(temp, in_block, i + 3 < inlen ? 3 : inlen - i);
        memset(out_block, 0, 4);
 
        out_block[0] = (temp[0] >> 2) & 0x3f;
        out_block[1] = ((temp[0] << 4) & 0x30) | ((temp[1] >> 4) & 0x0f);
        out_block[2] = ((temp[1] << 2) & 0x3c) | ((temp[2] >> 6) & 0x03);
        out_block[3] = (temp[2]) & 0x3f;
        //printf("%.2x %.2x %.2x\n", temp[0], temp[1], temp[2]);
        //printf("%.2x %.2x %.2x %.2x\n", out_block[0], out_block[1], out_block[2], out_block[3]);
        out_block += 4;
        in_block += 3;
    }
 
    base64_map(out, ((inlen * 4) - 1) / 3 + 1);
}
 
int base64_decode(char *in, int inlen, char *out) {
    char *in_block;
    char *out_block;
    char temp[4];
 
    out_block = out;
    in_block = in;
 
    for(int i = 0; i < inlen; i += 4) {
        if(*in_block == '=')
            return 0;
        memcpy(temp, in_block, 4);
        memset(out_block, 0, 3);
        base64_unmap(temp);
 
        out_block[0] = ((temp[0]<<2) & 0xfc) | ((temp[1]>>4) & 3);
        out_block[1] = ((temp[1]<<4) & 0xf0) | ((temp[2]>>2) & 0xf);
        out_block[2] = ((temp[2]<<6) & 0xc0) | ((temp[3]   ) & 0x3f);
 
        out_block += 3;
        in_block +=4;
    }
    return 0;
}
 