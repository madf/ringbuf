DEFS = -DBOOST_TEST_DYN_LINK -DBOOST_UNIT_TEST_FRAMEWORK_DYN_LINK -DBOOST_UNIT_TEST_FRAMEWORK_NO_LIB
CFLAGS = $(DEFS) -ggdb3 -W -Wall -Wextra -fsanitize=address
CXXFLAGS = $(DEFS) -ggdb3 -W -Wall -Wextra -std=c++17 -fsanitize=address
LDFLAGS = -fsanitize=address -lboost_unit_test_framework

SOURCES = test.cpp \
	  ringbuf.c

.PHONY: all clean

all: test

test: test.o ringbuf.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm *.o *.d test

ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean)
-include $(subst .cpp,.cpp.d,$(subst .c,.c.d,$(SOURCES)))
endif
endif

%.cpp.d: %.cpp
	@$(CXX) -MM $(CXXFLAGS) $< > $@

%.c.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@
