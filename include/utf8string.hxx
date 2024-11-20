#ifndef REPLXX_UTF8STRING_HXX_INCLUDED
#define REPLXX_UTF8STRING_HXX_INCLUDED

#include <cstring>
#include <memory>

#include "unicodestring.hxx"

namespace replxx
{

class Utf8String
{
private:
  std::vector<char> _data;

public:
  Utf8String() = default;
  Utf8String(Utf8String const&) = default;
  Utf8String(Utf8String&&) = default;
  Utf8String& operator=(Utf8String const&) = default;
  Utf8String& operator=(Utf8String&&) = default;

  explicit Utf8String(UnicodeString const& src);
  Utf8String(UnicodeString const& src_, int len_);

  void assign(UnicodeString const& str_, int len_);
  void assign(char32_t const* str_, int len_);
  void assign(std::string_view str_);

  operator std::string_view() const noexcept;

  // char const* get() const;
  std::size_t size() const;

  bool operator!=(Utf8String const&) const = default;
  bool operator==(Utf8String const&) const = default;

private:
  void realloc(int reqLen);
};

}  // namespace replxx

#endif
