#include "curl.h"

VALUE rb_mCurl;

void Init_curl() {
	VALUE rb_mOption, rb_mInfo;

	rb_mCurl   = rb_define_module("Curl");
	rb_mOption = rb_define_module_under(rb_mCurl, "Option");
	rb_mInfo = rb_define_module_under(rb_mCurl, "Info");

	RB_DEFINE_M_OPT_CONST("URL", CURLOPT_URL);
	RB_DEFINE_M_OPT_CONST("POST", CURLOPT_POST);
	RB_DEFINE_M_OPT_CONST("TIMEOUT", CURLOPT_TIMEOUT);
	RB_DEFINE_M_OPT_CONST("WRITEFUNCTION", CURLOPT_WRITEFUNCTION);
	RB_DEFINE_M_OPT_CONST("HEADERFUNCTION", CURLOPT_HEADERFUNCTION);
	RB_DEFINE_M_OPT_CONST("READFUNCTION", CURLOPT_READFUNCTION);
	RB_DEFINE_M_OPT_CONST("FOLLOWLOCATION", CURLOPT_FOLLOWLOCATION);
	RB_DEFINE_M_OPT_CONST("MAXREDIRS", CURLOPT_MAXREDIRS);

	RB_DEFINE_M_INFO_CONST("RESPONSE_CODE", CURLINFO_RESPONSE_CODE);

	Init_easy();
}
