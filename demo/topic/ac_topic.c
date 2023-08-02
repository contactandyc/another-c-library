#include "ac_topic.h"
#include "ac_allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ac_topic_parsed_header_s {
    char *key;
    char *value;
    size_t value_length;
} ac_topic_parsed_header_t;

struct ac_topic_parsed_message_s {
    char *topic;
    int32_t partition;
    ac_topic_parsed_header_t *headers;
    uint32_t num_headers;
    uint64_t timestamp;
};

struct ac_topic_message_s {
    uint64_t length : 28;
    uint64_t timestamp : 36;
    uint64_t offset;
    ac_topic_message_t *next;
    ac_topic_message_t *prev;
};

ac_topic_parsed_message_t *ac_topic_parse_message(ac_pool_t *pool, ac_json_t *json) {
    uint32_t ts = 0;
    int partition;
    char *timestamp = ac_jsonv(ac_jsono_scan(json, "ts"));
    if(!timestamp || sscanf(timestamp, "%u", &ts) != 1) {
        return NULL;
    }
    char *value = ac_jsonv(ac_jsono_scan(json, "p"));
    if(!value || sscanf(value, "%d", &partition) != 1) {
        partition = -1;
    }

    ac_topic_parsed_message_t *message = ac_pool_alloc(pool, sizeof(ac_topic_parsed_message_t));
    message->timestamp = ts;
    ac_json_t *header_json = ac_jsono_scan(json, "h");
    message->topic = ac_jsond(pool, ac_jsono_scan(json, "t"));
    char *key = ac_jsonv(ac_jsono_scan(json, "k"));
    value = ac_jsonv(ac_jsono_scan(json, "v"));
    int num_headers = ac_jsono_count(header_json);
    message->num_headers = num_headers+1;
    ac_topic_parsed_header_t *headers = ac_pool_alloc(pool, sizeof(ac_topic_parsed_header_t) * message->num_headers);
    message->headers = headers;
    headers->key = ac_json_decode(pool, key, key ? strlen(key) : 0);
    headers->value = ac_json_decode2(&(headers->value_length), pool, value, value ? strlen(value) : 0);
    headers++;
    ac_jsono_t *n = ac_jsono_first(header_json);
    while(n) {
        key = n->key;
        value = ac_jsonv(n->value);
        headers->key = ac_json_decode(pool, key, key ? strlen(key) : 0);
        headers->value = ac_json_decode2(&(headers->value_length), pool, value, value ? strlen(value) : 0);
        headers++;
        n = ac_jsono_next(n);
    }
    message->partition = partition+1;
    return message;
}

void ac_topic_parsed_message_dump(ac_topic_parsed_message_t *message) {
    if (message == NULL) {
        printf("Null message\n");
        return;
    }

    printf("Topic: %s\n", message->topic);
    printf("Partition: %d\n", message->partition);
    printf("Timestamp: %llu\n", message->timestamp);
    printf("Number of Headers: %u\n", message->num_headers);

    for (uint32_t i = 0; i < message->num_headers; i++) {
        printf("Header %u\n", i + 1);
        printf("\tKey: %s\n", message->headers[i].key);
        printf("\tValue: %s\n", message->headers[i].value);
        printf("\tValue Length: %zu\n", message->headers[i].value_length);
    }
}


static uint8_t* write_vlq(uint8_t* dest, uint64_t value) {
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) /* More bytes to come */
            byte |= 0x80;
        *dest++ = byte;
    } while (value != 0);

    return dest;
}

static size_t vlq_size(uint64_t value) {
    size_t size = 0;
    do {
        value >>= 7;
        size++;
    } while (value != 0);

    return size;
}

