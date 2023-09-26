#ifndef BASE64_H_  
#define BASE64_H_  
namespace base {
	namespace base64 {
		void base64_encode(const char * pData, int len, char * pOut);
		void base64_decode(const char * pData, int len, char * pOut);
		int calc_base64_len(int data_len);
		int calc_data_len(const char * base64, int base64_len);
	}
}
#endif  