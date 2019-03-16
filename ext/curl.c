#include "curl.h"

VALUE mCurl;

void Init_curl() {
	mCurl = rb_define_module("Curl");

	RUBY_CURL_DEF_CONST(CURLOPT_URL);
	RUBY_CURL_DEF_CONST(CURLOPT_POST);
	RUBY_CURL_DEF_CONST(CURLOPT_TIMEOUT);
	RUBY_CURL_DEF_CONST(CURLOPT_PUT);

	init_easy();
}
