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

static inline VALUE curl_slist_to_rb_array(struct curl_slist *slist) {
	VALUE arr = rb_ary_new();
	while (slist) {
		rb_ary_push(arr, rb_str_new_cstr(slist->data));
		slist = slist->next;
	}
	return arr;
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

static void rb_curl_zero_state(rb_curl_easy *rb_ch) {
	rb_ch->ch = NULL;
	rb_ch->rb_curl_easy_write_proc = Qnil;
	rb_ch->rb_curl_easy_write_header_proc = Qnil;
	rb_ch->rb_curl_easy_read_proc = Qnil;
	rb_ch->curl_httpheader_slist = NULL;
	rb_ch->curl_http200aliases_slist = NULL;
	rb_ch->curl_hosts_slist = NULL;
	rb_ch->curl_headers_slist = NULL;
}

static VALUE rb_curl_easy_allocate(VALUE klass) {
	rb_curl_easy *rb_ch;
	rb_ch = ALLOC(rb_curl_easy);
	rb_curl_zero_state(rb_ch);
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

static VALUE rb_curl_create_certinfo(struct curl_certinfo *curl_certinfo_chain) {
  int i;
	VALUE listcode = rb_ary_new();

  if (curl_certinfo_chain) {
    for (i=0; i < curl_certinfo_chain->num_of_certs; i++) {
      struct curl_slist *slist;

			for (slist = curl_certinfo_chain->certinfo[i]; slist; slist = slist->next) {
				rb_ary_push(listcode, rb_str_new2(slist->data));
			}
    }
  }

	return listcode;
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
		case CURLINFO_PROXY_SSL_VERIFYRESULT:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PROXY_SSL_VERIFYRESULT, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_CONTENT_TYPE:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CONTENT_TYPE, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_HTTPAUTH_AVAIL:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_HTTPAUTH_AVAIL, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_PROXYAUTH_AVAIL:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PROXYAUTH_AVAIL, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_OS_ERRNO:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_OS_ERRNO, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_NUM_CONNECTS:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_NUM_CONNECTS, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_PRIMARY_IP:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PRIMARY_IP, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_PRIMARY_PORT:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PRIMARY_PORT, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_LOCAL_IP:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_LOCAL_IP, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_LOCAL_PORT:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_LOCAL_PORT, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_LASTSOCKET:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_LASTSOCKET, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_PROTOCOL:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_PROTOCOL, &l_var) == CURLE_OK) {
				ret_val = INT2FIX(l_var);
			}
			break;
		case CURLINFO_SCHEME:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_SCHEME, &s_var) == CURLE_OK) {
				ret_val = rb_str_new2(s_var);
			}
			break;
		case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_CONTENT_LENGTH_UPLOAD:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CONTENT_LENGTH_UPLOAD, &d_var) == CURLE_OK) {
				ret_val = DBL2NUM(d_var);
			}
			break;
		case CURLINFO_CERTINFO:
			if (curl_easy_getinfo(rb_ch->ch, CURLINFO_CERTINFO, &curl_certinfo_chain) == CURLE_OK) {
				ret_val = rb_curl_create_certinfo(curl_certinfo_chain);
			}
			break;
		case CURLINFO_SSL_ENGINES:
		case CURLINFO_COOKIELIST:
			if (curl_easy_getinfo(rb_ch->ch, information, &curl_list_var) == CURLE_OK) {
				ret_val = curl_slist_to_rb_array(curl_list_var);
				curl_slist_free_all(curl_list_var);
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
		case CURLOPT_TIMEOUT:
		case CURLOPT_FOLLOWLOCATION:
		case CURLOPT_MAXREDIRS:
		case CURLOPT_POST:
		case CURLOPT_PUT:
		case CURLOPT_HTTPGET:
		case CURLOPT_UPLOAD:
		case CURLOPT_VERBOSE:
#if LIBCURL_VERSION_NUM >= 0x070b02 /* Available since 7.11.2 */
		case CURLOPT_TCP_NODELAY:
#endif
#if LIBCURL_VERSION_NUM >= 0x070e01 /* Available since 7.14.1 */
		case CURLOPT_IGNORE_CONTENT_LENGTH:
#endif
#if LIBCURL_VERSION_NUM >= 0x070f02 /* Available since 7.15.2 */
		case CURLOPT_CONNECT_ONLY:
		case CURLOPT_LOCALPORT:
#endif
#if LIBCURL_VERSION_NUM >= 0x071000 /* Available since 7.16.0 */
		case CURLOPT_SSL_SESSIONID_CACHE:
#endif
#if LIBCURL_VERSION_NUM >= 0x071002 /* Available since 7.16.2 */
		case CURLOPT_CONNECTTIMEOUT_MS:
		case CURLOPT_HTTP_CONTENT_DECODING:
		case CURLOPT_HTTP_TRANSFER_DECODING:
		case CURLOPT_TIMEOUT_MS:
#endif
#if LIBCURL_VERSION_NUM >= 0x071300 /* Available since 7.19.0 */
		case CURLOPT_ADDRESS_SCOPE:
#endif
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
		case CURLOPT_CERTINFO:
		case CURLOPT_POSTREDIR:
#endif
#if LIBCURL_VERSION_NUM >= 0x071600 /* Available since 7.22.0 */
		case CURLOPT_GSSAPI_DELEGATION:
#endif
#if LIBCURL_VERSION_NUM >= 0x072a00 /* Available since 7.42.0 */
		case CURLOPT_PATH_AS_IS:
#endif
#if LIBCURL_VERSION_NUM >= 0x072b00 /* Available since 7.43.0 */
		case CURLOPT_PIPEWAIT:
#endif
#if LIBCURL_VERSION_NUM >= 0x070a06 /* Available since 7.10.6 */
		case CURLOPT_HTTPAUTH:
#endif
#if LIBCURL_VERSION_NUM >= 0x070a07 /* Available since 7.10.7 */
		case CURLOPT_PROXYAUTH:
#endif
#if LIBCURL_VERSION_NUM >= 0x070a08 /* Available since 7.10.8 */
		case CURLOPT_IPRESOLVE:
		case CURLOPT_MAXFILESIZE:
#endif
		case CURLOPT_AUTOREFERER:
		case CURLOPT_BUFFERSIZE:
		case CURLOPT_CONNECTTIMEOUT:
		case CURLOPT_COOKIESESSION:
		case CURLOPT_CRLF:
		case CURLOPT_DNS_CACHE_TIMEOUT:
		case CURLOPT_DNS_USE_GLOBAL_CACHE:
		case CURLOPT_FAILONERROR:
		case CURLOPT_FILETIME:
		case CURLOPT_FORBID_REUSE:
		case CURLOPT_FRESH_CONNECT:
		case CURLOPT_HEADER:
		case CURLOPT_HTTPPROXYTUNNEL:
		case CURLOPT_HTTP_VERSION:
		case CURLOPT_INFILESIZE:
		case CURLOPT_LOW_SPEED_LIMIT:
		case CURLOPT_LOW_SPEED_TIME:
		case CURLOPT_MAXCONNECTS:
		case CURLOPT_NETRC:
		case CURLOPT_NOBODY:
		case CURLOPT_NOPROGRESS:
		case CURLOPT_NOSIGNAL:
		case CURLOPT_PORT:
		case CURLOPT_PROXYPORT:
		case CURLOPT_PROXYTYPE:
		case CURLOPT_RESUME_FROM:
		case CURLOPT_SSLENGINE_DEFAULT:
		case CURLOPT_SSLVERSION:
		case CURLOPT_SSL_VERIFYHOST:
		case CURLOPT_SSL_VERIFYPEER:
		case CURLOPT_TIMECONDITION:
		case CURLOPT_TIMEVALUE:
		case CURLOPT_UNRESTRICTED_AUTH:
#if LIBCURL_VERSION_NUM >= 0x071304 /* Available since 7.19.4 */
		case CURLOPT_PROTOCOLS:
		case CURLOPT_REDIR_PROTOCOLS:
		case CURLOPT_SOCKS5_GSSAPI_NEC:
#endif
			curl_easy_setopt(rb_ch->ch, option, NUM2LONG(val));
			break;
		case CURLOPT_URL:
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
		case CURLOPT_PASSWORD:
		case CURLOPT_USERNAME:
#endif
#if LIBCURL_VERSION_NUM >= 0x071304 /* Available since 7.19.4 */
		case CURLOPT_SOCKS5_GSSAPI_SERVICE:
#endif
		case CURLOPT_CAINFO:
		case CURLOPT_SSLCERTTYPE:
		case CURLOPT_SSLKEYTYPE:
			curl_easy_setopt(rb_ch->ch, option, StringValueCStr(val));
			break;
		case CURLOPT_POSTFIELDS:
		case CURLOPT_USERAGENT:
		case CURLOPT_USERPWD:
#if LIBCURL_VERSION_NUM >= 0x070b00 /* Available since 7.11.0 */
		case CURLOPT_NETRC_FILE:
#endif
#if LIBCURL_VERSION_NUM >= 0x070e01 /* Available since 7.14.1 */
		case CURLOPT_COOKIELIST:
#endif
#if LIBCURL_VERSION_NUM >= 0x071004 /* Available since 7.16.4 */
		case CURLOPT_KRBLEVEL:
#endif
#if LIBCURL_VERSION_NUM >= 0x071300 /* Available since 7.19.0 */
		case CURLOPT_CRLFILE:
		case CURLOPT_ISSUERCERT:
		case CURLOPT_KEYPASSWD:
#endif
#if LIBCURL_VERSION_NUM >= 0x071304 /* Available since 7.19.4 */
		case CURLOPT_NOPROXY:
#endif
#if LIBCURL_VERSION_NUM >= 0x071504 /* Available since 7.21.4 */
		case CURLOPT_TLSAUTH_PASSWORD:
		case CURLOPT_TLSAUTH_TYPE:
#endif
		case CURLOPT_TLSAUTH_USERNAME:
#if LIBCURL_VERSION_NUM >= 0x072800 /* Available since 7.40.0 */
		case CURLOPT_UNIX_SOCKET_PATH:
#endif
		case CURLOPT_CAPATH:
		case CURLOPT_COOKIE:
		case CURLOPT_COOKIEFILE:
		case CURLOPT_COOKIEJAR:
		case CURLOPT_CUSTOMREQUEST:
		case CURLOPT_EGDSOCKET:
		case CURLOPT_ENCODING:
		case CURLOPT_INTERFACE:
		case CURLOPT_PROXY:
		case CURLOPT_PROXYUSERPWD:
		case CURLOPT_RANDOM_FILE:
		case CURLOPT_RANGE:
		case CURLOPT_REFERER:
		case CURLOPT_SSLCERT:
		case CURLOPT_SSLENGINE:
		case CURLOPT_SSLKEY:
		case CURLOPT_SSL_CIPHER_LIST:
			curl_easy_setopt(rb_ch->ch, option, NIL_P(val) ? NULL : StringValueCStr(val));
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
#if LIBCURL_VERSION_NUM >= 0x070f02 /* Available since 7.15.2 */
		case CURLOPT_MAX_RECV_SPEED_LARGE:
		case CURLOPT_MAX_SEND_SPEED_LARGE:
			curl_easy_setopt(rb_ch->ch, option, (curl_off_t) NUM2LL(val));
			break;
#endif
		case CURLOPT_HTTP200ALIASES:
			rb_ch->curl_http200aliases_slist = rb_array_to_curl_slist(val, rb_ch->curl_http200aliases_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTP200ALIASES, rb_ch->curl_http200aliases_slist);
			break;
		case CURLOPT_HTTPHEADER:
			rb_ch->curl_httpheader_slist = rb_array_to_curl_slist(val, rb_ch->curl_httpheader_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_HTTPHEADER, rb_ch->curl_httpheader_slist);
			break;
#if LIBCURL_VERSION_NUM >= 0x071503 /* Available since 7.21.3 */
		case CURLOPT_RESOLVE:
			rb_ch->curl_hosts_slist = rb_array_to_curl_slist(val, rb_ch->curl_hosts_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_RESOLVE, rb_ch->curl_hosts_slist);
			break;
#endif
#if LIBCURL_VERSION_NUM >= 0x072500 /* Available since 7.37.0 */
		case CURLOPT_PROXYHEADER:
			rb_ch->curl_headers_slist = rb_array_to_curl_slist(val, rb_ch->curl_headers_slist);
			curl_easy_setopt(rb_ch->ch, CURLOPT_PROXYHEADER, rb_ch->curl_headers_slist);
			break;
#endif
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

static VALUE rb_curl_easy_reset(VALUE self) {
	rb_curl_easy *rb_ch;
	
	Data_Get_Struct(self, rb_curl_easy, rb_ch);
	curl_easy_reset(rb_ch->ch);
	rb_curl_zero_state(rb_ch);
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
	rb_define_method(rb_cEasy, "reset", rb_curl_easy_reset, 0);
}
