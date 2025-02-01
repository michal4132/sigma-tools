#ifndef _NVRAM_CORE_H_
#define _NVRAM_CORE_H_

#include "bcmnvram.h"

// Internal NVRAM functions
char *_nvram_get(const char *name);
int _nvram_set(const char *name, const char *value, int is_temp);
int _nvram_unset(const char *name);
int _nvram_getall(char *buf, int count, int include_temp);
int _nvram_init(struct nvram_header *header);
void _nvram_uninit(void);
int _nvram_generate(struct nvram_header *header, int rehash);

// Core NVRAM functions
struct nvram_tuple *_nvram_realloc(struct nvram_tuple *t, const char *name,
        const char *value, int is_temp);
void _nvram_free(struct nvram_tuple *t);
void _nvram_reset(void);

// NVRAM interface functions
int nvram_init(char *img_fname, int size);
void nvram_deinit(void);
int nvram_commit(void);
int nvram_clear(void);
int nvram_erase(void);
int nvram_set(const char *name, const char *value, int attr);
int nvram_unset(const char *name);
int nvram_get(const char *name, char *value);
int nvram_getall(char *buf, int count, int include_temp);

// NVRAM encoding/decoding
int nvram_encode(const char *in, char **out);
int nvram_decode(const char *in, char **out);
int kernel_nvram_decode(const char *in, char *out);

// Device operations
int dev_nvram_init(void);
void dev_nvram_exit(void);

// Debug functions
void nvram_dump(char *header);
int nvram_show(void);

#endif /* _NVRAM_CORE_H_ */
