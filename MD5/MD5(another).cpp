#include <iostream>
#include <string>

#define A 0x67452301
#define B 0xEFCDAB89
#define C 0x98BADCFE
#define D 0x10325476

#define F(x,y,z) (((x)&(y))|((~(x))&(z)))
#define G(x,y,z) (((x)&(z))|((y)&(~(z))))
#define H(x,y,z) ((x)^(y)^(z))
#define I(x,y,z) ((y)^((x)|(~(z))))

#define LOOPLEFTSHIFT(x,n) (((x)<<(n))|((x)>>(32-(n))))

using namespace std;

const unsigned int t[64]={
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
        0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
		0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
		0x6b901122,0xfd987193,0xa679438e,0x49b40821,
		0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
		0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
		0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
		0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
		0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
		0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
		0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
		0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
		0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
		0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
        
const int ls[64]={
	    7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
		5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
        4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,
		23,6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};
		
int strLen;

unsigned int* ext(string str) {
	unsigned int i = 0;
	
	//64字节(512位)1组，源字符串加上8字节(64位)长度信息后应有多少组 
	unsigned int n = (str.length()+8)/64+1;
	
	//1组64字节即16个整型数据 
	unsigned int* bits = new unsigned int[n*16];
	strLen = n*16;
	for (i = 0; i < n*16; i++) bits[i] = 0;
	
	//将源字符串数据填充进数组，使得数据各位相等 （小端序） 
	for (i = 0; i < str.length(); i++) bits[i>>2] |= str[i] << ((i%4)*8);
	
	//源字符串填充完后在填充末尾处添1（小端序），0x80即1000 0000 
	bits[str.length()>>2] |= 0x80 << ((str.length()%4)*8);
	
	//长度数据为32位数据，直接填在数据末尾 （小端序） 
	bits[n*16-2] = str.length()*8;
	
	return bits;
}

string changeHex(int num) {
    int tmp;
    string hex = "0123456789abcdef";
    string tmpStr;
    string str = "";
    for(int i = 0; i < 4; i++) {
        tmpStr = "";
        tmp = ((num >> i*8) % 0x100) & 0xff;   //因小端序，故从高位开始处理 
        for (int j = 0; j < 2; j++) {
            tmpStr.insert(0,1,hex[tmp%16]);
            tmp /= 16;
        }
        str += tmpStr;
    }
    return str;
}

string md5(string src) {
	unsigned int atmp, btmp, ctmp, dtmp;
	unsigned int a = A, b = B, c = C, d = D;
	unsigned int* bits = ext(src); //将源数据扩展
	
	/*将每一组数据都使用64次计算处理
	并将计算后数据代入下一组继续运算
	第一组数据代入四个标准梦幻数*/ 
	for (int i = 0; i < strLen/16; i++) {
		atmp = a;
		btmp = b;
		ctmp = c;
		dtmp = d;
		unsigned int group[16];
		for (int j = 0; j < 16; j++) group[j] = bits[i*16+j];
		for (int k = 0; k < 64; k++) {
			int x, y;
			if (k < 16) {
				x = F(btmp,ctmp,dtmp);
				y = k;
			} else if (k < 32) {
				x = G(btmp,ctmp,dtmp);
				y = (5*k+1)%16;
			} else if (k < 48) {
				x = H(btmp,ctmp,dtmp);
				y = (3*k+5)%16;
			} else {
				x = I(btmp,ctmp,dtmp);
				y = (7*k)%16;
			}
			//64次公式实际上在不断循环左移abcd位置，故不写出64个公式而用循环+交换值代替 
			unsigned int tmp = dtmp;
			dtmp = ctmp;
			ctmp = btmp;
			btmp += LOOPLEFTSHIFT(atmp+x+group[y]+t[k],ls[k]); //循环左移 
			atmp = tmp;
		}
		a += atmp;
		b += btmp;
		c += ctmp;
		d += dtmp;
	}
	
	delete []bits;
	//将abcd的最终结果转换成16进制的字符串（各36位）拼接成128位数据 
	return changeHex(a).append(changeHex(b)).append(changeHex(c)).append(changeHex(d));
}

int main() {
	string s;
	cin >> s;
	cout << md5(s) << endl;
	return 0;
}
