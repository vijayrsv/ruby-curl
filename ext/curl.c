#include "curl.h"

VALUE rb_curl_module;

void Init_curl() {
	VALUE rb_curl_option_module;

	rb_curl_module = rb_define_module("Curl");

	rb_curl_option_module = rb_define_module_under(rb_curl_module, "Option");

	RB_CURL_OPT_DEF_CONST("URL", CURLOPT_URL);
	RB_CURL_OPT_DEF_CONST("POST", CURLOPT_POST);
	RB_CURL_OPT_DEF_CONST("TIMEOUT", CURLOPT_TIMEOUT);

	init_easy();
}
