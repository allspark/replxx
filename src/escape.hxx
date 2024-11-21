#ifndef REPLXX_ESCAPE_HXX_INCLUDED
#define REPLXX_ESCAPE_HXX_INCLUDED 1

namespace replxx::EscapeSequenceProcessing
{
class EscapeSequenceProcessingCallback
{
public:
  virtual char32_t read_unicode_character() = 0;
  virtual void beep() = 0;

protected:
  EscapeSequenceProcessingCallback() = default;
  ~EscapeSequenceProcessingCallback() = default;
};

char32_t doDispatch(EscapeSequenceProcessingCallback&, char32_t c);
}  // namespace replxx::EscapeSequenceProcessing

#endif
