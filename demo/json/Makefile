ROOT=../..
include $(ROOT)/src/Makefile.include

FLAGS += -O3 -lz -I./rapidjson/include

all: rapidjson_test ac_json_test demo

rapidjson_test: rapidjson rapidjson_test.cc $(OBJECTS)
	g++ $(FLAGS) $(OBJECTS) rapidjson_test.cc -o rapidjson_test

ac_json_test: ac_json_test.c $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) ac_json_test.c -o ac_json_test

rapidjson:
	git clone https://github.com/Tencent/rapidjson.git

canada.json:
	curl -s "https://raw.githubusercontent.com/serde-rs/json-benchmark/master/data/canada.json" -o canada.json

twitter.json:
	curl -s "https://raw.githubusercontent.com/serde-rs/json-benchmark/master/data/twitter.json" -o twitter.json

citm_catalog.json:
	curl -s "https://raw.githubusercontent.com/serde-rs/json-benchmark/master/data/citm_catalog.json" -o citm_catalog.json

demo: canada.json twitter.json citm_catalog.json
	@echo Testing ac_json
	./ac_json_test 500 *.json
	@echo
	@echo
	@echo Testing RapidJSON
	./rapidjson_test 500 *.json
	@echo
	@echo

web: canada.json twitter.json citm_catalog.json
	@echo Testing ac_json
	./ac_json_test --web 500 *.json
	@echo
	@echo Testing RapidJSON
	./rapidjson_test --web 500 *.json
	@echo


clean:
	rm -rf *~ *.dSYM rapidjson_test ac_json_test

distclean: clean
	rm -rf rapidjson canada.json twitter.json citm_catalog.json
