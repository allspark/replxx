//
// Created by allspark on 11/21/24.
//

#include "escape.hxx"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

#include "replxx.hxx"

using namespace ::testing;

namespace
{
struct EscapeSequenceProcessingCallbackMock
  : replxx::EscapeSequenceProcessing::EscapeSequenceProcessingCallback
{
  MOCK_METHOD(char32_t, read_unicode_character, (), (override));
  MOCK_METHOD(void, beep, (), (override));
};

using TestValues = std::tuple<char32_t, std::string_view, char32_t>;

struct EscapeSequenceProcessingTestFixture : public ::testing::TestWithParam<TestValues>
{
  EscapeSequenceProcessingCallbackMock mock;
};
}  // namespace

TEST_P(EscapeSequenceProcessingTestFixture, EscapeDispatching)
{
  auto [init, input, expected] = GetParam();

  EXPECT_CALL(mock, read_unicode_character()).WillRepeatedly(Invoke([&input]() mutable -> char32_t
      {
        if (input.empty())
        {
          return 0;
        }
        auto const ret = input.front();
        input.remove_prefix(1);
        return ret;
      }));

  auto const ret = replxx::EscapeSequenceProcessing::doDispatch(mock, init);

  EXPECT_EQ(ret, expected);
}

INSTANTIATE_TEST_CASE_P(
    EscapeTests,
    EscapeSequenceProcessingTestFixture,
    ::testing::Values(
        TestValues{'\x1B', "[A", replxx::Replxx::KEY::UP},
        TestValues{'\x1B', "[B", replxx::Replxx::KEY::DOWN},
        TestValues{'\x1B', "[C", replxx::Replxx::KEY::RIGHT},
        TestValues{'\x1B', "[D", replxx::Replxx::KEY::LEFT},
        TestValues{'\x1B', "[H", replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[F", replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[Z", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::TAB},
        TestValues{'\x1B', "[0", -1},
        TestValues{'\x1B', "[1~", replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[1;2A", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::UP},
        TestValues{'\x1B', "[1;2B", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::DOWN},
        TestValues{'\x1B', "[1;2C", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::RIGHT},
        TestValues{'\x1B', "[1;2D", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::LEFT},
        TestValues{'\x1B', "[1;2H", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[1;2F", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[1;2P", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F1},
        TestValues{'\x1B', "[1;2Q", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F2},
        TestValues{'\x1B', "[1;2R", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F3},
        TestValues{'\x1B', "[1;2S", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F4},
        TestValues{'\x1B', "[1;3A", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::UP},
        TestValues{'\x1B', "[1;3B", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::DOWN},
        TestValues{'\x1B', "[1;3C", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::RIGHT},
        TestValues{'\x1B', "[1;3D", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::LEFT},
        TestValues{'\x1B', "[1;3H", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[1;3F", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[1;3P", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::F1},
        TestValues{'\x1B', "[1;3Q", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::F2},
        TestValues{'\x1B', "[1;3R", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::F3},
        TestValues{'\x1B', "[1;3S", replxx::Replxx::KEY::BASE_META | replxx::Replxx::KEY::F4},
        TestValues{'\x1B', "[1;5A", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::UP},
        TestValues{'\x1B', "[1;5B", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::DOWN},
        TestValues{'\x1B', "[1;5C", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::RIGHT},
        TestValues{'\x1B', "[1;5D", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::LEFT},
        TestValues{'\x1B', "[1;5H", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[1;5F", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[1;5P", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F1},
        TestValues{'\x1B', "[1;5Q", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F2},
        TestValues{'\x1B', "[1;5R", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F3},
        TestValues{'\x1B', "[1;5S", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F4},
        TestValues{'\x1B', "[15~", replxx::Replxx::KEY::F5},
        TestValues{'\x1B', "[15;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F5},
        TestValues{'\x1B', "[15;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F5},
        TestValues{'\x1B', "[17~", replxx::Replxx::KEY::F6},
        TestValues{'\x1B', "[17;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F6},
        TestValues{'\x1B', "[17;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F6},
        TestValues{'\x1B', "[18~", replxx::Replxx::KEY::F7},
        TestValues{'\x1B', "[18;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F7},
        TestValues{'\x1B', "[18;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F7},
        TestValues{'\x1B', "[19~", replxx::Replxx::KEY::F8},
        TestValues{'\x1B', "[19;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F8},
        TestValues{'\x1B', "[19;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F8},
        TestValues{'\x1B', "[2~", replxx::Replxx::KEY::INSERT},
        TestValues{'\x1B', "[20~", replxx::Replxx::KEY::F9},
        TestValues{'\x1B', "[20;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F9},
        TestValues{'\x1B', "[20;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F9},
        TestValues{'\x1B', "[200~", replxx::Replxx::KEY::PASTE_START},
        TestValues{'\x1B', "[201~", replxx::Replxx::KEY::PASTE_FINISH},
        TestValues{'\x1B', "[21~", replxx::Replxx::KEY::F10},
        TestValues{'\x1B', "[21;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F10},
        TestValues{'\x1B', "[21;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F10},
        TestValues{'\x1B', "[23~", replxx::Replxx::KEY::F11},
        TestValues{'\x1B', "[23;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F11},
        TestValues{'\x1B', "[23;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F11},
        TestValues{'\x1B', "[24~", replxx::Replxx::KEY::F12},
        TestValues{'\x1B', "[24;2~", replxx::Replxx::KEY::BASE_SHIFT | replxx::Replxx::KEY::F12},
        TestValues{'\x1B', "[24;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::F12},
        TestValues{'\x1B', "[3~", replxx::Replxx::KEY::DELETE},
        TestValues{'\x1B', "[4~", replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[5~", replxx::Replxx::KEY::PAGE_UP},
        TestValues{'\x1B', "[5;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::PAGE_UP},
        TestValues{'\x1B', "[6~", replxx::Replxx::KEY::PAGE_DOWN},
        TestValues{'\x1B', "[6;5~", replxx::Replxx::KEY::BASE_CONTROL | replxx::Replxx::KEY::PAGE_DOWN},
        TestValues{'\x1B', "[7~", replxx::Replxx::KEY::HOME},
        TestValues{'\x1B', "[8~", replxx::Replxx::KEY::END},
        TestValues{'\x1B', "[9", -1},
        TestValues{'\x7F', {}, replxx::Replxx::KEY::BACKSPACE}));
