#include "curl.h"

VALUE mCurl;

void Init_curl() {
 mCurl = rb_define_module("Curl");

 init_easy();
}
