#include "curl.h"

VALUE rb_mCurl;

void Init_curl() {
	VALUE rb_mOption;

	rb_mCurl   = rb_define_module("Curl");
	rb_mOption = rb_define_module_under(rb_mCurl, "Option");

	RB_DEFINE_M_OPT_CONST("URL", CURLOPT_URL);
	RB_DEFINE_M_OPT_CONST("POST", CURLOPT_POST);
	RB_DEFINE_M_OPT_CONST("TIMEOUT", CURLOPT_TIMEOUT);

	Init_easy();
}
