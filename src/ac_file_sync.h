#ifndef _ac_file_sync_h
#define _ac_file_sync_h

#include "ac_pool.h"
#include "ac_in.h"

/* Used to synchronize files from a local and remote source where all files are line delimited
   and are keyed by an id,...

   directory/
      <id>.json[.gz]
      <id>.json[.gz]
      local
      remote
      checkpoint

   sync_list.json

   sync_list.json points to a list of directories which have zero or more <id>.json files (with
   an optional .gz extension for compression).  Each file is labeled with the first id within
   the file.  The files are expected to be newline delimited and have the format of <id>,{json}

   local represents the maximum id locally
   remote represents the maximum id in the remote store
   checkpoint is used to indicate that a certain amount of data has been processed

   if writable is true, then this entry can be written to

   Future goal: replace this with kafka streams
*/

typedef struct {
    char *type;
    char *path;
    bool gzip;
    bool writable;
    uint32_t *files;
    uint32_t num_files;

    uint32_t buffer_size; // buffer_size in KB (defaults to 16, min 16)
    uint32_t index;
    uint32_t step; // 0 or the number of ids per file
    uint32_t checkpoint; // used for incremental reading
    uint32_t local;      // the local maximum
    uint32_t remote;     // the remote maximum
} ac_file_sync_entry_t;

typedef struct {
    ac_pool_t *pool;
    char *base_path;
    ac_file_sync_entry_t *entries;
    size_t num_entries;
} ac_file_sync_t;

ac_file_sync_t *ac_file_sync_init(const char *filename, const char *type);
void ac_file_sync_destroy(ac_file_sync_t *h);

/* is there data beyond the checkpoints */
bool ac_file_sync_has_data(ac_pool_t *pool, ac_file_sync_t *h, const char *type);

/* read data after the last checkpoint */
ac_in_t *ac_file_sync_read_from_checkpoint(ac_pool_t *pool, ac_file_sync_t *h, const char *type);

/* read all data regardless of checkpoints */
ac_in_t *ac_file_sync_read_from_start(ac_pool_t *pool, ac_file_sync_t *h, const char *type);

/* append data to the given type and update the checkpoint, local */
void ac_file_sync_append(ac_pool_t *pool, ac_file_sync_t *h, const char *type,
                         const char *data, size_t len );

/* update checkpoint to match local for given type */
void ac_file_sync_checkpoint(ac_pool_t *pool, ac_file_sync_t *h, const char *type);

#endif