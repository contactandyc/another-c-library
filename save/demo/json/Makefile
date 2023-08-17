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

demo: ../tools/side_by_side canada.json twitter.json citm_catalog.json
	@./ac_json_test 500 *.json > ac_json.out
	@./rapidjson_test 500 *.json > rapidjson.out
	../tools/side_by_side ac_json.out rapidjson.out

../tools/side_by_side: ../tools/side_by_side.c
	cd ../tools && make && cd -

web:
	@../tools/side_by_side ac_json.out rapidjson.out | ../tools/webify.sh


clean:
	rm -rf *~ *.dSYM rapidjson_test ac_json_test *.out

distclean: clean
	rm -rf rapidjson canada.json twitter.json citm_catalog.json
