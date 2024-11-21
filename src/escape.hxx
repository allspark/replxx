#ifndef REPLXX_ESCAPE_HXX_INCLUDED
#define REPLXX_ESCAPE_HXX_INCLUDED 1

namespace replxx
{

namespace EscapeSequenceProcessing
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
// This is a typedef for the routine called by doDispatch().	It takes the
// current character
// as input, does any required processing including reading more characters and
// calling other
// dispatch routines, then eventually returns the final (possibly extended or
// special) character.
//
typedef char32_t (*CharacterDispatchRoutine)(EscapeSequenceProcessingCallback&, char32_t);

// This structure is used by doDispatch() to hold a list of characters to test
// for and
// a list of routines to call if the character matches.	The dispatch routine
// list is one
// longer than the character list; the final entry is used if no character
// matches.
//
struct CharacterDispatch
{
  unsigned int len;                    // length of the chars list
  char const* chars;                   // chars to test
  CharacterDispatchRoutine* dispatch;  // array of routines to call
};

char32_t doDispatch(EscapeSequenceProcessingCallback&, char32_t c);

}  // namespace EscapeSequenceProcessing

}  // namespace replxx

#endif
