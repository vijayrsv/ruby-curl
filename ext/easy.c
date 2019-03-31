#include "easy.h"

extern VALUE rb_mCurl;

static VALUE id_call;

VALUE rb_cEasy;

static inline struct curl_slist *rb_array_to_curl_slist(VALUE arr, struct curl_slist *slist) {
	long arr_len;
	VALUE arr_ele;

	if(TYPE(arr) == T_ARRAY) {
		arr_len = RARRAY_LEN(arr);
		curl_slist_free_all(slist);

		for (long i = 0; i < arr_len; i++) {
			arr_ele = RARRAY_AREF(arr, i);
			slist = curl_slist_append(slist, StringValueCStr(arr_ele));
			if (!slist) {
				//Handle Error here.
			}
		}
	}
	else {
		rb_raise(rb_eTypeError, "Array is required.");
	}

	return slist;
}

void rb_curl_mark(rb_curl_easy *rb_ch) {
	rb_gc_mark(rb_ch->rb_curl_easy_write_proc);
	rb_gc_mark(rb_ch->rb_curl_easy_write_header_proc);
	rb_gc_mark(rb_ch->rb_curl_easy_read_proc);
}

void rb_curl_free(rb_curl_easy *rb_ch) {
	curl_slist_free_all(rb_ch->curl_httpheader_slist);
	curl_slist_free_all(rb_ch->curl_http200aliases_slist);
	curl_slist_free_all(rb_ch->curl_hosts_slist);
	curl_slist_free_all(rb_ch->curl_headers_slist);
	free(rb_ch);
}

