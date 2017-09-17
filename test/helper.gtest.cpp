#include <gtest/gtest.h>
#include "helper.hpp"

TEST(helper, xmlescape) {
    auto input = "\n\"&'<>";
    auto result = px::xmlescape(input);
    ASSERT_EQ(result, "&#xA;&quot;&amp;&apos;&lt;&gt;");
    ASSERT_EQ(input, px::xmlunescape(result));
}

//TEST(helper, xmlunescape) {
////    auto input = "&Auml;&Ouml;&Uuml;&sz;&auml;&ouml;&uuml;";
//    auto result = px::xmlunescape(input);
//    ASSERT_EQ(result, "ÄÖÜßäöü");
//}
