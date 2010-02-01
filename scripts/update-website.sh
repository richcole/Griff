#!/bin/bash

pushd /home/groups/g/gr/griff/web-site
cvs update -P -d
ruby txt_to_html.rb
cp *.{html,jpg,png} ../htdocs
cp style/*.{css,jpg} ../htdocs/style
popd
find /home/groups/g/gr/griff/htdocs/ -type f -exec chmod 0664 {} \;
find /home/groups/g/gr/griff/htdocs/ -type d -exec chmod 2775 {} \;

