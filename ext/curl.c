#include "curl.h"

VALUE rb_mCurl;

void Init_curl() {
	VALUE rb_mOpt, rb_mMOpt, rb_mInfo, rb_mProxy, rb_mAuth, rb_mProto, rb_mError;

	rb_mCurl = rb_define_module("Curl");
	rb_mOpt = rb_define_module_under(rb_mCurl, "Opt");
	rb_mMOpt = rb_define_module_under(rb_mCurl, "MOpt");
	rb_mInfo = rb_define_module_under(rb_mCurl, "Info");
	rb_mProxy = rb_define_module_under(rb_mCurl, "Proxy");
	rb_mAuth = rb_define_module_under(rb_mCurl, "Auth");
	rb_mProto = rb_define_module_under(rb_mCurl, "Proto");
	rb_mError = rb_define_module_under(rb_mCurl, "Error");

	curl_global_init(CURL_GLOBAL_ALL);

	/* Curl constants */
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_NONE", CURL_HTTP_VERSION_NONE);
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_1_0", CURL_HTTP_VERSION_1_0);
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_1_1", CURL_HTTP_VERSION_1_1);
#if LIBCURL_VERSION_NUM >= 0x072100 /* Available since 7.33.0 */
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_2_0", CURL_HTTP_VERSION_2_0);
#endif
#if LIBCURL_VERSION_NUM >= 0x072f00 /* Available since 7.47.0 */
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_2TLS", CURL_HTTP_VERSION_2TLS);
#endif
#if LIBCURL_VERSION_NUM >= 0x073100 /* Available since 7.49.0 */
	RB_DEFINE_M_CURL_CONST("HTTP_VERSION_2_PRIOR_KNOWLEDGE", CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
#endif
	RB_DEFINE_M_CURL_CONST("SSLVERSION_DEFAULT", CURL_SSLVERSION_DEFAULT);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_TLSv1", CURL_SSLVERSION_TLSv1);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_SSLv2", CURL_SSLVERSION_SSLv2);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_SSLv3", CURL_SSLVERSION_SSLv3);
#if LIBCURL_VERSION_NUM >= 0x072200 /* Available since 7.34.0 */
	RB_DEFINE_M_CURL_CONST("SSLVERSION_TLSv1_0", CURL_SSLVERSION_TLSv1_0);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_TLSv1_1", CURL_SSLVERSION_TLSv1_1);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_TLSv1_2", CURL_SSLVERSION_TLSv1_2);
#endif
	RB_DEFINE_M_CURL_CONST("SSLVERSION_TLSv1_3", CURL_SSLVERSION_TLSv1_3);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_MAX_DEFAULT", CURL_SSLVERSION_MAX_DEFAULT);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_MAX_TLSv1_0", CURL_SSLVERSION_MAX_TLSv1_0);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_MAX_TLSv1_1", CURL_SSLVERSION_MAX_TLSv1_1);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_MAX_TLSv1_2", CURL_SSLVERSION_MAX_TLSv1_2);
	RB_DEFINE_M_CURL_CONST("SSLVERSION_MAX_TLSv1_3", CURL_SSLVERSION_MAX_TLSv1_3);
	RB_DEFINE_M_CURL_CONST("TIMECOND_IFMODSINCE", CURL_TIMECOND_IFMODSINCE);
	RB_DEFINE_M_CURL_CONST("TIMECOND_IFUNMODSINCE", CURL_TIMECOND_IFUNMODSINCE);
	RB_DEFINE_M_CURL_CONST("TIMECOND_LASTMOD", CURL_TIMECOND_LASTMOD);
	RB_DEFINE_M_CURL_CONST("TIMECOND_NONE", CURL_TIMECOND_NONE);
	RB_DEFINE_M_CURL_CONST("NETRC_OPTIONAL", CURL_NETRC_OPTIONAL);
	RB_DEFINE_M_CURL_CONST("NETRC_IGNORED", CURL_NETRC_IGNORED);
	RB_DEFINE_M_CURL_CONST("NETRC_REQUIRED", CURL_NETRC_REQUIRED);
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
	RB_DEFINE_M_CURL_CONST("REDIR_POST_301", CURL_REDIR_POST_301);
	RB_DEFINE_M_CURL_CONST("REDIR_POST_302", CURL_REDIR_POST_302);
	RB_DEFINE_M_CURL_CONST("REDIR_POST_ALL", CURL_REDIR_POST_ALL);
#endif
#if LIBCURL_VERSION_NUM >= 0x071901 /* Available since 7.25.1 */
	RB_DEFINE_M_CURL_CONST("REDIR_POST_303", CURL_REDIR_POST_303);
