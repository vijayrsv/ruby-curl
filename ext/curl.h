#ifndef RUBY_CURL_H
#define RUBY_CURL_H

#include <ruby.h>
#include <curl/curl.h>

#define RB_DEFINE_M_OPT_CONST(name, curl_opt_name) \
	rb_define_const(rb_mOption, name, LONG2NUM(curl_opt_name))

extern VALUE rb_mCurl;

void Init_curl();

#endif
