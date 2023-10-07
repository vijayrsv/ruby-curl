#ifndef RUBY_CURL_MULTI_H
#define RUBY_CURL_MULTI_H

#include <ruby.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>

typedef struct {
	CURLM *cmh;
	VALUE self;
} rb_curl_multi;

extern VALUE rb_cMulti;

extern void Init_multi();

#endif
