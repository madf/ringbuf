#define BOOST_TEST_MODULE RingBufTests

#include "ringbuf.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include <iostream>
#include <vector>

namespace
{

class RB
{
    public:
        RB(size_t size) noexcept
            : m_rb(RingBuf_init(size))
        {}
        ~RB()
        {
            RingBuf_free(m_rb);
        }

        size_t size() const noexcept
        {
            return RingBuf_size(m_rb);
        }

        std::string read(size_t size) noexcept
        {
            std::vector<char> buf(size);
            auto res = RingBuf_read(m_rb, buf.data(), buf.size());
            return std::string(buf.data(), res);
        }

        size_t write(const std::string& data) noexcept
        {
            return RingBuf_write(m_rb, data.data(), data.size());
        }

        bool empty() const noexcept
        {
            return RingBuf_is_empty(m_rb);
        }

        size_t bytesToRead() const noexcept
        {
            return RingBuf_bytes_to_read(m_rb);
        }

        size_t bytesToWrite() const noexcept
        {
            return RingBuf_bytes_to_write(m_rb);
        }

        void* wptr() noexcept
        {
            return RingBuf_wptr(m_rb);
        }

        size_t maxWriteBytes() const noexcept
        {
            return RingBuf_max_write_size(m_rb);
        }

        size_t advanceWPtr(size_t offset) noexcept
        {
            return RingBuf_advance_wptr(m_rb, offset);
        }

        const void* rptr() const noexcept
        {
            return RingBuf_rptr(m_rb);
        }
    private:
        RingBuf* m_rb;
};

void writeTo(void* buf, const std::string& data)
{
    memcpy(buf, data.data(), data.size());
}

std::string readFrom(const void* buf, size_t size)
{
    return {static_cast<const char*>(buf), size};
}

}

BOOST_AUTO_TEST_SUITE(RBTests)

BOOST_AUTO_TEST_CASE(Normal)
{
    RB rb(1024);
    BOOST_CHECK_EQUAL(rb.size(), 1024);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1024);
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 3);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1021);
    BOOST_CHECK_EQUAL(rb.read(3), "abc");
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1024);
    BOOST_CHECK(rb.empty());
}

BOOST_AUTO_TEST_CASE(Wrap)
{
    RB rb(4);
    BOOST_CHECK_EQUAL(rb.size(), 4);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty()); // abc*
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 3);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1);
    BOOST_CHECK_EQUAL(rb.write("123"), 1);
    BOOST_CHECK(!rb.empty()); // abc1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(2), "ab");
    BOOST_CHECK(!rb.empty()); // **c1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 2);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 2);
    BOOST_CHECK_EQUAL(rb.read(2), "c1");
    BOOST_CHECK(rb.empty()); // ****
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty()); // abc*
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 3);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1);
    BOOST_CHECK_EQUAL(rb.read(2), "ab");
    BOOST_CHECK(!rb.empty()); // **c*
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 1);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 3);
    BOOST_CHECK_EQUAL(rb.write("123"), 3);
    BOOST_CHECK(!rb.empty()); // 23c1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "c123");
    BOOST_CHECK(rb.empty()); // ****
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty()); // abc*
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 3);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 1);
    BOOST_CHECK_EQUAL(rb.write("123"), 1);
    BOOST_CHECK(!rb.empty()); // abc1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(2), "ab");
    BOOST_CHECK(!rb.empty()); // **c1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 2);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 2);
    BOOST_CHECK_EQUAL(rb.write("23"), 2);
    BOOST_CHECK(!rb.empty()); // 23c1
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "c123");
    BOOST_CHECK(rb.empty()); // ****
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
}

BOOST_AUTO_TEST_CASE(EdgeWrite)
{
    RB rb(4);
    BOOST_CHECK_EQUAL(rb.size(), 4);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("ab"), 2);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 2);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 2);
    BOOST_CHECK_EQUAL(rb.read(4), "ab");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "abcd");
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
}

BOOST_AUTO_TEST_CASE(WriteLimit)
{
    RB rb(4);
    BOOST_CHECK_EQUAL(rb.size(), 4);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd12"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(6), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("ab"), 2);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 2);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 2);
    BOOST_CHECK_EQUAL(rb.read(4), "ab");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd12"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(6), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
}

BOOST_AUTO_TEST_CASE(Full)
{
    RB rb(4);
    BOOST_CHECK_EQUAL(rb.size(), 4);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
    BOOST_CHECK_EQUAL(rb.write("abcd12"), 4);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.write("34"), 0);
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 4);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 0);
    BOOST_CHECK_EQUAL(rb.read(4), "abcd");
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.bytesToRead(), 0);
    BOOST_CHECK_EQUAL(rb.bytesToWrite(), 4);
}

BOOST_AUTO_TEST_CASE(Sequential)
{
    RB rb(4);
    BOOST_CHECK_EQUAL(rb.size(), 4);
    BOOST_CHECK(rb.empty());
}

BOOST_AUTO_TEST_SUITE_END()