static ac_topic_message_t* ac_topic_create_message_from_parsed(ac_pool_t *pool, ac_topic_parsed_message_t* parsed_message) {
    size_t payload_size = vlq_size(parsed_message->partition) + vlq_size(parsed_message->num_headers);

    for (uint32_t i = 0; i < parsed_message->num_headers; i++) {
        ac_topic_parsed_header_t* header = parsed_message->headers+i;
        payload_size += strlen(header->key) + 1 + vlq_size(header->value_length) + header->value_length;
    }

    ac_topic_message_t* message = ac_pool_alloc(pool,sizeof(ac_topic_message_t) + payload_size);
    message->timestamp = parsed_message->timestamp;
    uint8_t* payload = (uint8_t*)(message + 1);
    uint8_t* dest = payload;

    dest = write_vlq(dest, parsed_message->partition);
    dest = write_vlq(dest, parsed_message->num_headers);

    for (uint32_t i = 0; i < parsed_message->num_headers; i++) {
        ac_topic_parsed_header_t* header = parsed_message->headers+i;
        size_t key_len = strlen(header->key);

        memcpy(dest, header->key, key_len + 1);
        dest += key_len + 1;

        dest = write_vlq(dest, header->value_length);
        memcpy(dest, header->value, header->value_length);
        dest += header->value_length;
    }

    message->length = payload_size;
    message->next = NULL;
    message->prev = NULL;
    return message;
}

typedef struct ac_topic_partition_s {
    ac_pool_t *pool;
    ac_topic_message_t *head;
    ac_topic_message_t *tail;
    uint64_t offset;

    // each topic has a list of subscribers
    // ac_subscriber_t *subscribers;
    // ac_subscriber_t *last_subscriber;
    uint32_t num_subscribers;
} ac_topic_partition_t;

struct ac_topic_s {
    char *name;
    ac_topic_partition_t *partitions;
    int num_partitions;
    int num_replicas;
    int pool_size;
};

ac_topic_t *ac_topic_init(const char *name, int num_partitions, int num_replicas, int pool_size) {
    ac_topic_t *topic = (ac_topic_t *)ac_calloc(sizeof(ac_topic_t) + strlen(name)+1 + (sizeof(ac_topic_partition_t) * num_partitions));
    topic->partitions = (ac_topic_partition_t *)(topic+1);
    topic->name = (char *)(topic->partitions+num_partitions);
    strcpy(topic->name, name);
    topic->num_partitions = num_partitions;
    topic->num_replicas = num_replicas;
    topic->pool_size = pool_size;

    for( int i=0; i<num_partitions; i++ ) {
        topic->partitions[i].pool = ac_pool_init(pool_size / num_partitions);
        topic->partitions[i].head = NULL;
        topic->partitions[i].tail = NULL;
        topic->partitions[i].offset = 0;
        // topic->partitions[i].subscribers = NULL;
        // topic->partitions[i].last_subscriber = NULL;
        topic->partitions[i].num_subscribers = 0;
    }
    return topic;
}

static uint64_t ac_topic_add_to_partition(ac_topic_t *topic, ac_topic_parsed_message_t *parsed_message, int partition) {
    ac_topic_partition_t *p = topic->partitions + partition;
    ac_topic_message_t *message =
      ac_topic_create_message_from_parsed(p->pool, parsed_message);
    message->offset = p->offset++;
    if (p->head == NULL) {
        p->head = message;
        p->tail = message;
    } else {
        p->tail->next = message;
        message->prev = p->tail;
        p->tail = message;
    }
    return message->offset;
}

uint64_t ac_topic_add(ac_topic_t *topic, ac_topic_parsed_message_t *parsed_message) {
    if(parsed_message->partition == 0) {
        // add message to all partitions
        uint64_t offset = 0;
        for( int i=0; i<topic->num_partitions; i++ ) {
            offset = ac_topic_add_to_partition(topic, parsed_message, i);
        }
        return offset;
    }
    else if(parsed_message->partition <= topic->num_partitions)
        return ac_topic_add_to_partition(topic, parsed_message, parsed_message->partition-1);
    else
        return 0;
}

void ac_topic_destroy(ac_topic_t *topic) {
    for( int i=0; i<topic->num_partitions; i++ ) {
        ac_pool_destroy(topic->partitions[i].pool);
    }
    ac_free(topic);
}
