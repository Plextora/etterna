/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 3.1.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2018 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "catch.hpp"

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <fstream>

TEST_CASE("UBJSON")
{
    SECTION("individual values")
    {
        SECTION("discarded")
        {
            // discarded values are not serialized
            json j = json::value_t::discarded;
            const auto result = json::to_ubjson(j);
            CHECK(result.empty());
        }

        SECTION("null")
        {
            json j = nullptr;
            std::vector<uint8_t> expected = {'Z'};
            const auto result = json::to_ubjson(j);
            CHECK(result == expected);

            // roundtrip
            CHECK(json::from_ubjson(result) == j);
        }

        SECTION("boolean")
        {
            SECTION("true")
            {
                json j = true;
                std::vector<uint8_t> expected = {'T'};
                const auto result = json::to_ubjson(j);
                CHECK(result == expected);

                // roundtrip
                CHECK(json::from_ubjson(result) == j);
            }

            SECTION("false")
            {
                json j = false;
                std::vector<uint8_t> expected = {'F'};
                const auto result = json::to_ubjson(j);
                CHECK(result == expected);

                // roundtrip
                CHECK(json::from_ubjson(result) == j);
            }
        }

        SECTION("number")
        {
            SECTION("signed")
            {
                SECTION("-9223372036854775808..-2147483649 (int64)")
                {
                    std::vector<int64_t> numbers;
                    numbers.push_back((std::numeric_limits<int64_t>::min)());
                    numbers.push_back(-1000000000000000000ll);
                    numbers.push_back(-100000000000000000ll);
                    numbers.push_back(-10000000000000000ll);
                    numbers.push_back(-1000000000000000ll);
                    numbers.push_back(-100000000000000ll);
                    numbers.push_back(-10000000000000ll);
                    numbers.push_back(-1000000000000ll);
                    numbers.push_back(-100000000000ll);
                    numbers.push_back(-10000000000ll);
                    numbers.push_back(-2147483649ll);
                    for (auto i : numbers)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('L'));
                        expected.push_back(static_cast<uint8_t>((i >> 56) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 48) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 40) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 32) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 24) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 16) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        int64_t restored = (static_cast<int64_t>(result[1]) << 070) +
                                           (static_cast<int64_t>(result[2]) << 060) +
                                           (static_cast<int64_t>(result[3]) << 050) +
                                           (static_cast<int64_t>(result[4]) << 040) +
                                           (static_cast<int64_t>(result[5]) << 030) +
                                           (static_cast<int64_t>(result[6]) << 020) +
                                           (static_cast<int64_t>(result[7]) << 010) +
                                           static_cast<int64_t>(result[8]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("-2147483648..-32769 (int32)")
                {
                    std::vector<int32_t> numbers;
                    numbers.push_back(-32769);
                    numbers.push_back(-100000);
                    numbers.push_back(-1000000);
                    numbers.push_back(-10000000);
                    numbers.push_back(-100000000);
                    numbers.push_back(-1000000000);
                    numbers.push_back(-2147483648);
                    for (auto i : numbers)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('l'));
                        expected.push_back(static_cast<uint8_t>((i >> 24) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 16) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        int32_t restored = (static_cast<int32_t>(result[1]) << 030) +
                                           (static_cast<int32_t>(result[2]) << 020) +
                                           (static_cast<int32_t>(result[3]) << 010) +
                                           static_cast<int32_t>(result[4]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("-32768..-129 (int16)")
                {
                    for (int32_t i = -32768; i <= -129; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('I'));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        int16_t restored = static_cast<int16_t>(((result[1] << 8) + result[2]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("-9263 (int16)")
                {
                    json j = -9263;
                    std::vector<uint8_t> expected = {'I', 0xdb, 0xd1};

                    // compare result + size
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);
                    CHECK(result.size() == 3);

                    // check individual bytes
                    CHECK(result[0] == 'I');
                    int16_t restored = static_cast<int16_t>(((result[1] << 8) + result[2]));
                    CHECK(restored == -9263);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("-128..-1 (int8)")
                {
                    for (auto i = -128; i <= -1; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('i');
                        expected.push_back(static_cast<uint8_t>(i));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        CHECK(static_cast<int8_t>(result[1]) == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("0..127 (int8)")
                {
                    for (size_t i = 0; i <= 127; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('i'));
                        expected.push_back(static_cast<uint8_t>(i));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        CHECK(result[1] == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("128..255 (uint8)")
                {
                    for (size_t i = 128; i <= 255; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('U'));
                        expected.push_back(static_cast<uint8_t>(i));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'U');
                        CHECK(result[1] == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("256..32767 (int16)")
                {
                    for (size_t i = 256; i <= 32767; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<uint8_t>('I'));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("65536..2147483647 (int32)")
                {
                    for (uint32_t i :
                            {
                                65536u, 77777u, 1048576u
                            })
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('l');
                        expected.push_back(static_cast<uint8_t>((i >> 24) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 16) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        uint32_t restored = (static_cast<uint32_t>(result[1]) << 030) +
                                            (static_cast<uint32_t>(result[2]) << 020) +
                                            (static_cast<uint32_t>(result[3]) << 010) +
                                            static_cast<uint32_t>(result[4]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("2147483648..9223372036854775807 (int64)")
                {
                    std::vector<uint64_t> v = {2147483648ul, 9223372036854775807ul};
                    for (uint64_t i : v)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('L');
                        expected.push_back(static_cast<uint8_t>((i >> 070) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 060) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 050) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 040) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 030) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 020) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 010) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        uint64_t restored = (static_cast<uint64_t>(result[1]) << 070) +
                                            (static_cast<uint64_t>(result[2]) << 060) +
                                            (static_cast<uint64_t>(result[3]) << 050) +
                                            (static_cast<uint64_t>(result[4]) << 040) +
                                            (static_cast<uint64_t>(result[5]) << 030) +
                                            (static_cast<uint64_t>(result[6]) << 020) +
                                            (static_cast<uint64_t>(result[7]) << 010) +
                                            static_cast<uint64_t>(result[8]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }
            }

            SECTION("unsigned")
            {
                SECTION("0..127 (int8)")
                {
                    for (size_t i = 0; i <= 127; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with unsigned integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('i');
                        expected.push_back(static_cast<uint8_t>(i));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        uint8_t restored = static_cast<uint8_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("128..255 (uint8)")
                {
                    for (size_t i = 128; i <= 255; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with unsigned integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('U');
                        expected.push_back(static_cast<uint8_t>(i));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'U');
                        uint8_t restored = static_cast<uint8_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("256..32767 (int16)")
                {
                    for (size_t i = 256; i <= 32767; ++i)
                    {
                        CAPTURE(i);

                        // create JSON value with unsigned integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('I');
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("65536..2147483647 (int32)")
                {
                    for (uint32_t i :
                            {
                                65536u, 77777u, 1048576u
                            })
                    {
                        CAPTURE(i);

                        // create JSON value with unsigned integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('l');
                        expected.push_back(static_cast<uint8_t>((i >> 24) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 16) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        uint32_t restored = (static_cast<uint32_t>(result[1]) << 030) +
                                            (static_cast<uint32_t>(result[2]) << 020) +
                                            (static_cast<uint32_t>(result[3]) << 010) +
                                            static_cast<uint32_t>(result[4]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }

                SECTION("2147483648..9223372036854775807 (int64)")
                {
                    std::vector<uint64_t> v = {2147483648ul, 9223372036854775807ul};
                    for (uint64_t i : v)
                    {
                        CAPTURE(i);

                        // create JSON value with integer number
                        json j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back('L');
                        expected.push_back(static_cast<uint8_t>((i >> 070) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 060) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 050) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 040) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 030) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 020) & 0xff));
                        expected.push_back(static_cast<uint8_t>((i >> 010) & 0xff));
                        expected.push_back(static_cast<uint8_t>(i & 0xff));

                        // compare result + size
                        const auto result = json::to_ubjson(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        uint64_t restored = (static_cast<uint64_t>(result[1]) << 070) +
                                            (static_cast<uint64_t>(result[2]) << 060) +
                                            (static_cast<uint64_t>(result[3]) << 050) +
                                            (static_cast<uint64_t>(result[4]) << 040) +
                                            (static_cast<uint64_t>(result[5]) << 030) +
                                            (static_cast<uint64_t>(result[6]) << 020) +
                                            (static_cast<uint64_t>(result[7]) << 010) +
                                            static_cast<uint64_t>(result[8]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_ubjson(result) == j);
                    }
                }
            }

            SECTION("float64")
            {
                SECTION("3.1415925")
                {
                    double v = 3.1415925;
                    json j = v;
                    std::vector<uint8_t> expected =
                    {
                        'D', 0x40, 0x09, 0x21, 0xfb, 0x3f, 0xa6, 0xde, 0xfc
                    };
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                    CHECK(json::from_ubjson(result) == v);
                }
            }
        }

        SECTION("string")
        {
            SECTION("N = 0..127")
            {
                for (size_t N = 0; N <= 127; ++N)
                {
                    CAPTURE(N);

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json j = s;

                    // create expected byte vector
                    std::vector<uint8_t> expected;
                    expected.push_back('S');
                    expected.push_back('i');
                    expected.push_back(static_cast<uint8_t>(N));
                    for (size_t i = 0; i < N; ++i)
                    {
                        expected.push_back('x');
                    }

                    // compare result + size
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 3);
                    // check that no null byte is appended
                    if (N > 0)
                    {
                        CHECK(result.back() != '\x00');
                    }

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("N = 128..255")
            {
                for (size_t N = 128; N <= 255; ++N)
                {
                    CAPTURE(N);

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json j = s;

                    // create expected byte vector
                    std::vector<uint8_t> expected;
                    expected.push_back('S');
                    expected.push_back('U');
                    expected.push_back(static_cast<uint8_t>(N));
                    for (size_t i = 0; i < N; ++i)
                    {
                        expected.push_back('x');
                    }

                    // compare result + size
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 3);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("N = 256..32767")
            {
                for (size_t N :
                        {
                            256u, 999u, 1025u, 3333u, 2048u, 32767u
                        })
                {
                    CAPTURE(N);

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json j = s;

                    // create expected byte vector (hack: create string first)
                    std::vector<uint8_t> expected(N, 'x');
                    // reverse order of commands, because we insert at begin()
                    expected.insert(expected.begin(), static_cast<uint8_t>(N & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 8) & 0xff));
                    expected.insert(expected.begin(), 'I');
                    expected.insert(expected.begin(), 'S');

                    // compare result + size
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 4);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("N = 65536..2147483647")
            {
                for (size_t N :
                        {
                            65536u, 77777u, 1048576u
                        })
                {
                    CAPTURE(N);

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json j = s;

                    // create expected byte vector (hack: create string first)
                    std::vector<uint8_t> expected(N, 'x');
                    // reverse order of commands, because we insert at begin()
                    expected.insert(expected.begin(), static_cast<uint8_t>(N & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 8) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 16) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 24) & 0xff));
                    expected.insert(expected.begin(), 'l');
                    expected.insert(expected.begin(), 'S');

                    // compare result + size
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 6);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }
        }

        SECTION("array")
        {
            SECTION("empty")
            {
                SECTION("size=false type=false")
                {
                    json j = json::array();
                    std::vector<uint8_t> expected = {'[', ']'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = json::array();
                    std::vector<uint8_t> expected = {'[', '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = json::array();
                    std::vector<uint8_t> expected = {'[', '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("[null]")
            {
                SECTION("size=false type=false")
                {
                    json j = {nullptr};
                    std::vector<uint8_t> expected = {'[', 'Z', ']'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = {nullptr};
                    std::vector<uint8_t> expected = {'[', '#', 'i', 1, 'Z'};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = {nullptr};
                    std::vector<uint8_t> expected = {'[', '$', 'Z', '#', 'i', 1};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("[1,2,3,4,5]")
            {
                SECTION("size=false type=false")
                {
                    json j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> expected = {'[', 'i', 1, 'i', 2, 'i', 3, 'i', 4, 'i', 5, ']'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> expected = {'[', '#', 'i', 5, 'i', 1, 'i', 2, 'i', 3, 'i', 4, 'i', 5};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> expected = {'[', '$', 'i', '#', 'i', 5, 1, 2, 3, 4, 5};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("[[[[]]]]")
            {
                SECTION("size=false type=false")
                {
                    json j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> expected = {'[', '[', '[', '[', ']', ']', ']', ']'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> expected = {'[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> expected = {'[', '$', '[', '#', 'i', 1, '$', '[', '#', 'i', 1, '$', '[', '#', 'i', 1, '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("array with uint16_t elements")
            {
                SECTION("size=false type=false")
                {
                    json j(257, nullptr);
                    std::vector<uint8_t> expected(j.size() + 2, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[258] = ']'; // closing array
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j(257, nullptr);
                    std::vector<uint8_t> expected(j.size() + 5, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[1] = '#'; // array size
                    expected[2] = 'I'; // int16
                    expected[3] = 0x01; // 0x0101, first byte
                    expected[4] = 0x01; // 0x0101, second byte
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j(257, nullptr);
                    std::vector<uint8_t> expected = {'[', '$', 'Z', '#', 'I', 0x01, 0x01};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("array with uint32_t elements")
            {
                SECTION("size=false type=false")
                {
                    json j(65793, nullptr);
                    std::vector<uint8_t> expected(j.size() + 2, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[65794] = ']'; // closing array
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j(65793, nullptr);
                    std::vector<uint8_t> expected(j.size() + 7, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[1] = '#'; // array size
                    expected[2] = 'l'; // int32
                    expected[3] = 0x00; // 0x00010101, first byte
                    expected[4] = 0x01; // 0x00010101, second byte
                    expected[5] = 0x01; // 0x00010101, third byte
                    expected[6] = 0x01; // 0x00010101, fourth byte
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j(65793, nullptr);
                    std::vector<uint8_t> expected = {'[', '$', 'Z', '#', 'l', 0x00, 0x01, 0x01, 0x01};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }
        }

        SECTION("object")
        {
            SECTION("empty")
            {
                SECTION("size=false type=false")
                {
                    json j = json::object();
                    std::vector<uint8_t> expected = {'{', '}'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = json::object();
                    std::vector<uint8_t> expected = {'{', '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = json::object();
                    std::vector<uint8_t> expected = {'{', '#', 'i', 0};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("{\"\":null}")
            {
                SECTION("size=false type=false")
                {
                    json j = {{"", nullptr}};
                    std::vector<uint8_t> expected = {'{', 'i', 0, 'Z', '}'};
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = {{"", nullptr}};
                    std::vector<uint8_t> expected = {'{', '#', 'i', 1, 'i', 0, 'Z'};
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = {{"", nullptr}};
                    std::vector<uint8_t> expected = {'{', '$', 'Z', '#', 'i', 1, 'i', 0};
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }

            SECTION("{\"a\": {\"b\": {\"c\": {}}}}")
            {
                SECTION("size=false type=false")
                {
                    json j = json::parse("{\"a\": {\"b\": {\"c\": {}}}}");
                    std::vector<uint8_t> expected =
                    {
                        '{', 'i', 1, 'a', '{', 'i', 1, 'b', '{', 'i', 1, 'c', '{', '}', '}', '}', '}'
                    };
                    const auto result = json::to_ubjson(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=false")
                {
                    json j = json::parse("{\"a\": {\"b\": {\"c\": {}}}}");
                    std::vector<uint8_t> expected =
                    {
                        '{', '#', 'i', 1, 'i', 1, 'a', '{', '#', 'i', 1, 'i', 1, 'b', '{', '#', 'i', 1, 'i', 1, 'c', '{', '#', 'i', 0
                    };
                    const auto result = json::to_ubjson(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }

                SECTION("size=true type=true")
                {
                    json j = json::parse("{\"a\": {\"b\": {\"c\": {}}}}");
                    std::vector<uint8_t> expected =
                    {
                        '{', '$', '{', '#', 'i', 1, 'i', 1, 'a', '$', '{', '#', 'i', 1, 'i', 1, 'b', '$', '{', '#', 'i', 1, 'i', 1, 'c', '#', 'i', 0
                    };
                    const auto result = json::to_ubjson(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_ubjson(result) == j);
                }
            }
        }
    }

    SECTION("errors")
    {
        SECTION("strict mode")
        {
            std::vector<uint8_t> vec = {'Z', 'Z'};
            SECTION("non-strict mode")
            {
                const auto result = json::from_ubjson(vec, false);
                CHECK(result == json());
            }

            SECTION("strict mode")
            {
                CHECK_THROWS_AS(json::from_ubjson(vec), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(vec),
                                  "[json.exception.parse_error.110] parse error at 2: expected end of input");
            }
        }

        SECTION("number out of range")
        {
            // larger than max int64
            json j = 9223372036854775808llu;
            CHECK_THROWS_AS(json::to_ubjson(j), json::out_of_range&);
            CHECK_THROWS_WITH(json::to_ubjson(j), "[json.exception.out_of_range.407] number overflow serializing 9223372036854775808");
        }
    }

    SECTION("parsing values")
    {
        SECTION("strings")
        {
            // create a single-character string for all number types
            std::vector<uint8_t> s_i = {'S', 'i', 1, 'a'};
            std::vector<uint8_t> s_U = {'S', 'U', 1, 'a'};
            std::vector<uint8_t> s_I = {'S', 'I', 0, 1, 'a'};
            std::vector<uint8_t> s_l = {'S', 'l', 0, 0, 0, 1, 'a'};
            std::vector<uint8_t> s_L = {'S', 'L', 0, 0, 0, 0, 0, 0, 0, 1, 'a'};

            // check if string is parsed correctly to "a"
            CHECK(json::from_ubjson(s_i) == "a");
            CHECK(json::from_ubjson(s_U) == "a");
            CHECK(json::from_ubjson(s_I) == "a");
            CHECK(json::from_ubjson(s_l) == "a");
            CHECK(json::from_ubjson(s_L) == "a");

            // roundtrip: output should be optimized
            CHECK(json::to_ubjson(json::from_ubjson(s_i)) == s_i);
            CHECK(json::to_ubjson(json::from_ubjson(s_U)) == s_i);
            CHECK(json::to_ubjson(json::from_ubjson(s_I)) == s_i);
            CHECK(json::to_ubjson(json::from_ubjson(s_l)) == s_i);
            CHECK(json::to_ubjson(json::from_ubjson(s_L)) == s_i);
        }

        SECTION("number")
        {
            SECTION("float")
            {
                // float32
                std::vector<uint8_t> v_d = {'d', 0x40, 0x49, 0x0f, 0xd0};
                CHECK(json::from_ubjson(v_d) == 3.14159f);

                // float64
                std::vector<uint8_t> v_D = {'D', 0x40, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e};
                CHECK(json::from_ubjson(v_D) == 3.14159);

                // float32 is serialized as float64 as the library does not support float32
                CHECK(json::to_ubjson(json::from_ubjson(v_d)) == json::to_ubjson(3.14159f));
            }
        }

        SECTION("array")
        {
            SECTION("optimized version (length only)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> v_T = {'[', '#', 'i', 2, 'T', 'T'};
                std::vector<uint8_t> v_F = {'[', '#', 'i', 2, 'F', 'F'};
                std::vector<uint8_t> v_Z = {'[', '#', 'i', 2, 'Z', 'Z'};
                std::vector<uint8_t> v_i = {'[', '#', 'i', 2, 'i', 0x7F, 'i', 0x7F};
                std::vector<uint8_t> v_U = {'[', '#', 'i', 2, 'U', 0xFF, 'U', 0xFF};
                std::vector<uint8_t> v_I = {'[', '#', 'i', 2, 'I', 0x7F, 0xFF, 'I', 0x7F, 0xFF};
                std::vector<uint8_t> v_l = {'[', '#', 'i', 2, 'l', 0x7F, 0xFF, 0xFF, 0xFF, 'l', 0x7F, 0xFF, 0xFF, 0xFF};
                std::vector<uint8_t> v_L = {'[', '#', 'i', 2, 'L', 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 'L', 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                std::vector<uint8_t> v_D = {'[', '#', 'i', 2, 'D', 0x40, 0x09, 0x21, 0xfb, 0x4d, 0x12, 0xd8, 0x4a, 'D', 0x40, 0x09, 0x21, 0xfb, 0x4d, 0x12, 0xd8, 0x4a};
                std::vector<uint8_t> v_S = {'[', '#', 'i', 2, 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> v_C = {'[', '#', 'i', 2, 'C', 'a', 'C', 'a'};

                // check if vector is parsed correctly
                CHECK(json::from_ubjson(v_T) == json({true, true}));
                CHECK(json::from_ubjson(v_F) == json({false, false}));
                CHECK(json::from_ubjson(v_Z) == json({nullptr, nullptr}));
                CHECK(json::from_ubjson(v_i) == json({127, 127}));
                CHECK(json::from_ubjson(v_U) == json({255, 255}));
                CHECK(json::from_ubjson(v_I) == json({32767, 32767}));
                CHECK(json::from_ubjson(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_ubjson(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_ubjson(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_ubjson(v_S) == json({"a", "a"}));
                CHECK(json::from_ubjson(v_C) == json({"a", "a"}));

                // roundtrip: output should be optimized
                CHECK(json::to_ubjson(json::from_ubjson(v_T), true) == v_T);
                CHECK(json::to_ubjson(json::from_ubjson(v_F), true) == v_F);
                CHECK(json::to_ubjson(json::from_ubjson(v_Z), true) == v_Z);
                CHECK(json::to_ubjson(json::from_ubjson(v_i), true) == v_i);
                CHECK(json::to_ubjson(json::from_ubjson(v_U), true) == v_U);
                CHECK(json::to_ubjson(json::from_ubjson(v_I), true) == v_I);
                CHECK(json::to_ubjson(json::from_ubjson(v_l), true) == v_l);
                CHECK(json::to_ubjson(json::from_ubjson(v_L), true) == v_L);
                CHECK(json::to_ubjson(json::from_ubjson(v_D), true) == v_D);
                CHECK(json::to_ubjson(json::from_ubjson(v_S), true) == v_S);
                CHECK(json::to_ubjson(json::from_ubjson(v_C), true) == v_S); // char is serialized to string
            }

            SECTION("optimized version (type and length)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> v_N = {'[', '$', 'N', '#', 'i', 2};
                std::vector<uint8_t> v_T = {'[', '$', 'T', '#', 'i', 2};
                std::vector<uint8_t> v_F = {'[', '$', 'F', '#', 'i', 2};
                std::vector<uint8_t> v_Z = {'[', '$', 'Z', '#', 'i', 2};
                std::vector<uint8_t> v_i = {'[', '$', 'i', '#', 'i', 2, 0x7F, 0x7F};
                std::vector<uint8_t> v_U = {'[', '$', 'U', '#', 'i', 2, 0xFF, 0xFF};
                std::vector<uint8_t> v_I = {'[', '$', 'I', '#', 'i', 2, 0x7F, 0xFF, 0x7F, 0xFF};
                std::vector<uint8_t> v_l = {'[', '$', 'l', '#', 'i', 2, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF};
                std::vector<uint8_t> v_L = {'[', '$', 'L', '#', 'i', 2, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                std::vector<uint8_t> v_D = {'[', '$', 'D', '#', 'i', 2, 0x40, 0x09, 0x21, 0xfb, 0x4d, 0x12, 0xd8, 0x4a, 0x40, 0x09, 0x21, 0xfb, 0x4d, 0x12, 0xd8, 0x4a};
                std::vector<uint8_t> v_S = {'[', '$', 'S', '#', 'i', 2, 'i', 1, 'a', 'i', 1, 'a'};
                std::vector<uint8_t> v_C = {'[', '$', 'C', '#', 'i', 2, 'a', 'a'};

                // check if vector is parsed correctly
                CHECK(json::from_ubjson(v_N) == json::array());
                CHECK(json::from_ubjson(v_T) == json({true, true}));
                CHECK(json::from_ubjson(v_F) == json({false, false}));
                CHECK(json::from_ubjson(v_Z) == json({nullptr, nullptr}));
                CHECK(json::from_ubjson(v_i) == json({127, 127}));
                CHECK(json::from_ubjson(v_U) == json({255, 255}));
                CHECK(json::from_ubjson(v_I) == json({32767, 32767}));
                CHECK(json::from_ubjson(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_ubjson(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_ubjson(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_ubjson(v_S) == json({"a", "a"}));
                CHECK(json::from_ubjson(v_C) == json({"a", "a"}));

                // roundtrip: output should be optimized
                std::vector<uint8_t> v_empty = {'[', '#', 'i', 0};
                CHECK(json::to_ubjson(json::from_ubjson(v_N), true, true) == v_empty);
                CHECK(json::to_ubjson(json::from_ubjson(v_T), true, true) == v_T);
                CHECK(json::to_ubjson(json::from_ubjson(v_F), true, true) == v_F);
                CHECK(json::to_ubjson(json::from_ubjson(v_Z), true, true) == v_Z);
                CHECK(json::to_ubjson(json::from_ubjson(v_i), true, true) == v_i);
                CHECK(json::to_ubjson(json::from_ubjson(v_U), true, true) == v_U);
                CHECK(json::to_ubjson(json::from_ubjson(v_I), true, true) == v_I);
                CHECK(json::to_ubjson(json::from_ubjson(v_l), true, true) == v_l);
                CHECK(json::to_ubjson(json::from_ubjson(v_L), true, true) == v_L);
                CHECK(json::to_ubjson(json::from_ubjson(v_D), true, true) == v_D);
                CHECK(json::to_ubjson(json::from_ubjson(v_S), true, true) == v_S);
                CHECK(json::to_ubjson(json::from_ubjson(v_C), true, true) == v_S); // char is serialized to string
            }
        }
    }

    SECTION("parse errors")
    {
        SECTION("empty byte vector")
        {
            CHECK_THROWS_AS(json::from_ubjson(std::vector<uint8_t>()), json::parse_error&);
            CHECK_THROWS_WITH(json::from_ubjson(std::vector<uint8_t>()),
                              "[json.exception.parse_error.110] parse error at 1: unexpected end of input");
        }

        SECTION("char")
        {
            SECTION("eof after C byte")
            {
                std::vector<uint8_t> v = {'C'};
                CHECK_THROWS_AS(json::from_ubjson(v), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(v), "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
            }

            SECTION("byte out of range")
            {
                std::vector<uint8_t> v = {'C', 130};
                CHECK_THROWS_AS(json::from_ubjson(v), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(v), "[json.exception.parse_error.113] parse error at 2: byte after 'C' must be in range 0x00..0x7F; last byte: 0x82");
            }
        }

        SECTION("strings")
        {
            SECTION("eof after S byte")
            {
                std::vector<uint8_t> v = {'S'};
                CHECK_THROWS_AS(json::from_ubjson(v), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(v), "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
            }

            SECTION("invalid byte")
            {
                std::vector<uint8_t> v = {'S', '1', 'a'};
                CHECK_THROWS_AS(json::from_ubjson(v), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(v), "[json.exception.parse_error.113] parse error at 2: expected a UBJSON string; last byte: 0x31");
            }
        }

        SECTION("array")
        {
            SECTION("optimized array: no size following type")
            {
                std::vector<uint8_t> v = {'[', '$', 'i', 2};
                CHECK_THROWS_AS(json::from_ubjson(v), json::parse_error&);
                CHECK_THROWS_WITH(json::from_ubjson(v), "[json.exception.parse_error.112] parse error at 4: expected '#' after UBJSON type information; last byte: 0x02");
            }
        }
    }

    SECTION("writing optimized values")
    {
        SECTION("integer")
        {
            SECTION("array of i")
            {
                json j = {1, -1};
                std::vector<uint8_t> expected = {'[', '$', 'i', '#', 'i', 2, 1, 0xff};
                CHECK(json::to_ubjson(j, true, true) == expected);
            }

            SECTION("array of U")
            {
                json j = {200, 201};
                std::vector<uint8_t> expected = {'[', '$', 'U', '#', 'i', 2, 0xC8, 0xC9};
                CHECK(json::to_ubjson(j, true, true) == expected);
            }

            SECTION("array of I")
            {
                json j = {30000, -30000};
                std::vector<uint8_t> expected = {'[', '$', 'I', '#', 'i', 2, 0x75, 0x30, 0x8a, 0xd0};
                CHECK(json::to_ubjson(j, true, true) == expected);
            }

            SECTION("array of l")
            {
                json j = {70000, -70000};
                std::vector<uint8_t> expected = {'[', '$', 'l', '#', 'i', 2, 0x00, 0x01, 0x11, 0x70, 0xFF, 0xFE, 0xEE, 0x90};
                CHECK(json::to_ubjson(j, true, true) == expected);
            }

            SECTION("array of L")
            {
                json j = {5000000000, -5000000000};
                std::vector<uint8_t> expected = {'[', '$', 'L', '#', 'i', 2, 0x00, 0x00, 0x00, 0x01, 0x2A, 0x05, 0xF2, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0xD5, 0xFA, 0x0E, 0x00};
                CHECK(json::to_ubjson(j, true, true) == expected);
            }
        }

        SECTION("unsigned integer")
        {
            SECTION("array of i")
            {
                json j = {1u, 2u};
                std::vector<uint8_t> expected = {'[', '$', 'i', '#', 'i', 2, 1, 2};
                std::vector<uint8_t> expected_size = {'[', '#', 'i', 2, 'i', 1, 'i', 2};
                CHECK(json::to_ubjson(j, true, true) == expected);
                CHECK(json::to_ubjson(j, true) == expected_size);
            }

            SECTION("array of U")
            {
                json j = {200u, 201u};
                std::vector<uint8_t> expected = {'[', '$', 'U', '#', 'i', 2, 0xC8, 0xC9};
                std::vector<uint8_t> expected_size = {'[', '#', 'i', 2, 'U', 0xC8, 'U', 0xC9};
                CHECK(json::to_ubjson(j, true, true) == expected);
                CHECK(json::to_ubjson(j, true) == expected_size);
            }

            SECTION("array of I")
            {
                json j = {30000u, 30001u};
                std::vector<uint8_t> expected = {'[', '$', 'I', '#', 'i', 2, 0x75, 0x30, 0x75, 0x31};
                std::vector<uint8_t> expected_size = {'[', '#', 'i', 2, 'I', 0x75, 0x30, 'I', 0x75, 0x31};
                CHECK(json::to_ubjson(j, true, true) == expected);
                CHECK(json::to_ubjson(j, true) == expected_size);
            }

            SECTION("array of l")
            {
                json j = {70000u, 70001u};
                std::vector<uint8_t> expected = {'[', '$', 'l', '#', 'i', 2, 0x00, 0x01, 0x11, 0x70, 0x00, 0x01, 0x11, 0x71};
                std::vector<uint8_t> expected_size = {'[', '#', 'i', 2, 'l', 0x00, 0x01, 0x11, 0x70, 'l', 0x00, 0x01, 0x11, 0x71};
                CHECK(json::to_ubjson(j, true, true) == expected);
                CHECK(json::to_ubjson(j, true) == expected_size);
            }

            SECTION("array of L")
            {
                json j = {5000000000u, 5000000001u};
                std::vector<uint8_t> expected = {'[', '$', 'L', '#', 'i', 2, 0x00, 0x00, 0x00, 0x01, 0x2A, 0x05, 0xF2, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2A, 0x05, 0xF2, 0x01};
                std::vector<uint8_t> expected_size = {'[', '#', 'i', 2, 'L', 0x00, 0x00, 0x00, 0x01, 0x2A, 0x05, 0xF2, 0x00, 'L', 0x00, 0x00, 0x00, 0x01, 0x2A, 0x05, 0xF2, 0x01};
                CHECK(json::to_ubjson(j, true, true) == expected);
                CHECK(json::to_ubjson(j, true) == expected_size);
            }
        }

        SECTION("discarded")
        {
            json j = {json::value_t::discarded, json::value_t::discarded};
            std::vector<uint8_t> expected = {'[', '$', 'N', '#', 'i', 2};
            CHECK(json::to_ubjson(j, true, true) == expected);
        }
    }
}

/*
// use this testcase outside [hide] to run it with Valgrind
TEST_CASE("single UBJSON roundtrip")
{
    SECTION("sample.json")
    {
        std::string filename = "test/data/json_testsuite/sample.json";

        // parse JSON file
        std::ifstream f_json(filename);
        json j1 = json::parse(f_json);

        // parse MessagePack file
        std::ifstream f_ubjson(filename + ".ubj", std::ios::binary);
        std::vector<uint8_t> packed((std::istreambuf_iterator<char>(f_ubjson)),
                                    std::istreambuf_iterator<char>());
        json j2;
        CHECK_NOTHROW(j2 = json::from_ubjson(packed));

        // compare parsed JSON values
        CHECK(j1 == j2);

        SECTION("roundtrips")
        {
            SECTION("std::ostringstream")
            {
                std::ostringstream ss;
                json::to_ubjson(j1, ss);
                json j3 = json::from_ubjson(ss.str());
                CHECK(j1 == j3);
            }

            SECTION("std::string")
            {
                std::string s;
                json::to_ubjson(j1, s);
                json j3 = json::from_ubjson(s);
                CHECK(j1 == j3);
            }
        }

        // check with different start index
        // packed.insert(packed.begin(), 5, 0xff);
        // CHECK(j1 == json::from_ubjson(packed.begin() + 5, packed.end()));
    }
}
*/

/*
TEST_CASE("UBJSON roundtrips", "[hide]")
{
    SECTION("input from py-ubjson")
    {
        for (std::string filename :
                {
                    "test/data/json_nlohmann_tests/all_unicode.json",
                    "test/data/json.org/1.json",
                    "test/data/json.org/2.json",
                    "test/data/json.org/3.json",
                    "test/data/json.org/4.json",
                    "test/data/json.org/5.json",
                    "test/data/json_roundtrip/roundtrip01.json",
                    "test/data/json_roundtrip/roundtrip02.json",
                    "test/data/json_roundtrip/roundtrip03.json",
                    "test/data/json_roundtrip/roundtrip04.json",
                    "test/data/json_roundtrip/roundtrip05.json",
                    "test/data/json_roundtrip/roundtrip06.json",
                    "test/data/json_roundtrip/roundtrip07.json",
                    "test/data/json_roundtrip/roundtrip08.json",
                    "test/data/json_roundtrip/roundtrip09.json",
                    "test/data/json_roundtrip/roundtrip10.json",
                    "test/data/json_roundtrip/roundtrip11.json",
                    "test/data/json_roundtrip/roundtrip12.json",
                    "test/data/json_roundtrip/roundtrip13.json",
                    "test/data/json_roundtrip/roundtrip14.json",
                    "test/data/json_roundtrip/roundtrip15.json",
                    "test/data/json_roundtrip/roundtrip16.json",
                    "test/data/json_roundtrip/roundtrip17.json",
                    "test/data/json_roundtrip/roundtrip18.json",
                    "test/data/json_roundtrip/roundtrip19.json",
                    "test/data/json_roundtrip/roundtrip20.json",
                    "test/data/json_roundtrip/roundtrip21.json",
                    "test/data/json_roundtrip/roundtrip22.json",
                    "test/data/json_roundtrip/roundtrip23.json",
                    "test/data/json_roundtrip/roundtrip24.json",
                    "test/data/json_roundtrip/roundtrip25.json",
                    "test/data/json_roundtrip/roundtrip26.json",
                    "test/data/json_roundtrip/roundtrip27.json",
                    "test/data/json_roundtrip/roundtrip28.json",
                    "test/data/json_roundtrip/roundtrip29.json",
                    "test/data/json_roundtrip/roundtrip30.json",
                    "test/data/json_roundtrip/roundtrip31.json",
                    "test/data/json_roundtrip/roundtrip32.json",
                    "test/data/json_testsuite/sample.json", // kills AppVeyor
                    "test/data/json_tests/pass1.json",
                    "test/data/json_tests/pass2.json",
                    "test/data/json_tests/pass3.json",
                    "test/data/regression/floats.json",
                    "test/data/regression/signed_ints.json",
                    "test/data/regression/unsigned_ints.json",
                    "test/data/regression/working_file.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_arraysWithSpaces.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_empty-string.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_empty.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_ending_with_newline.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_false.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_heterogeneous.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_null.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_with_1_and_newline.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_with_leading_space.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_with_several_null.json",
                    "test/data/nst_json_testsuite/test_parsing/y_array_with_trailing_space.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_0e+1.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_0e1.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_after_space.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_double_close_to_zero.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_double_huge_neg_exp.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_huge_exp.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_int_with_exp.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_minus_zero.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_negative_int.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_negative_one.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_negative_zero.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_neg_exp.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_pos_exp.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_exponent.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_fraction_exponent.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_neg_exp.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_real_neg_overflow.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_pos_exponent.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_real_pos_overflow.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_real_underflow.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_simple_int.json",
                    "test/data/nst_json_testsuite/test_parsing/y_number_simple_real.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_neg_int.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_pos_int.json",
                    //"test/data/nst_json_testsuite/test_parsing/y_number_very_big_negative_int.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_basic.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key_and_value.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_empty.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_empty_key.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_escaped_null_in_key.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_extreme_numbers.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_long_strings.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_simple.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_string_unicode.json",
                    "test/data/nst_json_testsuite/test_parsing/y_object_with_newlines.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_UTF-16_Surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pair.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pairs.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_allowed_escapes.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_and_u_escaped_zero.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_doublequotes.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_comments.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_a.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_n.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_control_character.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_noncharacter.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_in_array.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_in_array_with_leading_space.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_last_surrogates_1_and_2.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_newline_uescaped.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+1FFFF.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_null_escape.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_one-byte-utf-8.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_pi.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_simple_ascii.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_space.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_three-byte-utf-8.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_two-byte-utf-8.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_u+2028_line_sep.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_u+2029_par_sep.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_uEscape.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unescaped_char_delete.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicode.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicodeEscapedBackslash.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_2.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+2064_invisible_plus.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_escaped_double_quote.json",
                    // "test/data/nst_json_testsuite/test_parsing/y_string_utf16.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_utf8.json",
                    "test/data/nst_json_testsuite/test_parsing/y_string_with_del_character.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_false.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_int.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_negative_real.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_null.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_string.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_true.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_string_empty.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_trailing_newline.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_true_in_array.json",
                    "test/data/nst_json_testsuite/test_parsing/y_structure_whitespace_array.json"
                })
        {
            CAPTURE(filename);

            // parse JSON file
            std::ifstream f_json(filename);
            json j1 = json::parse(f_json);

            SECTION("std::vector<uint8_t>")
            {
                // parse UBJSON file
                std::ifstream f_ubjson(filename + ".ubj", std::ios::binary);
                std::vector<uint8_t> packed(
                    (std::istreambuf_iterator<char>(f_ubjson)),
                    std::istreambuf_iterator<char>());
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson(packed));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            SECTION("std::ifstream")
            {
                // parse UBJSON file
                std::ifstream f_ubjson(filename + ".ubj", std::ios::binary);
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson(f_ubjson));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            SECTION("uint8_t* and size")
            {
                // parse UBJSON file
                std::ifstream f_ubjson(filename + ".ubj", std::ios::binary);
                std::vector<uint8_t> packed(
                    (std::istreambuf_iterator<char>(f_ubjson)),
                    std::istreambuf_iterator<char>());
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson({packed.data(), packed.size()}));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }
        }
    }
}
*/

TEST_CASE("Universal Binary JSON Specification Examples 1")
{
    SECTION("Null Value")
    {
        json j = {{"passcode", nullptr}};
        std::vector<uint8_t> v = {'{', 'i', 8, 'p', 'a', 's', 's', 'c', 'o', 'd', 'e', 'Z', '}'};
        CHECK(json::to_ubjson(j) == v);
        CHECK(json::from_ubjson(v) == j);
    }

    SECTION("No-Op Value")
    {
        json j = {"foo", "bar", "baz"};
        std::vector<uint8_t> v = {'[', 'S', 'i', 3, 'f', 'o', 'o',
                                  'S', 'i', 3, 'b', 'a', 'r',
                                  'S', 'i', 3, 'b', 'a', 'z', ']'
                                 };
        std::vector<uint8_t> v2 = {'[', 'S', 'i', 3, 'f', 'o', 'o', 'N',
                                   'S', 'i', 3, 'b', 'a', 'r', 'N', 'N', 'N',
                                   'S', 'i', 3, 'b', 'a', 'z', 'N', 'N', ']'
                                  };
        CHECK(json::to_ubjson(j) == v);
        CHECK(json::from_ubjson(v) == j);
        CHECK(json::from_ubjson(v2) == j);
    }

    SECTION("Boolean Types")
    {
        json j = {{"authorized", true}, {"verified", false}};
        std::vector<uint8_t> v = {'{', 'i', 10, 'a', 'u', 't', 'h', 'o', 'r', 'i', 'z', 'e', 'd', 'T',
                                  'i', 8, 'v', 'e', 'r', 'i', 'f', 'i', 'e', 'd', 'F', '}'
                                 };
        CHECK(json::to_ubjson(j) == v);
        CHECK(json::from_ubjson(v) == j);
    }

    SECTION("Numeric Types")
    {
        json j =
        {
            {"int8", 16},
            {"uint8", 255},
            {"int16", 32767},
            {"int32", 2147483647},
            {"int64", 9223372036854775807},
            {"float64", 113243.7863123}
        };
        std::vector<uint8_t> v = {'{',
                                  'i', 7, 'f', 'l', 'o', 'a', 't', '6', '4', 'D', 0x40, 0xfb, 0xa5, 0xbc, 0x94, 0xbc, 0x34, 0xcf,
                                  'i', 5, 'i', 'n', 't', '1', '6', 'I', 0x7f, 0xff,
                                  'i', 5, 'i', 'n', 't', '3', '2', 'l', 0x7f, 0xff, 0xff, 0xff,
                                  'i', 5, 'i', 'n', 't', '6', '4', 'L', 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                  'i', 4, 'i', 'n', 't', '8', 'i', 16,
                                  'i', 5, 'u', 'i', 'n', 't', '8', 'U', 0xff,
                                  '}'
                                 };
        CHECK(json::to_ubjson(j) == v);
        CHECK(json::from_ubjson(v) == j);
    }

    SECTION("Char Type")
    {
        json j = {{"rolecode", "a"}, {"delim", ";"}};
        std::vector<uint8_t> v = {'{', 'i', 5, 'd', 'e', 'l', 'i', 'm', 'C', ';', 'i', 8, 'r', 'o', 'l', 'e', 'c', 'o', 'd', 'e', 'C', 'a', '}'};
        //CHECK(json::to_ubjson(j) == v);
        CHECK(json::from_ubjson(v) == j);
    }

    SECTION("String Type")
    {
        SECTION("English")
        {
            json j = "hello";
            std::vector<uint8_t> v = {'S', 'i', 5, 'h', 'e', 'l', 'l', 'o'};
            CHECK(json::to_ubjson(j) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("Russian")
        {
            json j = "привет";
            std::vector<uint8_t> v = {'S', 'i', 12, 0xD0, 0xBF, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82};
            CHECK(json::to_ubjson(j) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("Russian")
        {
            json j = "مرحبا";
            std::vector<uint8_t> v = {'S', 'i', 10, 0xD9, 0x85, 0xD8, 0xB1, 0xD8, 0xAD, 0xD8, 0xA8, 0xD8, 0xA7};
            CHECK(json::to_ubjson(j) == v);
            CHECK(json::from_ubjson(v) == j);
        }
    }

    SECTION("Array Type")
    {
        SECTION("size=false type=false")
        {
            // note the float has been replaced by a double
            json j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', 'Z', 'T', 'F', 'L', 0x00, 0x00, 0x00, 0x01, 0x1D, 0x0C, 0xCB, 0xE9, 'D', 0x40, 0x63, 0x24, 0x39, 0x58, 0x10, 0x62, 0x4e, 'S', 'i', 3, 'h', 'a', 'm', ']'};
            CHECK(json::to_ubjson(j) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("size=true type=false")
        {
            // note the float has been replaced by a double
            json j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', '#', 'i', 6, 'Z', 'T', 'F', 'L', 0x00, 0x00, 0x00, 0x01, 0x1D, 0x0C, 0xCB, 0xE9, 'D', 0x40, 0x63, 0x24, 0x39, 0x58, 0x10, 0x62, 0x4e, 'S', 'i', 3, 'h', 'a', 'm'};
            CHECK(json::to_ubjson(j, true) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("size=true type=true")
        {
            // note the float has been replaced by a double
            json j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', '#', 'i', 6, 'Z', 'T', 'F', 'L', 0x00, 0x00, 0x00, 0x01, 0x1D, 0x0C, 0xCB, 0xE9, 'D', 0x40, 0x63, 0x24, 0x39, 0x58, 0x10, 0x62, 0x4e, 'S', 'i', 3, 'h', 'a', 'm'};
            CHECK(json::to_ubjson(j, true, true) == v);
            CHECK(json::from_ubjson(v) == j);
        }
    }

    SECTION("Object Type")
    {
        SECTION("size=false type=false")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', 'i', 4, 'p', 'o', 's', 't', '{',
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x04, 0x71,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x00, 0x00, 0x01, 0x3D, 0xB1, 0x78, 0x66, 0x60,
                                      '}', '}'
                                     };
            CHECK(json::to_ubjson(j) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("size=true type=false")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', '#', 'i', 1, 'i', 4, 'p', 'o', 's', 't', '{', '#', 'i', 4,
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x04, 0x71,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x00, 0x00, 0x01, 0x3D, 0xB1, 0x78, 0x66, 0x60
                                     };
            CHECK(json::to_ubjson(j, true) == v);
            CHECK(json::from_ubjson(v) == j);
        }

        SECTION("size=true type=true")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', '$', '{', '#', 'i', 1, 'i', 4, 'p', 'o', 's', 't', '#', 'i', 4,
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x04, 0x71,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x00, 0x00, 0x01, 0x3D, 0xB1, 0x78, 0x66, 0x60
                                     };
            CHECK(json::to_ubjson(j, true, true) == v);
            CHECK(json::from_ubjson(v) == j);
        }
    }

    SECTION("Optimized Format")
    {
        SECTION("Array Example")
        {
            SECTION("No Optimization")
            {
                // note the floats have been replaced by doubles
                json j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[',
                                          'D', 0x40, 0x3d, 0xf8, 0x51, 0xeb, 0x85, 0x1e, 0xb8,
                                          'D', 0x40, 0x3f, 0x21, 0x47, 0xae, 0x14, 0x7a, 0xe1,
                                          'D', 0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          'D', 0x40, 0x00, 0xe7, 0x6c, 0x8b, 0x43, 0x95, 0x81,
                                          'D', 0x40, 0x37, 0xe3, 0x53, 0xf7, 0xce, 0xd9, 0x17,
                                          ']'
                                         };
                CHECK(json::to_ubjson(j) == v);
                CHECK(json::from_ubjson(v) == j);
            }

            SECTION("Optimized with count")
            {
                // note the floats have been replaced by doubles
                json j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[', '#', 'i', 5,
                                          'D', 0x40, 0x3d, 0xf8, 0x51, 0xeb, 0x85, 0x1e, 0xb8,
                                          'D', 0x40, 0x3f, 0x21, 0x47, 0xae, 0x14, 0x7a, 0xe1,
                                          'D', 0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          'D', 0x40, 0x00, 0xe7, 0x6c, 0x8b, 0x43, 0x95, 0x81,
                                          'D', 0x40, 0x37, 0xe3, 0x53, 0xf7, 0xce, 0xd9, 0x17
                                         };
                CHECK(json::to_ubjson(j, true) == v);
                CHECK(json::from_ubjson(v) == j);
            }

            SECTION("Optimized with type & count")
            {
                // note the floats have been replaced by doubles
                json j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[', '$', 'D', '#', 'i', 5,
                                          0x40, 0x3d, 0xf8, 0x51, 0xeb, 0x85, 0x1e, 0xb8,
                                          0x40, 0x3f, 0x21, 0x47, 0xae, 0x14, 0x7a, 0xe1,
                                          0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x40, 0x00, 0xe7, 0x6c, 0x8b, 0x43, 0x95, 0x81,
                                          0x40, 0x37, 0xe3, 0x53, 0xf7, 0xce, 0xd9, 0x17
                                         };
                CHECK(json::to_ubjson(j, true, true) == v);
                CHECK(json::from_ubjson(v) == j);
            }
        }

        SECTION("Object Example")
        {
            SECTION("No Optimization")
            {
                // note the floats have been replaced by doubles
                json j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{',
                                          'i', 3, 'a', 'l', 't', 'D', 0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          'i', 3, 'l', 'a', 't', 'D', 0x40, 0x3d, 0xf9, 0xdb, 0x22, 0xd0, 0xe5, 0x60,
                                          'i', 4, 'l', 'o', 'n', 'g', 'D', 0x40, 0x3f, 0x21, 0x89, 0x37, 0x4b, 0xc6, 0xa8,
                                          '}'
                                         };
                CHECK(json::to_ubjson(j) == v);
                CHECK(json::from_ubjson(v) == j);
            }

            SECTION("Optimized with count")
            {
                // note the floats have been replaced by doubles
                json j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{', '#', 'i', 3,
                                          'i', 3, 'a', 'l', 't', 'D', 0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          'i', 3, 'l', 'a', 't', 'D', 0x40, 0x3d, 0xf9, 0xdb, 0x22, 0xd0, 0xe5, 0x60,
                                          'i', 4, 'l', 'o', 'n', 'g', 'D', 0x40, 0x3f, 0x21, 0x89, 0x37, 0x4b, 0xc6, 0xa8
                                         };
                CHECK(json::to_ubjson(j, true) == v);
                CHECK(json::from_ubjson(v) == j);
            }

            SECTION("Optimized with type & count")
            {
                // note the floats have been replaced by doubles
                json j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{', '$', 'D', '#', 'i', 3,
                                          'i', 3, 'a', 'l', 't', 0x40, 0x50, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          'i', 3, 'l', 'a', 't', 0x40, 0x3d, 0xf9, 0xdb, 0x22, 0xd0, 0xe5, 0x60,
                                          'i', 4, 'l', 'o', 'n', 'g', 0x40, 0x3f, 0x21, 0x89, 0x37, 0x4b, 0xc6, 0xa8
                                         };
                CHECK(json::to_ubjson(j, true, true) == v);
                CHECK(json::from_ubjson(v) == j);
            }
        }

        SECTION("Special Cases (Null, No-Op and Boolean)")
        {
            SECTION("Array")
            {
                std::vector<uint8_t> v = {'[', '$', 'N', '#', 'I', 0x02, 0x00};
                CHECK(json::from_ubjson(v) == json::array());
            }

            SECTION("Object")
            {
                std::vector<uint8_t> v = {'{', '$', 'Z', '#', 'i', 3, 'i', 4, 'n', 'a', 'm', 'e', 'i', 8, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', 'i', 5, 'e', 'm', 'a', 'i', 'l'};
                CHECK(json::from_ubjson(v) == json({ {"name", nullptr}, {"password", nullptr}, {"email", nullptr} }));
            }
        }
    }
}

/*
TEST_CASE("Universal Binary JSON Specification Examples 2")
{
    for (std::string prefix :
            {
                "test/data/universal-binary-json-java/CouchDB4k",
                "test/data/universal-binary-json-java/MediaContent",
                "test/data/universal-binary-json-java/TwitterTimeline"
            })
    {
        CAPTURE(prefix);

        // parse JSON file
        std::ifstream f_json(prefix + ".formatted.json");
        json j1 = json::parse(f_json);

        SECTION("std::vector<uint8_t>")
        {
            // parse UBJSON file
            std::ifstream f_ubjson(prefix + ".ubj", std::ios::binary);
            std::vector<uint8_t> packed(
                (std::istreambuf_iterator<char>(f_ubjson)),
                std::istreambuf_iterator<char>());
            json j2;
            CHECK_NOTHROW(j2 = json::from_ubjson(packed));

            // compare parsed JSON values
            CHECK(j1 == j2);
        }
    }
}
*/


TEST_CASE("all UBJSON first bytes", "[!throws]")
{
    // these bytes will fail immediately with exception parse_error.112
    std::set<uint8_t> supported =
    {
        'T', 'F', 'Z', 'U', 'i', 'I', 'l', 'L', 'd', 'D', 'C', 'S', '[', '{', 'N'
    };

    for (auto i = 0; i < 256; ++i)
    {
        const auto byte = static_cast<uint8_t>(i);
        CAPTURE(byte);

        try
        {
            json::from_ubjson(std::vector<uint8_t>(1, byte));
        }
        catch (const json::parse_error& e)
        {
            // check that parse_error.112 is only thrown if the
            // first byte is not in the supported set
            CAPTURE(e.what());
            if (std::find(supported.begin(), supported.end(), byte) == supported.end())
            {
                CHECK(e.id == 112);
            }
            else
            {
                CHECK(e.id != 112);
            }
        }
    }
}

TEST_CASE("UBJSON roundtrips", "[hide]")
{
    SECTION("input from self-generated UBJSON files")
    {
        for (std::string filename :
                {
                    "test/data/json_nlohmann_tests/all_unicode.json",
                    "test/data/json.org/1.json",
                    "test/data/json.org/2.json",
                    "test/data/json.org/3.json",
                    "test/data/json.org/4.json",
                    "test/data/json.org/5.json",
                    "test/data/json_roundtrip/roundtrip01.json",
                    "test/data/json_roundtrip/roundtrip02.json",
                    "test/data/json_roundtrip/roundtrip03.json",
                    "test/data/json_roundtrip/roundtrip04.json",
                    "test/data/json_roundtrip/roundtrip05.json",
                    "test/data/json_roundtrip/roundtrip06.json",
                    "test/data/json_roundtrip/roundtrip07.json",
                    "test/data/json_roundtrip/roundtrip08.json",
                    "test/data/json_roundtrip/roundtrip09.json",
                    "test/data/json_roundtrip/roundtrip10.json",
                    "test/data/json_roundtrip/roundtrip11.json",
                    "test/data/json_roundtrip/roundtrip12.json",
                    "test/data/json_roundtrip/roundtrip13.json",
                    "test/data/json_roundtrip/roundtrip14.json",
                    "test/data/json_roundtrip/roundtrip15.json",
                    "test/data/json_roundtrip/roundtrip16.json",
                    "test/data/json_roundtrip/roundtrip17.json",
                    "test/data/json_roundtrip/roundtrip18.json",
                    "test/data/json_roundtrip/roundtrip19.json",
                    "test/data/json_roundtrip/roundtrip20.json",
                    "test/data/json_roundtrip/roundtrip21.json",
                    "test/data/json_roundtrip/roundtrip22.json",
                    "test/data/json_roundtrip/roundtrip23.json",
                    "test/data/json_roundtrip/roundtrip24.json",
                    "test/data/json_roundtrip/roundtrip25.json",
                    "test/data/json_roundtrip/roundtrip26.json",
                    "test/data/json_roundtrip/roundtrip27.json",
                    "test/data/json_roundtrip/roundtrip28.json",
                    "test/data/json_roundtrip/roundtrip29.json",
                    "test/data/json_roundtrip/roundtrip30.json",
                    "test/data/json_roundtrip/roundtrip31.json",
                    "test/data/json_roundtrip/roundtrip32.json",
                    "test/data/json_testsuite/sample.json",
                    "test/data/json_tests/pass1.json",
                    "test/data/json_tests/pass2.json",
                    "test/data/json_tests/pass3.json"
                })
        {
            CAPTURE(filename);

            // parse JSON file
            std::ifstream f_json(filename);
            json j1 = json::parse(f_json);

            SECTION("std::vector<uint8_t>")
            {
                // parse MessagePack file
                std::ifstream f_ubjson(filename + ".ubjson", std::ios::binary);
                std::vector<uint8_t> packed(
                    (std::istreambuf_iterator<char>(f_ubjson)),
                    std::istreambuf_iterator<char>());
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson(packed));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            SECTION("std::ifstream")
            {
                // parse MessagePack file
                std::ifstream f_ubjson(filename + ".ubjson", std::ios::binary);
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson(f_ubjson));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            SECTION("uint8_t* and size")
            {
                // parse MessagePack file
                std::ifstream f_ubjson(filename + ".ubjson", std::ios::binary);
                std::vector<uint8_t> packed(
                    (std::istreambuf_iterator<char>(f_ubjson)),
                    std::istreambuf_iterator<char>());
                json j2;
                CHECK_NOTHROW(j2 = json::from_ubjson({packed.data(), packed.size()}));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            SECTION("output to output adapters")
            {
                // parse MessagePack file
                std::ifstream f_ubjson(filename + ".ubjson", std::ios::binary);
                std::vector<uint8_t> packed(
                    (std::istreambuf_iterator<char>(f_ubjson)),
                    std::istreambuf_iterator<char>());

                SECTION("std::vector<uint8_t>")
                {
                    std::vector<uint8_t> vec;
                    json::to_ubjson(j1, vec);
                    CHECK(vec == packed);
                }
            }
        }
    }
}