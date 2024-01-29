#include <gtest/gtest.h>
#include <string>
#include "Utils.h"
// Demonstrate some basic assertions.
TEST(UtilsTest, Split) {
    std::string example = "Hello,World";

    std::vector<std::string> result = Utils::split(example, ",");

    EXPECT_EQ(result.size(), 2);
    EXPECT_STREQ(result[0].c_str(), "Hello");
    EXPECT_STREQ(result[1].c_str(), "World");
}

TEST(UtilsTest, ChopLine) {
    std::string example = "localhost:8123";

    std::pair<std::string, std::string> result = Utils::ChopLine(example);

    EXPECT_STREQ(result.first.c_str(), "localhost");
    EXPECT_STREQ(result.second.c_str(), "8123");
}


TEST(UtilsTest, printStringWithEscapes) {
    std::string example = "Hello\nWorld";

    std::string result = Utils::printStringWithEscapes(example);

    EXPECT_STREQ(result.c_str(), "Hello\\nWorld");
}
