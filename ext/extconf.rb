require 'mkmf'

dir_config('curl')

if find_executable('curl-config')
  $CFLAGS << " #{`curl-config --cflags`.strip} -g"
  if ENV['STATIC_BUILD']
    $LIBS << " #{`curl-config --static-libs`.strip}"
  else
    $LIBS << " #{`curl-config --libs`.strip}"
  end
  ca_bundle_path=`curl-config --ca`.strip
  if !ca_bundle_path.nil? and ca_bundle_path != ''
    $defs.push( %{-D HAVE_CURL_CONFIG_CA} )
    $defs.push( %{-D CURL_CONFIG_CA='#{ca_bundle_path.inspect}'} )
  end
elsif !have_library('curl') or !have_header('curl/curl.h')
  fail <<-EOM
  Can't find libcurl or curl/curl.h

  Try passing --with-curl-dir or --with-curl-lib and --with-curl-include
  options to extconf.
  EOM
end

create_makefile('curl')
