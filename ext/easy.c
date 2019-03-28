#include "easy.h"

extern VALUE rb_mCurl;

static VALUE id_call;

VALUE rb_cEasy;

void rb_curl_mark(rb_curl_easy *rb_ch) {
}

void rb_curl_free(rb_curl_easy *rb_ch) {
  free(rb_ch);
}

static VALUE rb_curl_allocate(VALUE klass) {
  rb_curl_easy *rb_ch;
  rb_ch = ALLOC(rb_curl_easy);
  rb_ch->ch = NULL;
  return Data_Wrap_Struct(klass, rb_curl_mark, rb_curl_free, rb_ch);
}

static size_t rb_curl_write(char *data, size_t size, size_t nmemb, rb_curl_easy *rb_ch) {
	size_t length = size * nmemb;
	VALUE ret_val;

	ret_val = rb_funcall(rb_ch->rb_curl_easy_write_proc, id_call, 1, rb_str_new(data, length));

	switch (rb_type(ret_val)) {
		case T_FIXNUM:
			return FIX2LONG(ret_val);
		case T_BIGNUM:
			return NUM2LONG(ret_val);
		default:
			rb_raise(rb_eRuntimeError, "Callback should return the number of bytes read.");
	}
}

static size_t rb_curl_write_header(char *stream, size_t size, size_t nmemb, rb_curl_easy *rb_ch) {
	size_t length = size * nmemb;
	VALUE ret_val;

	ret_val = rb_funcall(rb_ch->rb_curl_easy_write_header_proc, id_call, 1, rb_str_new(stream, length));

	switch (rb_type(ret_val)) {
		case T_FIXNUM:
			return FIX2LONG(ret_val);
		case T_BIGNUM:
			return NUM2LONG(ret_val);
		default:
			rb_raise(rb_eRuntimeError, "Callback should return the number of bytes read.");
	}
}

static size_t rb_curl_read(char *stream, size_t size, size_t nmemb, rb_curl_easy *rb_ch) {
	size_t max_bytes = size * nmemb;
	VALUE ret_val;

	ret_val = rb_funcall(rb_ch->rb_curl_easy_read_proc, id_call, 1, INT2NUM(max_bytes));

	if (rb_type(ret_val) == T_STRING) {
		memcpy(stream, RSTRING_PTR(ret_val), RSTRING_LEN(ret_val));
		return RSTRING_LEN(ret_val);
	}
	else {
		rb_raise(rb_eRuntimeError, "Callback must return a string.");
	}
}

static void rb_curl_easy_set_defaults(rb_curl_easy *rb_ch) {
	rb_ch->rb_curl_easy_write_proc = Qnil;
	rb_ch->rb_curl_easy_write_header_proc = Qnil;
	rb_ch->rb_curl_easy_read_proc = Qnil;
}

static VALUE rb_curl_easy_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE url;
  rb_curl_easy *rb_ch;

  rb_scan_args(argc, argv, "01", &url);

  Data_Get_Struct(self, rb_curl_easy, rb_ch);

  rb_ch->ch = curl_easy_init();
  if (!rb_ch->ch) {
    rb_raise(rb_eRuntimeError, "Failed to initialize easy handle");
  }

	if (url != Qnil)
	curl_easy_setopt(rb_ch->ch, CURLOPT_URL, StringValueCStr(url));

	rb_curl_easy_set_defaults(rb_ch);

  return self;
}

static VALUE rb_curl_easy_getinfo(VALUE self, VALUE info) {
	rb_curl_easy *rb_ch;
	VALUE ret_val = Qnil;
	long information = NUM2LONG(info);

	char *s_var;
	long l_var;
	double d_var;

	Data_Get_Struct(self, rb_curl_easy, rb_ch);

	switch (information) {
		case CURLINFO_RESPONSE_CODE:
				if (curl_easy_getinfo(rb_ch->ch, CURLINFO_RESPONSE_CODE, &l_var) == CURLE_OK) {
					ret_val = INT2FIX(l_var);
				}
			break;
		default:
			rb_raise(rb_eTypeError, "Unsupported information.");
	}
	return ret_val;
}


static VALUE rb_curl_easy_setopt(VALUE self, VALUE opt, VALUE val) {
	rb_curl_easy *rb_ch;
	long option = NUM2LONG(opt);

	Data_Get_Struct(self, rb_curl_easy, rb_ch);

	switch (option) {
		case CURLOPT_URL:
			curl_easy_setopt(rb_ch->ch, CURLOPT_URL, StringValueCStr(val));
			break;
		case CURLOPT_POST:
			curl_easy_setopt(rb_ch->ch, CURLOPT_POST, NUM2LONG(val));
			break;
		case CURLOPT_TIMEOUT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TIMEOUT, NUM2LONG(val));
			break;
		case CURLOPT_FOLLOWLOCATION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_FOLLOWLOCATION, NUM2LONG(val));
			break;
		case CURLOPT_MAXREDIRS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_MAXREDIRS, NUM2LONG(val));
			break;
		case CURLOPT_WRITEFUNCTION:
			rb_ch->rb_curl_easy_write_proc = val;
			curl_easy_setopt(rb_ch->ch, CURLOPT_WRITEFUNCTION, rb_curl_write);
			curl_easy_setopt(rb_ch->ch, CURLOPT_WRITEDATA, rb_ch);
			break;
		case CURLOPT_HEADERFUNCTION:
			rb_ch->rb_curl_easy_write_header_proc = val;
			curl_easy_setopt(rb_ch->ch, CURLOPT_HEADERFUNCTION, rb_curl_write_header);
			curl_easy_setopt(rb_ch->ch, CURLOPT_HEADERDATA, rb_ch);
			break;
		case CURLOPT_READFUNCTION:
			rb_ch->rb_curl_easy_read_proc = val;
			curl_easy_setopt(rb_ch->ch, CURLOPT_READFUNCTION, rb_curl_read);
			curl_easy_setopt(rb_ch->ch, CURLOPT_READDATA, rb_ch);
			break;
		default:
			rb_raise(rb_eTypeError, "Unsupported option.");
	}

	return Qnil;
}

static VALUE rb_curl_easy_perform(VALUE self) {
  rb_curl_easy *rb_ch;

  Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_perform(rb_ch->ch);
  return self;
}

static VALUE rb_curl_easy_cleanup(VALUE self) {
  rb_curl_easy *rb_ch;

  Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_cleanup(rb_ch->ch);
  return self;
}

void Init_easy() {
	id_call = rb_intern("call");

	rb_cEasy = rb_define_class_under(rb_mCurl, "Easy", rb_cObject);
	rb_define_alloc_func(rb_cEasy, rb_curl_allocate);
	rb_define_method(rb_cEasy, "initialize", rb_curl_easy_initialize, -1);
	rb_define_method(rb_cEasy, "setopt", rb_curl_easy_setopt, 2);
	rb_define_method(rb_cEasy, "getinfo", rb_curl_easy_getinfo, 1);
	rb_define_method(rb_cEasy, "perform", rb_curl_easy_perform, 0);
	rb_define_method(rb_cEasy, "cleanup", rb_curl_easy_cleanup, 0);
}
