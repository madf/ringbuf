#ifndef RingBuf_H
#define RingBuf_H

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RingBuf_st RingBuf;

/** @brief Constructor
 *  @param size ring buffer size.
 *  @returns a pointer to the ring buffer structure.
 */
RingBuf* RingBuf_init(size_t size);

/** @brief Destructor
 *  @param rb ring buffer pointer.
 */
void RingBuf_free(RingBuf* rb);

/** @brief Ring buffer size accessor
 *  @param rb ring buffer pointer.
 *  @returns ring buffer size.
 */
size_t RingBuf_size(const RingBuf* rb);


/** @brief Read bytes from a ring buffer
 *  @param rb ring buffer pointer.
 *  @param buf buffer pointer to read data into.
 *  @param size buffer size.
 *  @returns actual number of bytes read.
 *  @note The function reads up to the specified buffer size bytes. The function may read less than the specified read buffer capacity if the ring buffer does not hold as much data to read.
 */
size_t RingBuf_read(RingBuf* rb, void* buf, size_t size);

/** @brief Write bytes to a ring buffer.
 *  @param rb ring buffer pointer.
 *  @param buf buffer pointer to get bytes from.
 *  @param size number of bytes to write.
 *  @returns actual number of bytes written.
 *  @note The function may write less bytes than asked if the ring buffer does not have enought available free space.
 */
size_t RingBuf_write(RingBuf* rb, const void* buf, size_t size);
/** @brief Check if a ring buffer does not contain data.
 *  @param rb ring buffer pointer.
 *  @returns Non-zero if the ring buffer is empty, zero otherwise.
 */
int RingBuf_is_empty(const RingBuf* rb);
/** @brief Get a number of bytes available for reading in a ring buffer.
 *  @param rb ring buffer pointer.
 *  @returns the number of bytes available for reading.
 */
size_t RingBuf_bytes_to_read(const RingBuf* rb);
/** @brief Get a number of bytes available for writing in a ring buffer.
 *  @param rb ring buffer pointer.
 *  @returns the number of bytes available for writing.
 */
size_t RingBuf_bytes_to_write(const RingBuf* rb);

/** @brief Get a pointer for writing data into a ring buffer.
 *  @param rb ring buffer pointer.
 *  @returns a pointer for writing data.
 *  @note this is an unsafe operation, it allows writing more data than there is free space available in the ring buffer.
 */
void* RingBuf_wptr(RingBuf* rb);
/** @brief Get a size of free space available for writing at the writing pointer.
 *  @param rb ring buffer pointer.
 *  @returns a number of bytes that can be written using a writing pointer.
 */
size_t RingBuf_max_write_size(const RingBuf* rb);
/** @brief Shift writing pointer to the specified offset.
 *  @param rb rin buffer pointer.
 *  @param offset number of bytes to shift the pointer to.
 *  @returns actual number of bytes the pointer was shift to.
 *  @note If the offset is greater than the maximum number of bytes available for writing, this function shifts the writing pointer to the reading pointer or the beginning of the ring buffer and returns the actual number of bytes the pointer was shift to.
 */
size_t RingBuf_advance_wptr(RingBuf* rb, size_t offset);

/** @brief Get a pointer for reading data from a ring buffer.
 *  @param rb ring buffer pointer.
 *  @returns a pointer for reading data from a ring buffer.
 *  @note this is an unsafe operation, it allows reading more data than is available for reading in the ring buffer.
 */
const void* RingBuf_rptr(const RingBuf* rb);
/** @brief Get a number of bytes available for reading from the reading pointer.
 *  @param rb ring buffer pointer.
 *  @returns a number of bytes that can be read from the reading pointer.
 */
size_t RingBuf_max_read_size(const RingBuf* rb);
/** @brief Shift reading pointer to the specified offset.
 *  @param rb ring buffer pointer.
 *  @param offset number of bytes to shift the pointer to.
 *  @returns actual number of bytes the pointer was shift to.
 *  @note If the offset is greater than the maximum number of bytes available for reading, this function shifts the reading pointer to the writing pointer or the beginning of the ring buffer and returns the actual number of bytes the pointer was shift to.
 */
size_t RingBuf_advance_rptr(RingBuf* rb, size_t offset);

#ifdef  __cplusplus
}
#endif

#endif
