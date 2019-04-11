#ifndef RUBY_CURL_EASY_H
#define RUBY_CURL_EASY_H

#include <ruby.h>
#include <curl/curl.h>
#include <curl/easy.h>

typedef struct {
	CURL *ch;
	VALUE self;

	char *write_function;
	char *write_header_function;
	char *read_function;

	VALUE write_data;
	VALUE write_header_data;
	VALUE read_data;

	struct curl_slist *curl_httpheader_slist;
	struct curl_slist *curl_http200aliases_slist;
	struct curl_slist *curl_hosts_slist;
	struct curl_slist *curl_headers_slist;
} rb_curl_easy;

extern VALUE rb_cEasy;

extern void Init_easy();

#endif
