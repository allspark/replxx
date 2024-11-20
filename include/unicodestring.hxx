#ifndef REPLXX_UNICODESTRING_HXX_INCLUDED
#define REPLXX_UNICODESTRING_HXX_INCLUDED

#include <cassert>
#include <cwctype>
#include <string_view>
#include <vector>

namespace replxx
{

inline bool case_sensitive_equal(char32_t l, char32_t r)
{
  return l == r;
}

inline bool case_insensitive_equal(char32_t l, char32_t r)
{
  return towlower(static_cast<wint_t>(l)) == towlower(static_cast<wint_t>(r));
}

class UnicodeString
{
public:
  using data_buffer_t = std::vector<char32_t>;
  using const_iterator = data_buffer_t::const_iterator;
  using iterator = data_buffer_t::iterator;

private:
  data_buffer_t _data;

public:
  UnicodeString() = default;

  explicit UnicodeString(std::string_view src);
  explicit UnicodeString(UnicodeString const& other, int offset, int len = -1);
  explicit UnicodeString(char const* src);
  explicit UnicodeString(char8_t const* src);
  explicit UnicodeString(char32_t const* src);
  explicit UnicodeString(char32_t const* src, int len);
  explicit UnicodeString(int len);

  UnicodeString& assign(std::string_view str_);

  UnicodeString(UnicodeString const&) = default;
  UnicodeString& operator=(UnicodeString const&) = default;
  UnicodeString(UnicodeString&&) noexcept = default;
  UnicodeString& operator=(UnicodeString&&) noexcept = default;

  bool operator==(UnicodeString const& other_) const;
  bool operator!=(UnicodeString const& other_) const;
  bool operator<(UnicodeString const& other_) const;

  UnicodeString& append(UnicodeString const& other);

  void push_back(char32_t c_);

  UnicodeString& append(char32_t const* src, int len);

  UnicodeString& insert(std::size_t pos_, UnicodeString const& str_, std::size_t offset_, std::size_t len_);

  UnicodeString& insert(std::size_t pos_, char32_t c_);

  UnicodeString& erase(std::size_t pos_);

  UnicodeString& erase(std::size_t pos_, std::size_t len_);

  char32_t const* get() const;

  char32_t* get();

  std::size_t length() const;

  void clear();

  char32_t const& operator[](std::size_t pos) const;

  char32_t& operator[](std::size_t pos);

  bool starts_with(data_buffer_t::const_iterator first_, data_buffer_t::const_iterator last_) const;

  template <class BinaryPredicate>
  bool starts_with(data_buffer_t::const_iterator first_, data_buffer_t::const_iterator last_, BinaryPredicate&& pred) const
  {
    return std::distance(first_, last_) <= length() && std::equal(first_, last_, _data.begin(), std::forward<BinaryPredicate>(pred));
  }

  bool ends_with(data_buffer_t::const_iterator first_, data_buffer_t::const_iterator last_) const;

  bool is_empty() const;

  void swap(UnicodeString& other_);

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  char32_t back() const;
};

}  // namespace replxx

#endif
