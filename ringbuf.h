#ifndef RingBuf_H
#define RingBuf_H

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RingBuf_st RingBuf;

RingBuf* RingBuf_init(size_t size);
void RingBuf_free(RingBuf* rb);

size_t RingBuf_read(RingBuf* rb, void* buf, size_t size);
size_t RingBuf_write(RingBuf* rb, const void* buf, size_t size);
int RingBuf_is_empty(const RingBuf* rb);

#ifdef  __cplusplus
}
#endif

#endif
