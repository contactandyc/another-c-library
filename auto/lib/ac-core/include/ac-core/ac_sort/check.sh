#!/bin/bash

diff ac_sort_m.h ac_sort_compare_m.h
diff ac_sort_arg_m.h ac_sort_compare_arg_m.h
sed "s/, arg//g" ac_sort_compare_arg_m.h > x.h
diff -b x.h ac_sort_m.h
rm x.h
