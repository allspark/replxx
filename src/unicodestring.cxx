
#include "unicodestring.hxx"

#include <conversion.hxx>

#include "conversion.hxx"

namespace replxx
{
UnicodeString::UnicodeString(std::string_view src)
{
  assign(src);
}

UnicodeString::UnicodeString(UnicodeString const& other, int offset, int len)
{
  _data.insert(
      _data.end(),
      other._data.begin() + offset,
      len > 0 ? other._data.begin() + offset + len : other._data.end());
}

UnicodeString::UnicodeString(char const* src)
{
  assign(src);
}

UnicodeString::UnicodeString(char8_t const* src)
  : UnicodeString(reinterpret_cast<char const*>(src))
{
}

UnicodeString::UnicodeString(char32_t const* src)
{
  int len(0);
  while (src[len] != 0)
  {
    ++len;
  }
  _data.assign(src, src + len);
}

UnicodeString::UnicodeString(char32_t const* src, int len)
{
  _data.assign(src, src + len);
}

UnicodeString::UnicodeString(int len)
{
  _data.resize(len);
}

UnicodeString& UnicodeString::assign(std::string_view str_)
{
  _data.resize(static_cast<int>(str_.length()));
  int len(0);
  copyString8to32(_data.data(), static_cast<int>(str_.length()), len, str_.data());
  _data.resize(len);
  return *this;
}

bool UnicodeString::operator==(UnicodeString const& other_) const
{
  return (_data == other_._data);
}

bool UnicodeString::operator!=(UnicodeString const& other_) const
{
  return (_data != other_._data);
}

bool UnicodeString::operator<(UnicodeString const& other_) const
{
  return std::lexicographical_compare(begin(), end(), other_.begin(), other_.end());
}

UnicodeString& UnicodeString::append(UnicodeString const& other)
{
  _data.insert(_data.end(), other._data.begin(), other._data.end());
  return *this;
}

void UnicodeString::push_back(char32_t c_)
{
  _data.push_back(c_);
}

UnicodeString& UnicodeString::append(char32_t const* src, int len)
{
  _data.insert(_data.end(), src, src + len);
  return *this;
}

UnicodeString& UnicodeString::insert(std::size_t pos_, UnicodeString const& str_, std::size_t offset_, std::size_t len_)
{
  _data.insert(_data.begin() + pos_, str_._data.begin() + offset_, str_._data.begin() + offset_ + len_);
  return *this;
}

UnicodeString& UnicodeString::insert(std::size_t pos_, char32_t c_)
{
  _data.insert(_data.begin() + pos_, c_);
  return *this;
}

UnicodeString& UnicodeString::erase(std::size_t pos_)
{
  _data.erase(_data.begin() + pos_);
  return *this;
}

UnicodeString& UnicodeString::erase(std::size_t pos_, std::size_t len_)
{
  _data.erase(_data.begin() + pos_, _data.begin() + pos_ + len_);
  return *this;
}

char32_t const* UnicodeString::get() const
{
  return _data.data();
}

char32_t* UnicodeString::get()
{
  return _data.data();
}

std::size_t UnicodeString::length() const
{
  return _data.size();
}

void UnicodeString::clear()
{
  _data.clear();
}

char32_t const& UnicodeString::operator[](std::size_t pos) const
{
  assert(pos < static_cast<int>(_data.size()));
  return _data[pos];
}

char32_t& UnicodeString::operator[](std::size_t pos)
{
  assert(pos < static_cast<int>(_data.size()));
  return _data[pos];
}

bool UnicodeString::starts_with(data_buffer_t::const_iterator first_, data_buffer_t::const_iterator last_) const
{
  return std::distance(first_, last_) <= length() && std::equal(first_, last_, _data.begin());
}

bool UnicodeString::ends_with(data_buffer_t::const_iterator first_, data_buffer_t::const_iterator last_) const
{
  auto len = static_cast<std::size_t>(std::distance(first_, last_));
  return len <= length() && std::equal(first_, last_, _data.end() - len);
}

bool UnicodeString::is_empty() const
{
  return _data.empty();
}

void UnicodeString::swap(UnicodeString& other_)
{
  _data.swap(other_._data);
}

UnicodeString::const_iterator UnicodeString::begin() const
{
  return _data.begin();
}

UnicodeString::const_iterator UnicodeString::end() const
{
  return _data.end();
}

UnicodeString::iterator UnicodeString::begin()
{
  return _data.begin();
}

UnicodeString::iterator UnicodeString::end()
{
  return _data.end();
}

char32_t UnicodeString::back() const
{
  return _data.back();
}
}  // namespace replxx