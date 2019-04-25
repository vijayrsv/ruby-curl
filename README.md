[![Gem Version](https://badge.fury.io/rb/ruby-curl.svg)](https://badge.fury.io/rb/ruby-curl)


# Introduction

Ruby-curl aims to be a true libcurl binding for Ruby. Currently, ruby-curl offers full support for the HTTP protocol over libcurl’s easy interface. Just like libcurl, when using ruby-curl you init your easy-session and get a handle, which you use as input to the following interface functions you use.



# Requirements

```
Ruby 1.8 or higher

Libcurl 7.19.0 or higher
```


# Installation

Bundler:
```
Gem ‘ruby-curl’
```

Manual:
```
Gem install ruby-curl
```



# Usage:

```
require 'ruby-curl'

curl = Curl::Easy.new
curl.setopt(Curl::Opt::URL, "https://example.com")

res = curl.perform
# <!doctype html>...

res
# => 0 # CURLE_OK

curl.cleanup
```