#endif
#if LIBCURL_VERSION_NUM >= 0x070a08 /* Available since 7.10.8 */
	RB_DEFINE_M_CURL_CONST("IPRESOLVE_WHATEVER", CURL_IPRESOLVE_WHATEVER);
	RB_DEFINE_M_CURL_CONST("IPRESOLVE_V4", CURL_IPRESOLVE_V4);
	RB_DEFINE_M_CURL_CONST("IPRESOLVE_V6", CURL_IPRESOLVE_V6);
#endif

	/* Curl::Opt constants */
	RB_DEFINE_M_OPT_CONST("URL", CURLOPT_URL);
	RB_DEFINE_M_OPT_CONST("POST", CURLOPT_POST);
	RB_DEFINE_M_OPT_CONST("TIMEOUT", CURLOPT_TIMEOUT);
	RB_DEFINE_M_OPT_CONST("WRITEFUNCTION", CURLOPT_WRITEFUNCTION);
	RB_DEFINE_M_OPT_CONST("WRITEDATA", CURLOPT_WRITEDATA);
	RB_DEFINE_M_OPT_CONST("HEADERFUNCTION", CURLOPT_HEADERFUNCTION);
	RB_DEFINE_M_OPT_CONST("HEADERDATA", CURLOPT_HEADERDATA);
	RB_DEFINE_M_OPT_CONST("READFUNCTION", CURLOPT_READFUNCTION);
	RB_DEFINE_M_OPT_CONST("READDATA", CURLOPT_READDATA);
	RB_DEFINE_M_OPT_CONST("FOLLOWLOCATION", CURLOPT_FOLLOWLOCATION);
	RB_DEFINE_M_OPT_CONST("MAXREDIRS", CURLOPT_MAXREDIRS);
	RB_DEFINE_M_OPT_CONST("PUT", CURLOPT_PUT);
	RB_DEFINE_M_OPT_CONST("HTTPGET", CURLOPT_HTTPGET);
	RB_DEFINE_M_OPT_CONST("UPLOAD", CURLOPT_UPLOAD);
	RB_DEFINE_M_OPT_CONST("VERBOSE", CURLOPT_VERBOSE);
	RB_DEFINE_M_OPT_CONST("POSTFIELDS", CURLOPT_POSTFIELDS);
	RB_DEFINE_M_OPT_CONST("USERAGENT", CURLOPT_USERAGENT);
	RB_DEFINE_M_OPT_CONST("USERPWD", CURLOPT_USERPWD);
#if LIBCURL_VERSION_NUM >= 0x070b00 /* Available since 7.11.0 */
	RB_DEFINE_M_OPT_CONST("NETRC_FILE", CURLOPT_NETRC_FILE);
#endif
#if LIBCURL_VERSION_NUM >= 0x070b02 /* Available since 7.11.2 */
	RB_DEFINE_M_OPT_CONST("TCP_NODELAY", CURLOPT_TCP_NODELAY);
#endif
#if LIBCURL_VERSION_NUM >= 0x070e01 /* Available since 7.14.1 */
	RB_DEFINE_M_OPT_CONST("COOKIELIST", CURLOPT_COOKIELIST);
	RB_DEFINE_M_OPT_CONST("IGNORE_CONTENT_LENGTH", CURLOPT_IGNORE_CONTENT_LENGTH);
#endif
#if LIBCURL_VERSION_NUM >= 0x070f02 /* Available since 7.15.2 */
	RB_DEFINE_M_OPT_CONST("CONNECT_ONLY", CURLOPT_CONNECT_ONLY);
	RB_DEFINE_M_OPT_CONST("LOCALPORT", CURLOPT_LOCALPORT);
#endif
#if LIBCURL_VERSION_NUM >= 0x070f05 /* Available since 7.15.5 */
	RB_DEFINE_M_OPT_CONST("MAX_RECV_SPEED_LARGE", CURLOPT_MAX_RECV_SPEED_LARGE);
	RB_DEFINE_M_OPT_CONST("MAX_SEND_SPEED_LARGE", CURLOPT_MAX_SEND_SPEED_LARGE);
#endif
#if LIBCURL_VERSION_NUM >= 0x071000 /* Available since 7.16.0 */
	RB_DEFINE_M_OPT_CONST("SSL_SESSIONID_CACHE", CURLOPT_SSL_SESSIONID_CACHE);
#endif
#if LIBCURL_VERSION_NUM >= 0x071002 /* Available since 7.16.2 */
	RB_DEFINE_M_OPT_CONST("CONNECTTIMEOUT_MS", CURLOPT_CONNECTTIMEOUT_MS);
	RB_DEFINE_M_OPT_CONST("HTTP_CONTENT_DECODING", CURLOPT_HTTP_CONTENT_DECODING);
	RB_DEFINE_M_OPT_CONST("HTTP_TRANSFER_DECODING", CURLOPT_HTTP_TRANSFER_DECODING);
	RB_DEFINE_M_OPT_CONST("TIMEOUT_MS", CURLOPT_TIMEOUT_MS);
