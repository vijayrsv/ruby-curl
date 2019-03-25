#include "easy.h"
#include <curl/curl.h>
#include <curl/easy.h>

extern VALUE rb_curl_module;

VALUE rb_cEasy;

void rb_curl_mark(rb_curl *ch) {
}

void rb_curl_free(rb_curl *ch) {
  free(ch);
}

/*
 * Allocate space for a Curl::Easy instance.
 */
static VALUE rb_curl_allocate(VALUE klass) {
  rb_curl *ch;
  ch = ALLOC(rb_curl);
  ch->cp = NULL;
  return Data_Wrap_Struct(klass, rb_curl_mark, rb_curl_free, ch);
}

static VALUE rb_curl_easy_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE url;
  rb_curl *ch;

  rb_scan_args(argc, argv, "01", &url);

  Data_Get_Struct(self, rb_curl, ch);

  /* handler */
  ch->cp = curl_easy_init();
  if (!ch->cp) {
    rb_raise(rb_eRuntimeError, "Failed to initialize easy handle");
  }


	curl_easy_setopt(ch->cp, CURLOPT_URL, RSTRING_PTR(url));

  return self;
}

static VALUE rb_curl_easy_set_opt(VALUE self, VALUE opt, VALUE val) {
	rb_curl *ch;
	long option = NUM2LONG(opt);

	Data_Get_Struct(self, rb_curl, ch);

	switch (option) {
		case CURLOPT_URL:
			curl_easy_setopt(ch->cp, CURLOPT_URL, RSTRING_PTR(val));
			break;
		case CURLOPT_POST:
			curl_easy_setopt(ch->cp, CURLOPT_POST, NUM2LONG(val));
			break;
		case CURLOPT_TIMEOUT:
			curl_easy_setopt(ch->cp, CURLOPT_TIMEOUT, NUM2LONG(val));
			break;
		default:
			rb_raise(rb_eTypeError, "Unsupported option.");
	}

	return Qnil;
}

static VALUE rb_curl_easy_exec(VALUE self) {
  rb_curl *ch;
  Data_Get_Struct(self, rb_curl, ch);
	curl_easy_perform(ch->cp);
  return self;
}

static VALUE rb_curl_easy_close(VALUE self) {
  rb_curl *ch;
  Data_Get_Struct(self, rb_curl, ch);
	curl_easy_cleanup(ch->cp);
  return self;
}

void Init_easy() {
	rb_cEasy = rb_define_class_under(rb_mCurl, "Easy", rb_cObject);
	rb_define_alloc_func(rb_cEasy, rb_curl_allocate);
  rb_define_method(rb_cEasy, "initialize", rb_curl_easy_initialize, -1);
  rb_define_method(rb_cEasy, "set_opt", rb_curl_easy_set_opt, 2);
  rb_define_method(rb_cEasy, "exec", rb_curl_easy_exec, 0);
  rb_define_method(rb_cEasy, "close", rb_curl_easy_close, 0);
}
