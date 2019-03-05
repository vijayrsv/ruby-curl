#ifndef RUBY_CURL_H
#define RUBY_CURL_H

#include <ruby.h>
#include <curl/curl.h>

#include "easy.h"

extern VALUE mCurl;

extern void Init_curl();
#endif