#endif
#if LIBCURL_VERSION_NUM >= 0x071004 /* Available since 7.16.4 */
	RB_DEFINE_M_OPT_CONST("KRBLEVEL", CURLOPT_KRBLEVEL);
#endif
#if LIBCURL_VERSION_NUM >= 0x071300 /* Available since 7.19.0 */
	RB_DEFINE_M_OPT_CONST("ADDRESS_SCOPE", CURLOPT_ADDRESS_SCOPE);
	RB_DEFINE_M_OPT_CONST("CRLFILE", CURLOPT_CRLFILE);
	RB_DEFINE_M_OPT_CONST("ISSUERCERT", CURLOPT_ISSUERCERT);
	RB_DEFINE_M_OPT_CONST("KEYPASSWD", CURLOPT_KEYPASSWD);
#endif
#if LIBCURL_VERSION_NUM >= 0x071301 /* Available since 7.19.1 */
	RB_DEFINE_M_OPT_CONST("CERTINFO", CURLOPT_CERTINFO);
	RB_DEFINE_M_OPT_CONST("PASSWORD", CURLOPT_PASSWORD);
	RB_DEFINE_M_OPT_CONST("POSTREDIR", CURLOPT_POSTREDIR);
	RB_DEFINE_M_OPT_CONST("USERNAME", CURLOPT_USERNAME);
#endif
#if LIBCURL_VERSION_NUM >= 0x071304 /* Available since 7.19.4 */
	RB_DEFINE_M_OPT_CONST("NOPROXY", CURLOPT_NOPROXY);
	RB_DEFINE_M_OPT_CONST("PROTOCOLS", CURLOPT_PROTOCOLS);
	RB_DEFINE_M_OPT_CONST("REDIR_PROTOCOLS", CURLOPT_REDIR_PROTOCOLS);
	RB_DEFINE_M_OPT_CONST("SOCKS5_GSSAPI_NEC", CURLOPT_SOCKS5_GSSAPI_NEC);
	RB_DEFINE_M_OPT_CONST("SOCKS5_GSSAPI_SERVICE", CURLOPT_SOCKS5_GSSAPI_SERVICE);
#endif
#if LIBCURL_VERSION_NUM >= 0x071504 /* Available since 7.21.4 */
	RB_DEFINE_M_OPT_CONST("TLSAUTH_PASSWORD", CURLOPT_TLSAUTH_PASSWORD);
	RB_DEFINE_M_OPT_CONST("TLSAUTH_TYPE", CURLOPT_TLSAUTH_TYPE);
#endif
#if LIBCURL_VERSION_NUM >= 0x071600 /* Available since 7.22.0 */
	RB_DEFINE_M_OPT_CONST("GSSAPI_DELEGATION", CURLOPT_GSSAPI_DELEGATION);
#endif
#if LIBCURL_VERSION_NUM >= 0x072800 /* Available since 7.40.0 */
	RB_DEFINE_M_OPT_CONST("UNIX_SOCKET_PATH", CURLOPT_UNIX_SOCKET_PATH);
#endif
#if LIBCURL_VERSION_NUM >= 0x072a00 /* Available since 7.42.0 */
	RB_DEFINE_M_OPT_CONST("PATH_AS_IS", CURLOPT_PATH_AS_IS);
#endif
#if LIBCURL_VERSION_NUM >= 0x072b00 /* Available since 7.43.0 */
	RB_DEFINE_M_OPT_CONST("PIPEWAIT", CURLOPT_PIPEWAIT);
#endif
#if LIBCURL_VERSION_NUM >= 0x070a06 /* Available since 7.10.6 */
	RB_DEFINE_M_OPT_CONST("HTTPAUTH", CURLOPT_HTTPAUTH);
#endif
#if LIBCURL_VERSION_NUM >= 0x070a07 /* Available since 7.10.7 */
	RB_DEFINE_M_OPT_CONST("PROXYAUTH", CURLOPT_PROXYAUTH);
#endif
#if LIBCURL_VERSION_NUM >= 0x070a08 /* Available since 7.10.8 */
	RB_DEFINE_M_OPT_CONST("IPRESOLVE", CURLOPT_IPRESOLVE);
	RB_DEFINE_M_OPT_CONST("MAXFILESIZE", CURLOPT_MAXFILESIZE);
