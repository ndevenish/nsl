#!/bin/sh
#autoreconf --force --install -I config
rm -f config.cache
aclocal
autoconf
touch README AUTHORS NEWS ChangeLog
automake -a
exit