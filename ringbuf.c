#include "ringbuf.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RingBuf_st
{
    void* data;
    size_t size;
    size_t rSize;
    void* rPtr;
    void* wPtr;
};

static uint8_t* bytePtr(void* ptr)
{
    return ptr;
}

static const uint8_t* bytePtrC(const void* ptr)
{
    return ptr;
}

static void* ptrAdd(void* ptr, size_t d)
{
    return bytePtr(ptr) + d;
}

static const void* ptrAddC(const void* ptr, size_t d)
{
    return bytePtrC(ptr) + d;
}

static size_t ptrDiff(void* a, void* b)
{
    return bytePtr(a) - bytePtr(b);
}

static int ptrLE(void* a, void* b)
{
    return bytePtr(a) <= bytePtr(b);
}

static int ptrLess(void* a, void* b)
{
    return bytePtr(a) < bytePtr(b);
}

RingBuf* RingBuf_init(size_t size)
{
    RingBuf* rb = malloc(sizeof(RingBuf));
    rb->data = malloc(size);
    rb->size = size;
    rb->rSize = 0;
    rb->rPtr = rb->data;
    rb->wPtr = rb->data;
    return rb;
}

void RingBuf_free(RingBuf* rb)
{
    free(rb->data);
    free(rb);
}

size_t RingBuf_read(RingBuf* rb, void* buf, size_t size)
{
    size_t toRead = rb->rSize;
    size_t partRead = 0;
    if (toRead > size)
        toRead = size;

    if (ptrLess(rb->rPtr, rb->wPtr))
    {
        // Normal
        partRead = ptrDiff(rb->wPtr, rb->rPtr);
        if (toRead > partRead)
            toRead = partRead;
        memcpy(buf, rb->rPtr, toRead);
        rb->rPtr = ptrAdd(rb->rPtr, toRead);
        rb->rSize -= toRead;
        return toRead;
    }

    // Wrapped
    partRead = rb->size - ptrDiff(rb->rPtr, rb->data);
    if (toRead == partRead)
    {
        memcpy(buf, rb->rPtr, toRead);
        rb->rPtr = rb->data; // Wrap to begin
        rb->rSize -= toRead;
        return toRead;
    }

    if (toRead < partRead)
    {
        memcpy(buf, rb->rPtr, toRead);
        rb->rPtr = ptrAdd(rb->rPtr, toRead);
        rb->rSize -= toRead;
        return toRead;
    }

    // Part one
    memcpy(buf, rb->rPtr, partRead);
    rb->rPtr = rb->data; // Wrap to begin
    rb->rSize -= partRead;

    // Part two
    return partRead + RingBuf_read(rb, ptrAdd(buf, partRead), size - partRead);
}

size_t RingBuf_write(RingBuf* rb, const void* buf, size_t size)
{
    size_t res = 0;
    size_t toWrite = 0;

    // Special case when buffer is full
    if (rb->rPtr == rb->wPtr && rb->rSize > 0)
        return 0;

    if (!ptrLE(rb->rPtr, rb->wPtr))
    {
        // Wrapped
        toWrite = ptrDiff(rb->rPtr, rb->wPtr);
        if (toWrite > size)
            toWrite = size;
        memcpy(rb->wPtr, buf, toWrite);
        rb->wPtr = ptrAdd(rb->wPtr, toWrite);
        rb->rSize += toWrite;
        return toWrite;
    }

    // Normal
    toWrite = rb->size - ptrDiff(rb->wPtr, rb->data);
    if (toWrite == size)
    {
        memcpy(rb->wPtr, buf, toWrite);
        rb->wPtr = rb->data; // Wrap to begin
        rb->rSize += toWrite;
        return toWrite;
    }

    if (toWrite > size)
    {
        toWrite = size;
        memcpy(rb->wPtr, buf, toWrite);
        rb->wPtr = ptrAdd(rb->wPtr, toWrite);
        rb->rSize += toWrite;
        return toWrite;
    }

    // Part one
    memcpy(rb->wPtr, buf, toWrite);
    rb->wPtr = rb->data;
    buf = ptrAddC(buf, toWrite);
    res = toWrite;
    size -= toWrite;

    // Part two
    toWrite = ptrDiff(rb->rPtr, rb->wPtr);
    if (toWrite > size)
        toWrite = size;
    memcpy(rb->wPtr, buf, toWrite);
    rb->wPtr = ptrAdd(rb->wPtr, toWrite);
    res += toWrite;

    rb->rSize += res;

    return res;
}

int RingBuf_is_empty(const RingBuf* rb)
{
    return rb->rPtr == rb->wPtr && rb->rSize == 0 ? 1 : 0;
}

#ifdef  __cplusplus
}
#endif
