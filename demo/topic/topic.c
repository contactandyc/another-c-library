#include <stdio.h>

#include "ac_topic.h"
#include "ac_pool.h"
#include "ac_io.h"

int main(int argc, char *argv[]) {
    ac_topic_t *topic = ac_topic_init("topic", 2, 1, 16384);
    ac_pool_t *pool = ac_pool_init(16384);
    for( int i=1; i<argc; i++) {
        size_t length;
        char *buffer = ac_io_read_file(&length, argv[i]);
        ac_pool_clear(pool);
        ac_json_t *json = ac_json_parse(pool, buffer, buffer+length);  // replace with your JSON parsing function
        if(ac_json_is_error(json)) {
            ac_json_dump_error(stdout, json);
            ac_free(buffer);
            continue;
        }
        // Use the ac_topic_parse_message function
        ac_topic_parsed_message_t *parsedMessage = ac_topic_parse_message(pool, json);

        ac_topic_add(topic, parsedMessage);

        // Print out the message using the dump function
        ac_topic_parsed_message_dump(parsedMessage);
        ac_free(buffer);
    }
    // Clean up here as necessary
    ac_pool_destroy(pool);

    ac_topic_dump(topic);
    ac_topic_destroy(topic);
    return 0;
}