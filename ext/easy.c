#include "easy.h"
#include <curl/curl.h>
#include <curl/easy.h>

extern VALUE mCurl;

VALUE cCurlEasy;

void ruby_curl_mark(ruby_curl *rb_curl) {
}

void ruby_curl_free(ruby_curl *rb_curl) {
  free(rb_curl);
}

/*
 * Allocate space for a Curl::Easy instance.
 */
static VALUE ruby_curl_allocate(VALUE klass) {
  ruby_curl *rb_curl;
  rb_curl = ALLOC(ruby_curl);
  rb_curl->cp = NULL;
  return Data_Wrap_Struct(klass, ruby_curl_mark, ruby_curl_free, rb_curl);
}

static VALUE ruby_curl_easy_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE url;
  ruby_curl *rb_curl;

  rb_scan_args(argc, argv, "01", &url);

  Data_Get_Struct(self, ruby_curl, rb_curl);

  /* handler */
  rb_curl->cp = curl_easy_init();
  if (!rb_curl->cp) {
    rb_raise(rb_eRuntimeError, "Failed to initialize easy handle");
  }


	curl_easy_setopt(rb_curl->cp, CURLOPT_URL, RSTRING_PTR(url));

  return self;
}

static VALUE ruby_curl_easy_set_opt(VALUE self, VALUE opt, VALUE val) {
	ruby_curl *rb_curl;
	long option = NUM2LONG(opt);

	Data_Get_Struct(self, ruby_curl, rb_curl);

	switch (option) {
		case CURLOPT_URL:
			curl_easy_setopt(rb_curl->cp, CURLOPT_URL, RSTRING_PTR(val));
			break;
		case CURLOPT_POST:
			curl_easy_setopt(rb_curl->cp, CURLOPT_POST, NUM2LONG(val));
			break;
		case CURLOPT_TIMEOUT:
			curl_easy_setopt(rb_curl->cp, CURLOPT_TIMEOUT, NUM2LONG(val));
			break;
		default:
			rb_raise(rb_eTypeError, "Unsupported option.");
	}

	return Qnil;
}

static VALUE ruby_curl_easy_exec(VALUE self) {
  ruby_curl *rb_curl;
  Data_Get_Struct(self, ruby_curl, rb_curl);
	curl_easy_perform(rb_curl->cp);
  return self;
}

static VALUE ruby_curl_easy_close(VALUE self) {
  ruby_curl *rb_curl;
  Data_Get_Struct(self, ruby_curl, rb_curl);
	curl_easy_cleanup(rb_curl->cp);
  return self;
}

void init_easy() {
	cCurlEasy = rb_define_class_under(mCurl, "Easy", rb_cObject);
	rb_define_alloc_func(cCurlEasy, ruby_curl_allocate);
  rb_define_method(cCurlEasy, "initialize", ruby_curl_easy_initialize, -1);
  rb_define_method(cCurlEasy, "set_opt", ruby_curl_easy_set_opt, 2);
  rb_define_method(cCurlEasy, "exec", ruby_curl_easy_exec, 0);
  rb_define_method(cCurlEasy, "close", ruby_curl_easy_close, 0);
}
