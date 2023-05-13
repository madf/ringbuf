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
    private:
        RingBuf* m_rb;
};

}

BOOST_AUTO_TEST_SUITE(RBTests)

BOOST_AUTO_TEST_CASE(Normal)
{
    RB rb(1024);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty());
    BOOST_CHECK_EQUAL(rb.read(3), "abc");
    BOOST_CHECK(rb.empty());
}

BOOST_AUTO_TEST_CASE(Wrap)
{
    RB rb(4);
    BOOST_CHECK(rb.empty());
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty()); // abc*
    BOOST_CHECK_EQUAL(rb.write("123"), 1);
    BOOST_CHECK(!rb.empty()); // abc1
    BOOST_CHECK_EQUAL(rb.read(2), "ab");
    BOOST_CHECK(!rb.empty()); // **c1
    BOOST_CHECK_EQUAL(rb.read(2), "c1");
    BOOST_CHECK(rb.empty()); // ****
    BOOST_CHECK_EQUAL(rb.write("abc"), 3);
    BOOST_CHECK(!rb.empty()); // abc*
    BOOST_CHECK_EQUAL(rb.read(2), "ab");
    BOOST_CHECK(!rb.empty()); // **c*
    BOOST_CHECK_EQUAL(rb.write("123"), 3);
    BOOST_CHECK(!rb.empty()); // 23c1
    BOOST_CHECK_EQUAL(rb.read(4), "c123");
    BOOST_CHECK(rb.empty()); // ****
}

BOOST_AUTO_TEST_SUITE_END()
