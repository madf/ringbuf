#!/bin/sh

CFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0" CXXFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0" LDFLAGS="-fprofile-arcs -ftest-coverage -fPIC -O0" make ${1}
