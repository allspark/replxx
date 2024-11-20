
#include "utf8string.hxx"

#include "conversion.hxx"

namespace replxx
{

Utf8String::Utf8String(UnicodeString const& src)
{
  assign(src, src.length());
}

Utf8String::Utf8String(UnicodeString const& src_, int len_)
{
  assign(src_, len_);
}

void Utf8String::assign(UnicodeString const& str_, int len_)
{
  assign(str_.get(), len_);
}

void Utf8String::assign(char32_t const* str_, int len_)
{
  int len = len_ * 4;
  realloc(len);
  len = copyString32to8(_data.data(), len, str_, len_);
  realloc(len);
}

void Utf8String::assign(std::string_view str_)
{
  realloc(static_cast<int>(str_.length()));
  strncpy(_data.data(), str_.data(), str_.length());
}

Utf8String::operator std::string_view() const noexcept
{
  if (_data.empty())
  {
    return {};
  }
  return {_data.data(), _data.size()};
}

// char const* Utf8String::get() const
// {
//   return _data.data();
// }

std::size_t Utf8String::size() const
{
  return _data.size();
}

void Utf8String::realloc(int reqLen)
{
  _data.resize(reqLen);
}
};  // namespace replxx