#endif
	RB_DEFINE_M_OPT_CONST("AUTOREFERER", CURLOPT_AUTOREFERER);
	RB_DEFINE_M_OPT_CONST("BUFFERSIZE", CURLOPT_BUFFERSIZE);
	RB_DEFINE_M_OPT_CONST("CAINFO", CURLOPT_CAINFO);
	RB_DEFINE_M_OPT_CONST("CAPATH", CURLOPT_CAPATH);
	RB_DEFINE_M_OPT_CONST("CONNECTTIMEOUT", CURLOPT_CONNECTTIMEOUT);
	RB_DEFINE_M_OPT_CONST("COOKIE", CURLOPT_COOKIE);
	RB_DEFINE_M_OPT_CONST("COOKIEFILE", CURLOPT_COOKIEFILE);
	RB_DEFINE_M_OPT_CONST("COOKIEJAR", CURLOPT_COOKIEJAR);
	RB_DEFINE_M_OPT_CONST("COOKIESESSION", CURLOPT_COOKIESESSION);
	RB_DEFINE_M_OPT_CONST("CRLF", CURLOPT_CRLF);
	RB_DEFINE_M_OPT_CONST("CUSTOMREQUEST", CURLOPT_CUSTOMREQUEST);
	RB_DEFINE_M_OPT_CONST("DNS_CACHE_TIMEOUT", CURLOPT_DNS_CACHE_TIMEOUT);
	RB_DEFINE_M_OPT_CONST("DNS_USE_GLOBAL_CACHE", CURLOPT_DNS_USE_GLOBAL_CACHE);
	RB_DEFINE_M_OPT_CONST("EGDSOCKET", CURLOPT_EGDSOCKET);
	RB_DEFINE_M_OPT_CONST("ENCODING", CURLOPT_ENCODING);
	RB_DEFINE_M_OPT_CONST("FAILONERROR", CURLOPT_FAILONERROR);
	RB_DEFINE_M_OPT_CONST("FILETIME", CURLOPT_FILETIME);
	RB_DEFINE_M_OPT_CONST("FORBID_REUSE", CURLOPT_FORBID_REUSE);
	RB_DEFINE_M_OPT_CONST("FRESH_CONNECT", CURLOPT_FRESH_CONNECT);
	RB_DEFINE_M_OPT_CONST("HEADER", CURLOPT_HEADER);
	RB_DEFINE_M_OPT_CONST("HTTP200ALIASES", CURLOPT_HTTP200ALIASES);
	RB_DEFINE_M_OPT_CONST("HTTPHEADER", CURLOPT_HTTPHEADER);
	RB_DEFINE_M_OPT_CONST("HTTPPROXYTUNNEL", CURLOPT_HTTPPROXYTUNNEL);
	RB_DEFINE_M_OPT_CONST("HTTP_VERSION", CURLOPT_HTTP_VERSION);
	RB_DEFINE_M_OPT_CONST("INFILESIZE", CURLOPT_INFILESIZE);
	RB_DEFINE_M_OPT_CONST("INTERFACE", CURLOPT_INTERFACE);
	RB_DEFINE_M_OPT_CONST("LOW_SPEED_LIMIT", CURLOPT_LOW_SPEED_LIMIT);
	RB_DEFINE_M_OPT_CONST("LOW_SPEED_TIME", CURLOPT_LOW_SPEED_TIME);
	RB_DEFINE_M_OPT_CONST("MAXCONNECTS", CURLOPT_MAXCONNECTS);
	RB_DEFINE_M_OPT_CONST("NETRC", CURLOPT_NETRC);
	RB_DEFINE_M_OPT_CONST("NOBODY", CURLOPT_NOBODY);
	RB_DEFINE_M_OPT_CONST("NOPROGRESS", CURLOPT_NOPROGRESS);
	RB_DEFINE_M_OPT_CONST("NOSIGNAL", CURLOPT_NOSIGNAL);
	RB_DEFINE_M_OPT_CONST("PORT", CURLOPT_PORT);
	RB_DEFINE_M_OPT_CONST("PRIVATE", CURLOPT_PRIVATE);
	RB_DEFINE_M_OPT_CONST("PROXY", CURLOPT_PROXY);
	RB_DEFINE_M_OPT_CONST("PROXYPORT", CURLOPT_PROXYPORT);
	RB_DEFINE_M_OPT_CONST("PROXYTYPE", CURLOPT_PROXYTYPE);
	RB_DEFINE_M_OPT_CONST("PROXYUSERPWD", CURLOPT_PROXYUSERPWD);
	RB_DEFINE_M_OPT_CONST("RANDOM_FILE", CURLOPT_RANDOM_FILE);
	RB_DEFINE_M_OPT_CONST("RANGE", CURLOPT_RANGE);
	RB_DEFINE_M_OPT_CONST("REFERER", CURLOPT_REFERER);
	RB_DEFINE_M_OPT_CONST("RESUME_FROM", CURLOPT_RESUME_FROM);
	RB_DEFINE_M_OPT_CONST("SSLCERT", CURLOPT_SSLCERT);
	RB_DEFINE_M_OPT_CONST("SSLCERTTYPE", CURLOPT_SSLCERTTYPE);
	RB_DEFINE_M_OPT_CONST("SSLENGINE", CURLOPT_SSLENGINE);
	RB_DEFINE_M_OPT_CONST("SSLENGINE_DEFAULT", CURLOPT_SSLENGINE_DEFAULT);
	RB_DEFINE_M_OPT_CONST("SSLKEY", CURLOPT_SSLKEY);
	RB_DEFINE_M_OPT_CONST("SSLKEYTYPE", CURLOPT_SSLKEYTYPE);
	RB_DEFINE_M_OPT_CONST("SSLVERSION", CURLOPT_SSLVERSION);
	RB_DEFINE_M_OPT_CONST("SSL_CIPHER_LIST", CURLOPT_SSL_CIPHER_LIST);
	RB_DEFINE_M_OPT_CONST("SSL_VERIFYHOST", CURLOPT_SSL_VERIFYHOST);
	RB_DEFINE_M_OPT_CONST("SSL_VERIFYPEER", CURLOPT_SSL_VERIFYPEER);
	RB_DEFINE_M_OPT_CONST("TIMECONDITION", CURLOPT_TIMECONDITION);
	RB_DEFINE_M_OPT_CONST("TIMEVALUE", CURLOPT_TIMEVALUE);
	RB_DEFINE_M_OPT_CONST("TRANSFERTEXT", CURLOPT_TRANSFERTEXT);
	RB_DEFINE_M_OPT_CONST("UNRESTRICTED_AUTH", CURLOPT_UNRESTRICTED_AUTH);
