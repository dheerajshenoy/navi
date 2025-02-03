#!/bin/sh

NEW_VERSION=$1

if [[ "$NEW_VERSION" == "" ]]; then
    echo "Version number is empty. Stopping"
fi

file1="src/main.cpp"
file2="resources/navi.desktop" 
file3="rtd/conf.py"
file4="latest-version.txt"

sed -i "s/\(static const QString VERSION = \"\)v[0-9]\+\.[0-9]\+\.[0-9]\+\(\";\)/\1v$NEW_VERSION\2/" "$file1" && echo "Changed $file1"
sed -i "s/^Version=[0-9]\+\.[0-9]\+\.[0-9]\+/Version=$NEW_VERSION/" "$file2" && echo "Changed $file2"
sed -i "s/^version = '[0-9]\+\.[0-9]\+\.[0-9]\+'/version = '$NEW_VERSION'/" "$file3" && echo "Changed $file3"
echo v$NEW_VERSION > "$file4" && echo "Changed $file4"

