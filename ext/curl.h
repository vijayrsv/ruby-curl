#ifndef RUBY_CURL_H
#define RUBY_CURL_H

#include <ruby.h>
#include <curl/curl.h>

#include "easy.h"

#define RUBY_CURL_DEF_CONST(name) \
	rb_define_const(mCurl, #name, LONG2NUM(name))

extern VALUE mCurl;

extern void Init_curl();
#endif
