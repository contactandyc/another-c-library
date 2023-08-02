#ifndef _AC_TOPIC_H
#define _AC_TOPIC_H

#include "ac_pool.h"
#include "ac_json.h"

struct ac_topic_message_s;
typedef struct ac_topic_message_s ac_topic_message_t;

struct ac_topic_parsed_message_s;
typedef struct ac_topic_parsed_message_s ac_topic_parsed_message_t;

struct ac_topic_s;
typedef struct ac_topic_s ac_topic_t;

/*
    Parse a message from JSON into a structure. The memory for the
    structure is allocated from the given memory pool.

    The format of the JSON is
    {
      "ts": 1622464709, // timestamp
      "p": 2,           // partition (1 based, zero means all partitions)
      "t": "topic_name",
      "k": "key1",      // this is the primary key, often used for partitioning
      "v": "value1",    // this is the payload or value associated with the message
      "h": {            // headers allow for named key/value pairs to be associated with the message
        "key2": "value2",
        "key3": "value3"
      }
    }

    The headers are optional.  The timestamp is optional, and if not provided, the current time is used.
    The partition is optional, and if not provided, message is routed to all partitions.
*/
ac_topic_parsed_message_t *ac_topic_parse_message(ac_pool_t *pool, ac_json_t *json);

// Dump (print) the contents of a parsed message for debugging purposes.
void ac_topic_parsed_message_dump(ac_topic_parsed_message_t *message);

// Initialize a new topic with the given name, number of partitions and replicas,
// and size of the memory pool for the topic.
ac_topic_t *ac_topic_init(const char *name, int num_partitions, int num_replicas, int pool_size);

// Add a parsed message to a topic. Returns the offset (message id)
// at which the message was added.
uint64_t ac_topic_add(ac_topic_t *topic, ac_topic_parsed_message_t *parsed_message);

// Cleanup
void ac_topic_destroy(ac_topic_t *topic);

#endif // _AC_TOPIC_H
