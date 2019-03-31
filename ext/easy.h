#ifndef RUBY_CURL_EASY_H
#define RUBY_CURL_EASY_H

#include <ruby.h>
#include <curl/curl.h>
#include <curl/easy.h>

typedef struct {
	CURL *ch;
	VALUE rb_curl_easy_write_proc;
	VALUE rb_curl_easy_write_header_proc;
	VALUE rb_curl_easy_read_proc;

	struct curl_slist * curl_httpheader_slist;
	struct curl_slist * curl_http200aliases_slist;
} rb_curl_easy;

extern VALUE rb_cEasy;

extern void Init_easy();

#endif
