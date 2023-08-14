#!/bin/bash

diff ac_search_m.h ac_search_compare_m.h
diff ac_search_arg_m.h ac_search_compare_arg_m.h
sed "s/, arg//g" ac_search_compare_arg_m.h > x.h
diff -b x.h ac_search_m.h
rm x.h
