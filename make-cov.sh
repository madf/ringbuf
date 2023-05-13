#!/bin/sh

CFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0 -ggdb3" CXXFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0 -ggdb3" LDFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0" make ${1}
