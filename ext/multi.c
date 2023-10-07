#include "multi.h"

extern VALUE rb_mCurl;

VALUE rb_cMulti;

static VALUE rb_curl_multi_strerror(VALUE self, VALUE errornum) {
	const char *error_str;

	error_str = curl_multi_strerror(NUM2LONG(errornum));

	return rb_str_new2(error_str);
}

void rb_curl_multi_mark(rb_curl_multi *rb_cmh) {
	rb_gc_mark(rb_cmh->self);
}

void rb_curl_multi_free(rb_curl_multi *rb_cmh) {
	free(rb_cmh);
}

static void rb_curl_multi_zero_state(rb_curl_multi *rb_cmh) {
	rb_cmh->cmh = NULL;
}

static VALUE rb_curl_multi_allocate(VALUE klass) {
	rb_curl_multi *rb_cmh;

	rb_cmh = ALLOC(rb_curl_multi);
	rb_curl_multi_zero_state(rb_cmh);

	return Data_Wrap_Struct(klass, rb_curl_multi_mark, rb_curl_multi_free, rb_cmh);
}

static VALUE rb_curl_multi_initialize(int argc, VALUE *argv, VALUE self) {
	rb_curl_multi *rb_cmh;

	Data_Get_Struct(self, rb_curl_multi, rb_cmh);

	rb_cmh->cmh = curl_multi_init();
	if (!rb_cmh->cmh) {
		rb_raise(rb_eRuntimeError, "Failed to initialize multi handle");
	}

	rb_cmh->self = self;

	return self;
}

void Init_multi() {
	rb_cMulti = rb_define_class_under(rb_mCurl, "Multi", rb_cObject);
	rb_define_alloc_func(rb_cMulti, rb_curl_multi_allocate);
	rb_define_singleton_method(rb_cMulti, "strerror", rb_curl_multi_strerror, 1);
	rb_define_method(rb_cMulti, "initialize", rb_curl_multi_initialize, -1);
	rb_define_method(rb_cMulti, "setopt", rb_curl_multi_setopt, 2);
	rb_define_method(rb_cMulti, "add_handle", rb_curl_multi_add_handle, 1);
	rb_define_method(rb_cMulti, "remove_handle", rb_curl_multi_remove_handle, 1);
	rb_define_method(rb_cMulti, "get_handles", rb_curl_multi_get_handles, 0);
	rb_define_method(rb_cMulti, "perform", rb_curl_multi_perform, 0);
	rb_define_method(rb_cMulti, "cleanup", rb_curl_multi_cleanup, 0);
}
