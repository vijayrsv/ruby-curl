#include "curl.h"

VALUE rb_mCurl;

void Init_curl() {
	VALUE rb_mOption, rb_mInfo;

	rb_mCurl   = rb_define_module("Curl");
	rb_mOption = rb_define_module_under(rb_mCurl, "Option");
	rb_mInfo = rb_define_module_under(rb_mCurl, "Info");
	rb_mAuth = rb_define_module_under(rb_mCurl, "Auth");
	rb_mIPResolve = rb_define_module_under(rb_mCurl, "IPResolve");
	rb_mRedirPost = rb_define_module_under(rb_mCurl, "RedirPost");

	RB_DEFINE_M_OPT_CONST("URL", CURLOPT_URL);
	RB_DEFINE_M_OPT_CONST("POST", CURLOPT_POST);
	RB_DEFINE_M_OPT_CONST("TIMEOUT", CURLOPT_TIMEOUT);
	RB_DEFINE_M_OPT_CONST("WRITEFUNCTION", CURLOPT_WRITEFUNCTION);
	RB_DEFINE_M_OPT_CONST("HEADERFUNCTION", CURLOPT_HEADERFUNCTION);
	RB_DEFINE_M_OPT_CONST("READFUNCTION", CURLOPT_READFUNCTION);
	RB_DEFINE_M_OPT_CONST("FOLLOWLOCATION", CURLOPT_FOLLOWLOCATION);
	RB_DEFINE_M_OPT_CONST("MAXREDIRS", CURLOPT_MAXREDIRS);
	RB_DEFINE_M_OPT_CONST("PUT", CURLOPT_PUT);
	RB_DEFINE_M_OPT_CONST("HTTPGET", CURLOPT_HTTPGET);
	RB_DEFINE_M_OPT_CONST("UPLOAD", CURLOPT_UPLOAD);
	RB_DEFINE_M_OPT_CONST("VERBOSE", CURLOPT_VERBOSE);
	RB_DEFINE_M_OPT_CONST("POSTFIELDS", CURLOPT_POSTFIELDS);
	RB_DEFINE_M_OPT_CONST("USERAGENT", CURLOPT_USERAGENT);
	RB_DEFINE_M_OPT_CONST("USERPWD", CURLOPT_USERPWD);
	RB_DEFINE_M_OPT_CONST("MAXFILESIZE", CURLOPT_MAXFILESIZE);
	RB_DEFINE_M_OPT_CONST("NETRC_FILE", CURLOPT_NETRC_FILE);
	RB_DEFINE_M_OPT_CONST("TCP_NODELAY", CURLOPT_TCP_NODELAY);
	RB_DEFINE_M_OPT_CONST("COOKIELIST", CURLOPT_COOKIELIST);
	RB_DEFINE_M_OPT_CONST("IGNORE_CONTENT_LENGTH", CURLOPT_IGNORE_CONTENT_LENGTH);
	RB_DEFINE_M_OPT_CONST("CONNECT_ONLY", CURLOPT_CONNECT_ONLY);
	RB_DEFINE_M_OPT_CONST("LOCALPORT", CURLOPT_LOCALPORT);
	RB_DEFINE_M_OPT_CONST("MAX_RECV_SPEED_LARGE", CURLOPT_MAX_RECV_SPEED_LARGE);
	RB_DEFINE_M_OPT_CONST("MAX_SEND_SPEED_LARGE", CURLOPT_MAX_SEND_SPEED_LARGE);
	RB_DEFINE_M_OPT_CONST("SSL_SESSIONID_CACHE", CURLOPT_SSL_SESSIONID_CACHE);
	RB_DEFINE_M_OPT_CONST("CONNECTTIMEOUT_MS", CURLOPT_CONNECTTIMEOUT_MS);
	RB_DEFINE_M_OPT_CONST("HTTP_CONTENT_DECODING", CURLOPT_HTTP_CONTENT_DECODING);
	RB_DEFINE_M_OPT_CONST("HTTP_TRANSFER_DECODING", CURLOPT_HTTP_TRANSFER_DECODING);
	RB_DEFINE_M_OPT_CONST("TIMEOUT_MS", CURLOPT_TIMEOUT_MS);
	RB_DEFINE_M_OPT_CONST("KRBLEVEL", CURLOPT_KRBLEVEL);
	RB_DEFINE_M_OPT_CONST("ADDRESS_SCOPE", CURLOPT_ADDRESS_SCOPE);
	RB_DEFINE_M_OPT_CONST("CRLFILE", CURLOPT_CRLFILE);
	RB_DEFINE_M_OPT_CONST("ISSUERCERT", CURLOPT_ISSUERCERT);
	RB_DEFINE_M_OPT_CONST("KEYPASSWD", CURLOPT_KEYPASSWD);
	RB_DEFINE_M_OPT_CONST("CURLOPT_CERTINFO", CURLOPT_CERTINFO);
	RB_DEFINE_M_OPT_CONST("PASSWORD", CURLOPT_PASSWORD);
	RB_DEFINE_M_OPT_CONST("USERNAME", CURLOPT_USERNAME);
	RB_DEFINE_M_OPT_CONST("NOPROXY", CURLOPT_NOPROXY);
	RB_DEFINE_M_OPT_CONST("SOCKS5_GSSAPI_NEC", CURLOPT_SOCKS5_GSSAPI_NEC);
	RB_DEFINE_M_OPT_CONST("SOCKS5_GSSAPI_SERVICE", CURLOPT_SOCKS5_GSSAPI_SERVICE);
	RB_DEFINE_M_OPT_CONST("TLSAUTH_PASSWORD", CURLOPT_TLSAUTH_PASSWORD);
	RB_DEFINE_M_OPT_CONST("TLSAUTH_TYPE", CURLOPT_TLSAUTH_TYPE);
	RB_DEFINE_M_OPT_CONST("GSSAPI_DELEGATION", CURLOPT_GSSAPI_DELEGATION);
	RB_DEFINE_M_OPT_CONST("UNIX_SOCKET_PATH", CURLOPT_UNIX_SOCKET_PATH);
	RB_DEFINE_M_OPT_CONST("PATH_AS_IS", CURLOPT_PATH_AS_IS);
	RB_DEFINE_M_OPT_CONST("PIPEWAIT", CURLOPT_PIPEWAIT);
	RB_DEFINE_M_OPT_CONST("HTTPAUTH", CURLOPT_HTTPAUTH);
	RB_DEFINE_M_OPT_CONST("PROXYAUTH", CURLOPT_PROXYAUTH);
	RB_DEFINE_M_OPT_CONST("IPRESOLVE", CURLOPT_IPRESOLVE);
	RB_DEFINE_M_OPT_CONST("POSTREDIR", CURLOPT_POSTREDIR);

	RB_DEFINE_M_INFO_CONST("EFFECTIVE_URL", CURLINFO_EFFECTIVE_URL);
	RB_DEFINE_M_INFO_CONST("RESPONSE_CODE", CURLINFO_RESPONSE_CODE);
	RB_DEFINE_M_INFO_CONST("HTTP_CONNECTCODE", CURLINFO_HTTP_CONNECTCODE);
	RB_DEFINE_M_INFO_CONST("HTTP_VERSION", CURLINFO_HTTP_VERSION);
	RB_DEFINE_M_INFO_CONST("FILETIME", CURLINFO_FILETIME);
	RB_DEFINE_M_INFO_CONST("TOTAL_TIME", CURLINFO_TOTAL_TIME);
	RB_DEFINE_M_INFO_CONST("NAMELOOKUP_TIME", CURLINFO_NAMELOOKUP_TIME);
	RB_DEFINE_M_INFO_CONST("CONNECT_TIME", CURLINFO_CONNECT_TIME);
	RB_DEFINE_M_INFO_CONST("PRETRANSFER_TIME", CURLINFO_PRETRANSFER_TIME);
	RB_DEFINE_M_INFO_CONST("STARTTRANSFER_TIME", CURLINFO_STARTTRANSFER_TIME);
	RB_DEFINE_M_INFO_CONST("REDIRECT_TIME", CURLINFO_REDIRECT_TIME);
	RB_DEFINE_M_INFO_CONST("REDIRECT_COUNT", CURLINFO_REDIRECT_COUNT);
	RB_DEFINE_M_INFO_CONST("REDIRECT_URL", CURLINFO_REDIRECT_URL);
	RB_DEFINE_M_INFO_CONST("SIZE_UPLOAD", CURLINFO_SIZE_UPLOAD);
	RB_DEFINE_M_INFO_CONST("SIZE_DOWNLOAD", CURLINFO_SIZE_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("SPEED_UPLOAD", CURLINFO_SPEED_UPLOAD);
	RB_DEFINE_M_INFO_CONST("SPEED_DOWNLOAD", CURLINFO_SPEED_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("HEADER_SIZE", CURLINFO_HEADER_SIZE);
	RB_DEFINE_M_INFO_CONST("REQUEST_SIZE", CURLINFO_REQUEST_SIZE);
	RB_DEFINE_M_INFO_CONST("SSL_VERIFYRESULT", CURLINFO_SSL_VERIFYRESULT);
	RB_DEFINE_M_INFO_CONST("PROXY_SSL_VERIFYRESULT", CURLINFO_PROXY_SSL_VERIFYRESULT);
	RB_DEFINE_M_INFO_CONST("SSL_ENGINES", CURLINFO_SSL_ENGINES);
	RB_DEFINE_M_INFO_CONST("CONTENT_TYPE", CURLINFO_CONTENT_TYPE);
	RB_DEFINE_M_INFO_CONST("HTTPAUTH_AVAIL", CURLINFO_HTTPAUTH_AVAIL);
	RB_DEFINE_M_INFO_CONST("PROXYAUTH_AVAIL", CURLINFO_PROXYAUTH_AVAIL);
	RB_DEFINE_M_INFO_CONST("OS_ERRNO", CURLINFO_OS_ERRNO);
	RB_DEFINE_M_INFO_CONST("NUM_CONNECTS", CURLINFO_NUM_CONNECTS);
	RB_DEFINE_M_INFO_CONST("PRIMARY_IP", CURLINFO_PRIMARY_IP);
	RB_DEFINE_M_INFO_CONST("PRIMARY_PORT", CURLINFO_PRIMARY_PORT);
	RB_DEFINE_M_INFO_CONST("LOCAL_IP", CURLINFO_LOCAL_IP);
	RB_DEFINE_M_INFO_CONST("LOCAL_PORT", CURLINFO_LOCAL_PORT);
	RB_DEFINE_M_INFO_CONST("COOKIELIST", CURLINFO_COOKIELIST);
	RB_DEFINE_M_INFO_CONST("LASTSOCKET", CURLINFO_LASTSOCKET);
	RB_DEFINE_M_INFO_CONST("ACTIVESOCKET", CURLINFO_ACTIVESOCKET);
	RB_DEFINE_M_INFO_CONST("CERTINFO", CURLINFO_CERTINFO);
	RB_DEFINE_M_INFO_CONST("PROTOCOL", CURLINFO_PROTOCOL);
	RB_DEFINE_M_INFO_CONST("SCHEME", CURLINFO_SCHEME);
	RB_DEFINE_M_INFO_CONST("CONTENT_LENGTH_DOWNLOAD", CURLINFO_CONTENT_LENGTH_DOWNLOAD);
	RB_DEFINE_M_INFO_CONST("CONTENT_LENGTH_UPLOAD", CURLINFO_CONTENT_LENGTH_UPLOAD);
	
	RB_DEFINE_M_AUTH_CONST("BASIC", CURLAUTH_BASIC);
	RB_DEFINE_M_AUTH_CONST("DIGEST", CURLAUTH_DIGEST);
	RB_DEFINE_M_AUTH_CONST("DIGEST_IE", CURLAUTH_DIGEST_IE);
	RB_DEFINE_M_AUTH_CONST("BEARER", CURLAUTH_BEARER);
	RB_DEFINE_M_AUTH_CONST("NEGOTIATE", CURLAUTH_NEGOTIATE);
	RB_DEFINE_M_AUTH_CONST("NTLM", CURLAUTH_NTLM);
	RB_DEFINE_M_AUTH_CONST("NTLM_WB", CURLAUTH_NTLM_WB);
	RB_DEFINE_M_AUTH_CONST("ANY", CURLAUTH_ANY);
	RB_DEFINE_M_AUTH_CONST("ANYSAFE", CURLAUTH_ANYSAFE);
	RB_DEFINE_M_AUTH_CONST("ONLY", CURLAUTH_ONLY);
	
	RB_DEFINE_M_IPResolve_CONST("WHATEVER", CURL_IPRESOLVE_WHATEVER);
	RB_DEFINE_M_IPResolve_CONST("V4", CURL_IPRESOLVE_V4);
	RB_DEFINE_M_IPResolve_CONST("V6", CURL_IPRESOLVE_V6);
	
	/* appending C for code before int code to enable assignment */
	RB_DEFINE_M_RedirPost_CONST("C301", CURL_REDIR_POST_301);
	RB_DEFINE_M_RedirPost_CONST("C302", CURL_REDIR_POST_302);
	RB_DEFINE_M_RedirPost_CONST("C303", CURL_REDIR_POST_303);
	RB_DEFINE_M_RedirPost_CONST("ALL", CURL_REDIR_POST_ALL);

	Init_easy();
}
