#!/bin/sh

./make-cov.sh
./test
gcovr --html-details ringbuf-coverage.html