#if LIBCURL_VERSION_NUM >= 0x071503 /* Available since 7.21.3 */
	RB_DEFINE_M_OPT_CONST("RESOLVE", CURLOPT_RESOLVE);
#endif
#if LIBCURL_VERSION_NUM >= 0x072500 /* Available since 7.37.0 */
	RB_DEFINE_M_OPT_CONST("PROXYHEADER", CURLOPT_PROXYHEADER);
#endif

	/* Curl::MOpt constants */
	RB_DEFINE_M_MOPT_CONST("MAXCONNECTS", CURLMOPT_MAXCONNECTS);
	RB_DEFINE_M_MOPT_CONST("PIPELINING", CURLMOPT_PIPELINING);

	/* Curl::Info constants */
	RB_DEFINE_M_INFO_CONST("EFFECTIVE_URL", CURLINFO_EFFECTIVE_URL);
	RB_DEFINE_M_INFO_CONST("FILETIME", CURLINFO_FILETIME);
	RB_DEFINE_M_INFO_CONST("TOTAL_TIME", CURLINFO_TOTAL_TIME);
	RB_DEFINE_M_INFO_CONST("NAMELOOKUP_TIME", CURLINFO_NAMELOOKUP_TIME);
	RB_DEFINE_M_INFO_CONST("CONNECT_TIME", CURLINFO_CONNECT_TIME);
	RB_DEFINE_M_INFO_CONST("PRETRANSFER_TIME", CURLINFO_PRETRANSFER_TIME);
	RB_DEFINE_M_INFO_CONST("STARTTRANSFER_TIME", CURLINFO_STARTTRANSFER_TIME);
	RB_DEFINE_M_INFO_CONST("REDIRECT_TIME", CURLINFO_REDIRECT_TIME);
	RB_DEFINE_M_INFO_CONST("REDIRECT_COUNT", CURLINFO_REDIRECT_COUNT);
	RB_DEFINE_M_INFO_CONST("SIZE_UPLOAD", CURLINFO_SIZE_UPLOAD);
	RB_DEFINE_M_INFO_CONST("SIZE_DOWNLOAD", CURLINFO_SIZE_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("SPEED_UPLOAD", CURLINFO_SPEED_UPLOAD);
	RB_DEFINE_M_INFO_CONST("SPEED_DOWNLOAD", CURLINFO_SPEED_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("HEADER_SIZE", CURLINFO_HEADER_SIZE);
	RB_DEFINE_M_INFO_CONST("REQUEST_SIZE", CURLINFO_REQUEST_SIZE);
	RB_DEFINE_M_INFO_CONST("SSL_VERIFYRESULT", CURLINFO_SSL_VERIFYRESULT);
	RB_DEFINE_M_INFO_CONST("CONTENT_TYPE", CURLINFO_CONTENT_TYPE);
	RB_DEFINE_M_INFO_CONST("LASTSOCKET", CURLINFO_LASTSOCKET);
	RB_DEFINE_M_INFO_CONST("CONTENT_LENGTH_DOWNLOAD", CURLINFO_CONTENT_LENGTH_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("CONTENT_LENGTH_UPLOAD", CURLINFO_CONTENT_LENGTH_UPLOAD);
	/* Curl::Info constants (Available since 7.10.7) */
	RB_DEFINE_M_INFO_CONST("HTTP_CONNECTCODE", CURLINFO_HTTP_CONNECTCODE);
	/* Curl::Info constants (Available since 7.10.8) */
	RB_DEFINE_M_INFO_CONST("HTTPAUTH_AVAIL", CURLINFO_HTTPAUTH_AVAIL);
	RB_DEFINE_M_INFO_CONST("RESPONSE_CODE", CURLINFO_RESPONSE_CODE);
	RB_DEFINE_M_INFO_CONST("PROXYAUTH_AVAIL", CURLINFO_PROXYAUTH_AVAIL);
	/* Curl::Info constants (Available since 7.12.2) */
	RB_DEFINE_M_INFO_CONST("OS_ERRNO", CURLINFO_OS_ERRNO);
	/* Curl::Info constants (Available since 7.12.3) */
	RB_DEFINE_M_INFO_CONST("NUM_CONNECTS", CURLINFO_NUM_CONNECTS);
	RB_DEFINE_M_INFO_CONST("SSL_ENGINES", CURLINFO_SSL_ENGINES);
	/* Curl::Info constants (Available since 7.14.1) */
	RB_DEFINE_M_INFO_CONST("COOKIELIST", CURLINFO_COOKIELIST);
#if LIBCURL_VERSION_NUM >= 0x071202 /* Curl::Info constants (Available since 7.18.2) */
	RB_DEFINE_M_INFO_CONST("REDIRECT_URL", CURLINFO_REDIRECT_URL);
#endif
#if LIBCURL_VERSION_NUM >= 0x071300 /* Curl::Info constants (Available since 7.19.0) */
	RB_DEFINE_M_INFO_CONST("PRIMARY_IP", CURLINFO_PRIMARY_IP);
#endif
#if LIBCURL_VERSION_NUM >= 0x071301 /* Curl::Info constants (Available since 7.19.1) */
	RB_DEFINE_M_INFO_CONST("CERTINFO", CURLINFO_CERTINFO);
#endif
#if LIBCURL_VERSION_NUM >= 0x071500 /* Curl::Info constants (Available since 7.21.0) */
	RB_DEFINE_M_INFO_CONST("LOCAL_IP", CURLINFO_LOCAL_IP);
	RB_DEFINE_M_INFO_CONST("LOCAL_PORT", CURLINFO_LOCAL_PORT);
	RB_DEFINE_M_INFO_CONST("PRIMARY_PORT", CURLINFO_PRIMARY_PORT);
#endif
#if LIBCURL_VERSION_NUM >= 0x073200 /* Curl::Info constants (Available since 7.50.0) */
	RB_DEFINE_M_INFO_CONST("HTTP_VERSION", CURLINFO_HTTP_VERSION);
#endif
#if LIBCURL_VERSION_NUM >= 0x073400 /* Curl::Info constants (Available since 7.52.0) */
	RB_DEFINE_M_INFO_CONST("PROTOCOL", CURLINFO_PROTOCOL);
	RB_DEFINE_M_INFO_CONST("PROXY_SSL_VERIFYRESULT", CURLINFO_PROXY_SSL_VERIFYRESULT);
	RB_DEFINE_M_INFO_CONST("SCHEME", CURLINFO_SCHEME);
#endif
	
	/* Curl::Auth constants */
#if LIBCURL_VERSION_NUM >= 0x071303 /* Available since 7.19.3 */
	RB_DEFINE_M_AUTH_CONST("DIGEST_IE", CURLAUTH_DIGEST_IE);
#endif
#if LIBCURL_VERSION_NUM >= 0x073d00 /* Available since 7.61.0 */
	RB_DEFINE_M_AUTH_CONST("BEARER", CURLAUTH_BEARER);
#endif
#if LIBCURL_VERSION_NUM >= 0x072600 /* Available since 7.38.0 */
	RB_DEFINE_M_AUTH_CONST("NEGOTIATE", CURLAUTH_NEGOTIATE);
#endif
#if LIBCURL_VERSION_NUM >= 0x071600 /* Available since 7.22.0 */
	RB_DEFINE_M_AUTH_CONST("NTLM_WB", CURLAUTH_NTLM_WB);
#endif
#if LIBCURL_VERSION_NUM >= 0x070a06 /* Available since 7.10.6 */
	RB_DEFINE_M_AUTH_CONST("ANY", CURLAUTH_ANY);
	RB_DEFINE_M_AUTH_CONST("ANYSAFE", CURLAUTH_ANYSAFE);
	RB_DEFINE_M_AUTH_CONST("BASIC", CURLAUTH_BASIC);
	RB_DEFINE_M_AUTH_CONST("DIGEST", CURLAUTH_DIGEST);
	RB_DEFINE_M_AUTH_CONST("NTLM", CURLAUTH_NTLM);
#endif
#if LIBCURL_VERSION_NUM >= 0x071503 /* Available since 7.21.3 */
	RB_DEFINE_M_AUTH_CONST("ONLY", CURLAUTH_ONLY);
#endif
	
	/* Curl::Proto constants */
	RB_DEFINE_M_PROTO_CONST("DICT", CURLPROTO_DICT);
	RB_DEFINE_M_PROTO_CONST("FILE", CURLPROTO_FILE);
	RB_DEFINE_M_PROTO_CONST("FTP", CURLPROTO_FTP);
	RB_DEFINE_M_PROTO_CONST("FTPS", CURLPROTO_FTPS);
	RB_DEFINE_M_PROTO_CONST("GOPHER", CURLPROTO_GOPHER);
	RB_DEFINE_M_PROTO_CONST("HTTP", CURLPROTO_HTTP);
	RB_DEFINE_M_PROTO_CONST("HTTPS", CURLPROTO_HTTPS);
	RB_DEFINE_M_PROTO_CONST("IMAP", CURLPROTO_IMAP);
	RB_DEFINE_M_PROTO_CONST("IMAPS", CURLPROTO_IMAPS);
	RB_DEFINE_M_PROTO_CONST("LDAP", CURLPROTO_LDAP);
	RB_DEFINE_M_PROTO_CONST("LDAPS", CURLPROTO_LDAPS);
	RB_DEFINE_M_PROTO_CONST("POP3", CURLPROTO_POP3);
	RB_DEFINE_M_PROTO_CONST("POP3S", CURLPROTO_POP3S);
	RB_DEFINE_M_PROTO_CONST("RTMP", CURLPROTO_RTMP);
	RB_DEFINE_M_PROTO_CONST("RTMPE", CURLPROTO_RTMPE);
	RB_DEFINE_M_PROTO_CONST("RTMPS", CURLPROTO_RTMPS);
	RB_DEFINE_M_PROTO_CONST("RTMPT", CURLPROTO_RTMPT);
	RB_DEFINE_M_PROTO_CONST("RTMPTE", CURLPROTO_RTMPTE);
	RB_DEFINE_M_PROTO_CONST("RTMPTS", CURLPROTO_RTMPTS);
	RB_DEFINE_M_PROTO_CONST("RTSP", CURLPROTO_RTSP);
	RB_DEFINE_M_PROTO_CONST("SCP", CURLPROTO_SCP);
	RB_DEFINE_M_PROTO_CONST("SFTP", CURLPROTO_SFTP);
	RB_DEFINE_M_PROTO_CONST("SMB", CURLPROTO_SMB);
	RB_DEFINE_M_PROTO_CONST("SMBS", CURLPROTO_SMBS);
	RB_DEFINE_M_PROTO_CONST("SMTP", CURLPROTO_SMTP);
	RB_DEFINE_M_PROTO_CONST("SMTPS", CURLPROTO_SMTPS);
	RB_DEFINE_M_PROTO_CONST("TELNET", CURLPROTO_TELNET);
	RB_DEFINE_M_PROTO_CONST("TFTP", CURLPROTO_TFTP);

	/* Curl::Proxy constants */
	RB_DEFINE_M_PROXY_CONST("HTTP", CURLPROXY_HTTP);
	RB_DEFINE_M_PROXY_CONST("HTTPS", CURLPROXY_HTTPS);
	RB_DEFINE_M_PROXY_CONST("HTTP_1_0", CURLPROXY_HTTP_1_0);
	RB_DEFINE_M_PROXY_CONST("SOCKS4", CURLPROXY_SOCKS4);
	RB_DEFINE_M_PROXY_CONST("SOCKS4A", CURLPROXY_SOCKS4A);
	RB_DEFINE_M_PROXY_CONST("SOCKS5", CURLPROXY_SOCKS5);
	RB_DEFINE_M_PROXY_CONST("SOCKS5_HOSTNAME", CURLPROXY_SOCKS5_HOSTNAME);

	/* Curl::Error constants */
	RB_DEFINE_M_ERROR_CONST("OK", CURLE_OK);
	RB_DEFINE_M_ERROR_CONST("UNSUPPORTED_PROTOCOL", CURLE_UNSUPPORTED_PROTOCOL);
	RB_DEFINE_M_ERROR_CONST("FAILED_INIT", CURLE_FAILED_INIT);
	RB_DEFINE_M_ERROR_CONST("URL_MALFORMAT", CURLE_URL_MALFORMAT);
	RB_DEFINE_M_ERROR_CONST("NOT_BUILT_IN", CURLE_NOT_BUILT_IN);
	RB_DEFINE_M_ERROR_CONST("COULDNT_RESOLVE_PROXY", CURLE_COULDNT_RESOLVE_PROXY);
	RB_DEFINE_M_ERROR_CONST("COULDNT_RESOLVE_HOST", CURLE_COULDNT_RESOLVE_HOST);
	RB_DEFINE_M_ERROR_CONST("COULDNT_CONNECT", CURLE_COULDNT_CONNECT);
	RB_DEFINE_M_ERROR_CONST("REMOTE_ACCESS_DENIED", CURLE_REMOTE_ACCESS_DENIED);
	RB_DEFINE_M_ERROR_CONST("HTTP2", CURLE_HTTP2);
	RB_DEFINE_M_ERROR_CONST("HTTP_RETURNED_ERROR", CURLE_HTTP_RETURNED_ERROR);
	RB_DEFINE_M_ERROR_CONST("WRITE_ERROR", CURLE_WRITE_ERROR);
	RB_DEFINE_M_ERROR_CONST("UPLOAD_FAILED", CURLE_UPLOAD_FAILED);
	RB_DEFINE_M_ERROR_CONST("READ_ERROR", CURLE_READ_ERROR);
	RB_DEFINE_M_ERROR_CONST("OUT_OF_MEMORY", CURLE_OUT_OF_MEMORY);
	RB_DEFINE_M_ERROR_CONST("OPERATION_TIMEDOUT", CURLE_OPERATION_TIMEDOUT);
	RB_DEFINE_M_ERROR_CONST("RANGE_ERROR", CURLE_RANGE_ERROR);
	RB_DEFINE_M_ERROR_CONST("HTTP_POST_ERROR", CURLE_HTTP_POST_ERROR);
	RB_DEFINE_M_ERROR_CONST("SSL_CONNECT_ERROR", CURLE_SSL_CONNECT_ERROR);
	RB_DEFINE_M_ERROR_CONST("BAD_DOWNLOAD_RESUME", CURLE_BAD_DOWNLOAD_RESUME);
	RB_DEFINE_M_ERROR_CONST("FILE_COULDNT_READ_FILE", CURLE_FILE_COULDNT_READ_FILE);
	RB_DEFINE_M_ERROR_CONST("FUNCTION_NOT_FOUND", CURLE_FUNCTION_NOT_FOUND);
	RB_DEFINE_M_ERROR_CONST("ABORTED_BY_CALLBACK", CURLE_ABORTED_BY_CALLBACK);
	RB_DEFINE_M_ERROR_CONST("BAD_FUNCTION_ARGUMENT", CURLE_BAD_FUNCTION_ARGUMENT);
	RB_DEFINE_M_ERROR_CONST("INTERFACE_FAILED", CURLE_INTERFACE_FAILED);
	RB_DEFINE_M_ERROR_CONST("TOO_MANY_REDIRECTS", CURLE_TOO_MANY_REDIRECTS);
	RB_DEFINE_M_ERROR_CONST("UNKNOWN_OPTION", CURLE_UNKNOWN_OPTION);
	RB_DEFINE_M_ERROR_CONST("GOT_NOTHING", CURLE_GOT_NOTHING);
	RB_DEFINE_M_ERROR_CONST("SSL_ENGINE_NOTFOUND", CURLE_SSL_ENGINE_NOTFOUND);
	RB_DEFINE_M_ERROR_CONST("SSL_ENGINE_SETFAILED", CURLE_SSL_ENGINE_SETFAILED);
	RB_DEFINE_M_ERROR_CONST("SEND_ERROR", CURLE_SEND_ERROR);
	RB_DEFINE_M_ERROR_CONST("RECV_ERROR", CURLE_RECV_ERROR);
	RB_DEFINE_M_ERROR_CONST("SSL_CERTPROBLEM", CURLE_SSL_CERTPROBLEM);
	RB_DEFINE_M_ERROR_CONST("SSL_CIPHER", CURLE_SSL_CIPHER);
	RB_DEFINE_M_ERROR_CONST("PEER_FAILED_VERIFICATION", CURLE_PEER_FAILED_VERIFICATION);
	RB_DEFINE_M_ERROR_CONST("BAD_CONTENT_ENCODING", CURLE_BAD_CONTENT_ENCODING);
	RB_DEFINE_M_ERROR_CONST("FILESIZE_EXCEEDED", CURLE_FILESIZE_EXCEEDED);
	RB_DEFINE_M_ERROR_CONST("SEND_FAIL_REWIND", CURLE_SEND_FAIL_REWIND);
	RB_DEFINE_M_ERROR_CONST("SSL_ENGINE_INITFAILED", CURLE_SSL_ENGINE_INITFAILED);
	RB_DEFINE_M_ERROR_CONST("LOGIN_DENIED", CURLE_LOGIN_DENIED);
	RB_DEFINE_M_ERROR_CONST("SSL_CACERT_BADFILE", CURLE_SSL_CACERT_BADFILE);
	RB_DEFINE_M_ERROR_CONST("AGAIN", CURLE_AGAIN);
	RB_DEFINE_M_ERROR_CONST("SSL_CRL_BADFILE", CURLE_SSL_CRL_BADFILE);
	RB_DEFINE_M_ERROR_CONST("SSL_ISSUER_ERROR", CURLE_SSL_ISSUER_ERROR);
	RB_DEFINE_M_ERROR_CONST("CHUNK_FAILED", CURLE_CHUNK_FAILED);
	RB_DEFINE_M_ERROR_CONST("HTTP2_STREAM", CURLE_HTTP2_STREAM);

	Init_easy();
	Init_multi();
}
