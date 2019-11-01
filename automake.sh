#!/bin/bash

glibtoolize
aclocal
autoconf
automake --add-missing
