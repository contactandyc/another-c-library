/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _xxx_H
#define _xxx_H

struct xxx_s;
typedef struct xxx_s xxx_t;

xxx_t * xxx_init(int param1, int param2);
void xxx_destroy( xxx_t *h );

void xxx_do_something( xxx_t *h, const char *prefix );

#endif