static VALUE rb_curl_easy_allocate(VALUE klass) {
	rb_curl_easy *rb_ch;
	rb_ch = ALLOC(rb_curl_easy);
	rb_ch->ch = NULL;
	rb_ch->rb_curl_easy_write_proc = Qnil;
	rb_ch->rb_curl_easy_write_header_proc = Qnil;
	rb_ch->rb_curl_easy_read_proc = Qnil;
	rb_ch->curl_httpheader_slist = NULL;
	rb_ch->curl_http200aliases_slist = NULL;
	rb_ch->curl_hosts_slist = NULL;
	rb_ch->curl_headers_slist = NULL;
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

static void rb_curl_create_certinfo(struct curl_certinfo *curl_certinfo_chain, VALUE *listcode) {
  int i;

  if (curl_certinfo_chain) {
    for (i=0; i < curl_certinfo_chain->num_of_certs; i++) {
      struct curl_slist *slist;

			for (slist = curl_certinfo_chain->certinfo[i]; slist; slist = slist->next) {
				rb_ary_push(listcode, rb_str_new2(slist->data))
			}
    }
  }
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
	struct curl_certinfo *curl_certinfo_chain = NULL;

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
		case CURLINFO_CERTINFO:
			ret_val = rb_ary_new()
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CERTINFO, &curl_certinfo_chain) == CURLE_OK) {
				rb_curl_create_certinfo(curl_certinfo_chain, ret_val);
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
		case CURLOPT_AUTOREFERER:
			curl_easy_setopt(rb_ch->ch, CURLOPT_AUTOREFERER, NUM2LONG(val));
			break;
		case CURLOPT_BUFFERSIZE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_BUFFERSIZE, NUM2LONG(val));
			break;
		case CURLOPT_CAINFO:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CAINFO, StringValueCStr(val));
			break;
		case CURLOPT_CAPATH:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CAPATH,  NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_CONNECTTIMEOUT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CONNECTTIMEOUT, NUM2LONG(val));
			break;
		case CURLOPT_COOKIE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_COOKIE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_COOKIEFILE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_COOKIEFILE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_COOKIEJAR:
			curl_easy_setopt(rb_ch->ch, CURLOPT_COOKIEJAR, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_COOKIESESSION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_COOKIESESSION, NUM2LONG(val));
			break;
		case CURLOPT_CRLF:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CRLF, NUM2LONG(val));
			break;
		case CURLOPT_CUSTOMREQUEST:
			curl_easy_setopt(rb_ch->ch, CURLOPT_CUSTOMREQUEST, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_DNS_CACHE_TIMEOUT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_DNS_CACHE_TIMEOUT, NUM2LONG(val));
			break;
		case CURLOPT_DNS_USE_GLOBAL_CACHE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_DNS_USE_GLOBAL_CACHE, NUM2LONG(val));
			break;
		case CURLOPT_EGDSOCKET:
			curl_easy_setopt(rb_ch->ch, CURLOPT_EGDSOCKET, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_ENCODING:
			curl_easy_setopt(rb_ch->ch, CURLOPT_ENCODING, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_FAILONERROR:
			curl_easy_setopt(rb_ch->ch, CURLOPT_FAILONERROR, NUM2LONG(val));
			break;
		case CURLOPT_FILETIME:
			curl_easy_setopt(rb_ch->ch, CURLOPT_FILETIME, NUM2LONG(val));
			break;
		case CURLOPT_FORBID_REUSE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_FORBID_REUSE, NUM2LONG(val));
			break;
		case CURLOPT_FRESH_CONNECT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_FRESH_CONNECT, NUM2LONG(val));
			break;
		case CURLOPT_HEADER:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HEADER, NUM2LONG(val));
			break;
		case CURLOPT_HTTP200ALIASES:
			rb_ch->curl_http200aliases_slist = rb_array_to_curl_slist(val, rb_ch->curl_http200aliases_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTP200ALIASES, rb_ch->curl_http200aliases_slist);
			break;
		case CURLOPT_HTTPHEADER:
			rb_ch->curl_httpheader_slist = rb_array_to_curl_slist(val, rb_ch->curl_httpheader_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTPHEADER, rb_ch->curl_httpheader_slist);
			break;
		case CURLOPT_HTTPPROXYTUNNEL:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTPPROXYTUNNEL, NUM2LONG(val));
			break;
		case CURLOPT_HTTP_VERSION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTP_VERSION, NUM2LONG(val));
			break;
		case CURLOPT_INFILESIZE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_INFILESIZE, NUM2LONG(val));
			break;
		case CURLOPT_INTERFACE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_INTERFACE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_LOW_SPEED_LIMIT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_LOW_SPEED_LIMIT, NUM2LONG(val));
			break;
		case CURLOPT_LOW_SPEED_TIME:
			curl_easy_setopt(rb_ch->ch, CURLOPT_LOW_SPEED_TIME, NUM2LONG(val));
			break;
		case CURLOPT_MAXCONNECTS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_MAXCONNECTS, NUM2LONG(val));
			break;
		case CURLOPT_NETRC:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NETRC, NUM2LONG(val));
			break;
		case CURLOPT_NOBODY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NOBODY, NUM2LONG(val));
			break;
		case CURLOPT_NOPROGRESS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NOPROGRESS, NUM2LONG(val));
			break;
		case CURLOPT_NOSIGNAL:
			curl_easy_setopt(rb_ch->ch, CURLOPT_NOSIGNAL, NUM2LONG(val));
			break;
		case CURLOPT_PORT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PORT, NUM2LONG(val));
			break;
		case CURLOPT_PROXY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXY, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_PROXYPORT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYPORT, NUM2LONG(val));
			break;
		case CURLOPT_PROXYTYPE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYTYPE, NUM2LONG(val));
			break;
		case CURLOPT_PROXYUSERPWD:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYUSERPWD, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_RANDOM_FILE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_RANDOM_FILE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_RANGE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_RANGE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_REFERER:
			curl_easy_setopt(rb_ch->ch, CURLOPT_REFERER, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_RESUME_FROM:
			curl_easy_setopt(rb_ch->ch, CURLOPT_RESUME_FROM, NUM2LONG(val));
			break;
		case CURLOPT_SSLCERT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLCERT, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_SSLCERTTYPE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLCERTTYPE, StringValueCStr(val));
			break;
		case CURLOPT_SSLENGINE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLENGINE, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_SSLENGINE_DEFAULT:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLENGINE_DEFAULT, NUM2LONG(val));
			break;
		case CURLOPT_SSLKEY:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLKEY, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_SSLKEYTYPE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLKEYTYPE, StringValueCStr(val));
			break;
		case CURLOPT_SSLVERSION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSLVERSION, NUM2LONG(val));
			break;
		case CURLOPT_SSL_CIPHER_LIST:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSL_CIPHER_LIST, NIL_P(val) ? NULL : StringValueCStr(val));
			break;
		case CURLOPT_SSL_VERIFYHOST:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSL_VERIFYHOST, NUM2LONG(val));
			break;
		case CURLOPT_SSL_VERIFYPEER:
			curl_easy_setopt(rb_ch->ch, CURLOPT_SSL_VERIFYPEER, NUM2LONG(val));
			break;
		case CURLOPT_TIMECONDITION:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TIMECONDITION, NUM2LONG(val));
			break;
		case CURLOPT_TIMEVALUE:
			curl_easy_setopt(rb_ch->ch, CURLOPT_TIMEVALUE, NUM2LONG(val));
			break;
		case CURLOPT_UNRESTRICTED_AUTH:
			curl_easy_setopt(rb_ch->ch, CURLOPT_UNRESTRICTED_AUTH, NUM2LONG(val));
			break;
		case CURLOPT_PROTOCOLS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROTOCOLS, NUM2LONG(val));
			break;
		case CURLOPT_REDIR_PROTOCOLS:
			curl_easy_setopt(rb_ch->ch, CURLOPT_REDIR_PROTOCOLS, NUM2LONG(val));
			break;
		case CURLOPT_RESOLVE:
			rb_ch->curl_hosts_slist = rb_array_to_curl_slist(val, rb_ch->curl_hosts_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_RESOLVE, rb_ch->curl_hosts_slist);
			break;
		case CURLOPT_PROXYHEADER:
			rb_ch->curl_headers_slist = rb_array_to_curl_slist(val, rb_ch->curl_headers_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYHEADER, rb_ch->curl_headers_slist);
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

	//CURLE_OK ? self : raise_error
	return self;
}

static VALUE rb_curl_easy_cleanup(VALUE self) {
	rb_curl_easy *rb_ch;

	Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_cleanup(rb_ch->ch);
	// CURLE_OK ? self : raise_error
	return self;
}

void Init_easy() {
	id_call = rb_intern("call");

	rb_cEasy = rb_define_class_under(rb_mCurl, "Easy", rb_cObject);
	rb_define_alloc_func(rb_cEasy, rb_curl_easy_allocate);
	rb_define_method(rb_cEasy, "initialize", rb_curl_easy_initialize, -1);
	rb_define_method(rb_cEasy, "setopt", rb_curl_easy_setopt, 2);
	rb_define_method(rb_cEasy, "getinfo", rb_curl_easy_getinfo, 1);
	rb_define_method(rb_cEasy, "perform", rb_curl_easy_perform, 0);
	rb_define_method(rb_cEasy, "cleanup", rb_curl_easy_cleanup, 0);
}
