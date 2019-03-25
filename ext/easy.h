#ifndef RUBY_CURL_EASY_H
#define RUBY_CURL_EASY_H

#include <ruby.h>
#include <curl/curl.h>
#include <curl/easy.h>

typedef struct {
	CURL *cp;
} rb_curl;

extern VALUE rb_cEasy;

extern void Init_easy();

#endif
