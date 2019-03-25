#ifndef RUBY_CURL_EASY_H
#define RUBY_CURL_EASY_H

#include "curl.h"
#include <curl/easy.h>

typedef struct {
	CURL *cp;
} ruby_curl;

extern VALUE rb_curl_easy_class;

extern void init_easy();

#endif
