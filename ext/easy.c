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
	struct curl_slist *curl_list_var = NULL;

	Data_Get_Struct(self, rb_curl_easy, rb_ch);

	switch (information) {
		case CURLINFO_EFFECTIVE_URL:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_EFFECTIVE_URL, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_RESPONSE_CODE:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_RESPONSE_CODE, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_HTTP_CONNECTCODE:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_HTTP_CONNECTCODE, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_HTTP_VERSION:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_HTTP_VERSION, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_FILETIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_FILETIME, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_TOTAL_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_TOTAL_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_NAMELOOKUP_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_NAMELOOKUP_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_CONNECT_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CONNECT_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_PRETRANSFER_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PRETRANSFER_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_STARTTRANSFER_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_STARTTRANSFER_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_REDIRECT_TIME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_REDIRECT_TIME, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_REDIRECT_COUNT:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_REDIRECT_COUNT, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_REDIRECT_URL:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_REDIRECT_URL, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_SIZE_UPLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SIZE_UPLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_SIZE_DOWNLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SIZE_DOWNLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_SPEED_UPLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SPEED_UPLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_SPEED_DOWNLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SPEED_DOWNLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_HEADER_SIZE:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_HEADER_SIZE, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_REQUEST_SIZE:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_REQUEST_SIZE, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_SSL_VERIFYRESULT:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SSL_VERIFYRESULT, &l_var) == CURLE_OK) {
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
		case CURLOPT_PUT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PUT, NUM2LONG(val));
			break;
		case CURLOPT_HTTPGET:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTPGET, NUM2LONG(val));
			break;
		case CURLOPT_UPLOAD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_UPLOAD, NUM2LONG(val));
			break;
		case CURLOPT_VERBOSE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_VERBOSE, NUM2LONG(val));
			break;
		case CURLOPT_POSTFIELDS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_POSTFIELDS, StringValueCStr(val));
			break;
		case CURLOPT_USERAGENT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_USERAGENT, StringValueCStr(val));
			break;
		case CURLOPT_USERPWD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_USERPWD, StringValueCStr(val));
			break;
		case CURLOPT_MAXFILESIZE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_MAXFILESIZE, NUM2LONG(val));
			break;
		case CURLOPT_NETRC_FILE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NETRC_FILE, StringValueCStr(val));
			break;
		case CURLOPT_TCP_NODELAY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TCP_NODELAY, NUM2LONG(val));
			break;
		case CURLOPT_COOKIELIST:
			curl_easy_setopt(rb_ch->ch, CURLOPT_COOKIELIST, StringValueCStr(val));
			break;
		case CURLOPT_IGNORE_CONTENT_LENGTH:
			curl_easy_setopt(rb_ch->ch, CURLOPT_IGNORE_CONTENT_LENGTH, NUM2LONG(val));
			break;
		case CURLOPT_CONNECT_ONLY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CONNECT_ONLY, NUM2LONG(val));
			break;
		case CURLOPT_LOCALPORT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_LOCALPORT, NUM2LONG(val));
			break;
		case CURLOPT_MAX_RECV_SPEED_LARGE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t) NUM2LL(val));
			break;
		case CURLOPT_MAX_SEND_SPEED_LARGE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_MAX_SEND_SPEED_LARGE, (curl_off_t) NUM2LL(val));
			break;
		case CURLOPT_SSL_SESSIONID_CACHE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSL_SESSIONID_CACHE, NUM2LONG(val));
			break;
		case CURLOPT_CONNECTTIMEOUT_MS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CONNECTTIMEOUT_MS, NUM2LONG(val));
			break;
		case CURLOPT_HTTP_CONTENT_DECODING:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTP_CONTENT_DECODING, NUM2LONG(val));
			break;
		case CURLOPT_HTTP_TRANSFER_DECODING:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTP_TRANSFER_DECODING, NUM2LONG(val));
			break;
		case CURLOPT_TIMEOUT_MS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TIMEOUT_MS, NUM2LONG(val));
			break;
		case CURLOPT_KRBLEVEL:
			/* Curl nullable string options */
			curl_easy_setopt(rb_ch->ch, CURLOPT_KRBLEVEL, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_ADDRESS_SCOPE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_ADDRESS_SCOPE, NUM2LONG(val));
			break;
		case CURLOPT_CRLFILE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CRLFILE, StringValueCStr(val));
			break;
		case CURLOPT_ISSUERCERT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_ISSUERCERT, StringValueCStr(val));
			break;
		case CURLOPT_KEYPASSWD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_KEYPASSWD, StringValueCStr(val));
			break;
		case CURLOPT_CERTINFO:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CERTINFO, NUM2LONG(val));
			break;
		case CURLOPT_PASSWORD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PASSWORD, StringValueCStr(val));
			break;
		case CURLOPT_USERNAME:
			curl_easy_setopt(rb_ch->ch, CURLOPT_USERNAME, StringValueCStr(val));
			break;
		case CURLOPT_NOPROXY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NOPROXY, StringValueCStr(val));
			break;
		case CURLOPT_SOCKS5_GSSAPI_NEC:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SOCKS5_GSSAPI_NEC, NUM2LONG(val));
			break;
		case CURLOPT_SOCKS5_GSSAPI_SERVICE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SOCKS5_GSSAPI_SERVICE, StringValueCStr(val));
			break;
		case CURLOPT_TLSAUTH_PASSWORD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TLSAUTH_PASSWORD, StringValueCStr(val));
			break;
		case CURLOPT_TLSAUTH_TYPE:
			/* parameter is a zero terminated string */
			curl_easy_setopt(rb_ch->ch, CURLOPT_TLSAUTH_PASSWORD, StringValueCStr(val));
			break;
		case CURLOPT_TLSAUTH_USERNAME:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TLSAUTH_USERNAME, StringValueCStr(val));
			break;
		case CURLOPT_GSSAPI_DELEGATION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_GSSAPI_DELEGATION, NUM2LONG(val));
			break;
		case CURLOPT_UNIX_SOCKET_PATH:
			/* Curl nullable string options */
			curl_easy_setopt(rb_ch->ch, CURLOPT_UNIX_SOCKET_PATH, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_PATH_AS_IS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PATH_AS_IS, NUM2LONG(val));
			break;
		case CURLOPT_PIPEWAIT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PIPEWAIT, NUM2LONG(val));
			break;
		case CURLOPT_HTTPAUTH:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTPAUTH, NUM2LONG(val));
			break;
		case CURLOPT_PROXYAUTH:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYAUTH, NUM2LONG(val));
			break;
		case CURLOPT_IPRESOLVE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_IPRESOLVE, NUM2LONG(val));
			break;
		case CURLOPT_POSTREDIR:
			curl_easy_setopt(rb_ch->ch, CURLOPT_POSTREDIR, NUM2LONG(val));
			break;
		default:
			rb_raise(rb_eTypeError, "Unsupported option.");
	}

	return Qtrue;
}

static VALUE rb_curl_easy_perform(VALUE self) {
	rb_curl_easy *rb_ch;

	Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_perform(rb_ch->ch);

	//XNOTE: CURLE_OK ? self : raise_error
	return self;
}

static VALUE rb_curl_easy_cleanup(VALUE self) {
	rb_curl_easy *rb_ch;

	Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_cleanup(rb_ch->ch);
	//XNOTE: CURLE_OK ? self : raise_error
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
