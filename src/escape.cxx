#include "escape.hxx"
#include "replxx.hxx"

#include <span>

#ifndef _WIN32

namespace replxx::EscapeSequenceProcessing
{
namespace
{
struct Impl;
using CharacterDispatchRoutine = char32_t (Impl::*)(char32_t);
template <CharacterDispatchRoutine... funcs>
constexpr std::span<CharacterDispatchRoutine const> Routines()
{
  constexpr static std::array ret{funcs...};
  return ret;
}
struct Impl
{
  struct CharacterDispatch
  {
    std::string_view chars;                              // chars to test
    std::span<CharacterDispatchRoutine const> dispatch;  // array of routines to call
  };

  Impl(EscapeSequenceProcessingCallback& t_callback)
    : callback(t_callback)
  {
  }

  EscapeSequenceProcessingCallback& callback;

  // This chunk of code does parsing of the escape sequences sent by various Linux
  // terminals.
  //
  // It handles arrow keys, Home, End and Delete keys by interpreting the
  // sequences sent by
  // gnome terminal, xterm, rxvt, konsole, aterm and yakuake including the Alt and
  // Ctrl key
  // combinations that are understood by replxx.
  //
  // The parsing uses tables, a bunch of intermediate dispatch routines and a
  // doDispatch
  // loop that reads the tables and sends control to "deeper" routines to continue
  // the
  // parsing.	The starting call to doDispatch( c, initialDispatch ) will
  // eventually return
  // either a character (with optional CTRL and META bits set), or -1 if parsing
  // fails, or
  // zero if an attempt to read from the keyboard fails.
  //
  // This is rather sloppy escape sequence processing, since we're not paying
  // attention to what the
  // actual TERM is set to and are processing all key sequences for all terminals,
  // but it works with
  // the most common keystrokes on the most common terminals.	It's intricate, but
  // the nested 'if'
  // statements required to do it directly would be worse.	This way has the
  // advantage of allowing
  // changes and extensions without having to touch a lot of code.

  char32_t thisKeyMetaCtrl = 0;  // holds pre-set Meta and/or Ctrl modifiers // FIXME move this

  // This dispatch routine is given a dispatch table and then farms work out to
  // routines
  // listed in the table based on the character it is called with.	The dispatch
  // routines can
  // read more input characters to decide what should eventually be returned.
  // Eventually,
  // a called routine returns either a character or -1 to indicate parsing
  // failure.
  //
  char32_t doDispatch(char32_t c, CharacterDispatch const& dispatchTable)
  {
    for (unsigned int i = 0; i < dispatchTable.chars.size(); ++i)
    {
      if (static_cast<unsigned char>(dispatchTable.chars[i]) == c)
      {
        return std::invoke(dispatchTable.dispatch[i], this, c);
      }
    }
    return std::invoke(dispatchTable.dispatch.back(), this, c);
  }

  // Final dispatch routines -- return something
  //
  char32_t normalKeyRoutine(char32_t c)
  {
    return thisKeyMetaCtrl | c;
  }
  char32_t upArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::UP;
    ;
  }
  char32_t downArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::DOWN;
  }
  char32_t rightArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::RIGHT;
  }
  char32_t leftArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::LEFT;
  }
  char32_t homeKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::HOME;
  }
  char32_t endKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::END;
  }
  char32_t shiftTabRoutine(char32_t)
  {
    return Replxx::KEY::BASE_SHIFT | Replxx::KEY::TAB;
  }
  char32_t f1KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F1;
  }
  char32_t f2KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F2;
  }
  char32_t f3KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F3;
  }
  char32_t f4KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F4;
  }
  char32_t f5KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F5;
  }
  char32_t f6KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F6;
  }
  char32_t f7KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F7;
  }
  char32_t f8KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F8;
  }
  char32_t f9KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F9;
  }
  char32_t f10KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F10;
  }
  char32_t f11KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F11;
  }
  char32_t f12KeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::F12;
  }
  char32_t pageUpKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::PAGE_UP;
  }
  char32_t pageDownKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::PAGE_DOWN;
  }
  char32_t deleteCharRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::BACKSPACE;
  }  // key labeled Backspace
  char32_t insertKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::INSERT;
  }  // key labeled Delete
  char32_t deleteKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::DELETE;
  }  // key labeled Delete
  char32_t ctrlUpArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::BASE_CONTROL | Replxx::KEY::UP;
  }
  char32_t ctrlDownArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::BASE_CONTROL | Replxx::KEY::DOWN;
  }
  char32_t ctrlRightArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::BASE_CONTROL | Replxx::KEY::RIGHT;
  }
  char32_t ctrlLeftArrowKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::BASE_CONTROL | Replxx::KEY::LEFT;
  }
  char32_t bracketPasteStartKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::PASTE_START;
  }
  char32_t bracketPasteFinishKeyRoutine(char32_t)
  {
    return thisKeyMetaCtrl | Replxx::KEY::PASTE_FINISH;
  }
  char32_t escFailureRoutine(char32_t)
  {
    callback.beep();
    return -1;
  }

  // Handle ESC [ 1 ; 2 or 3 (or 5) <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket1Semicolon2or3or5Dispatch = {
      "ABCDHFPQRS", Routines<                         //
                        &Impl::upArrowKeyRoutine,     //
                        &Impl::downArrowKeyRoutine,   //
                        &Impl::rightArrowKeyRoutine,  //
                        &Impl::leftArrowKeyRoutine,   //
                        &Impl::homeKeyRoutine,        //
                        &Impl::endKeyRoutine,         //
                        &Impl::f1KeyRoutine,          //
                        &Impl::f2KeyRoutine,          //
                        &Impl::f3KeyRoutine,          //
                        &Impl::f4KeyRoutine,          //
                        &Impl::escFailureRoutine      //
                        >()};

  // Handle ESC [ 1 ; <more stuff> escape sequences
  //
  char32_t escLeftBracket1Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket1Semicolon2or3or5Dispatch);
  }
  char32_t escLeftBracket1Semicolon3Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_META;
    return doDispatch(c, escLeftBracket1Semicolon2or3or5Dispatch);
  }
  char32_t escLeftBracket1Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket1Semicolon2or3or5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket1SemicolonDispatch = {
      "235", Routines<                                     //
                 &Impl::escLeftBracket1Semicolon2Routine,  //
                 &Impl::escLeftBracket1Semicolon3Routine,  //
                 &Impl::escLeftBracket1Semicolon5Routine,  //
                 &Impl::escFailureRoutine                  //
                 >()};

  // Handle ESC [ 1 ; <more stuff> escape sequences
  //
  char32_t escLeftBracket1SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket1SemicolonDispatch);
  }

  // (S)-F5
  static constexpr CharacterDispatch escLeftBracket15Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f5KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket15Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket15Semicolon2Dispatch);
  }

  // (C)-F5
  static constexpr CharacterDispatch escLeftBracket15Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f5KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket15Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket15Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket15SemicolonDispatch = {
      "25",
      Routines<                                      //
          &Impl::escLeftBracket15Semicolon2Routine,  //
          &Impl::escLeftBracket15Semicolon5Routine,  //
          &Impl::escFailureRoutine                   //
          >()};
  char32_t escLeftBracket15SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket15SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket15Dispatch = {
      "~;", Routines<                                     //
                &Impl::f5KeyRoutine,                      //
                &Impl::escLeftBracket15SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket15Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket15Dispatch);
  }

  // (S)-F6
  static constexpr CharacterDispatch escLeftBracket17Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f6KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket17Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket17Semicolon2Dispatch);
  }

  // (C)-F6
  static constexpr CharacterDispatch escLeftBracket17Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f6KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket17Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket17Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket17SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket17Semicolon2Routine,  //
                &Impl::escLeftBracket17Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket17SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket17SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket17Dispatch = {
      "~;", Routines<                                     //
                &Impl::f6KeyRoutine,                      //
                &Impl::escLeftBracket17SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket17Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket17Dispatch);
  }

  // (S)-F7
  static constexpr CharacterDispatch escLeftBracket18Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f7KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket18Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket18Semicolon2Dispatch);
  }

  // (C)-F7
  static constexpr CharacterDispatch escLeftBracket18Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f7KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket18Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket18Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket18SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket18Semicolon2Routine,  //
                &Impl::escLeftBracket18Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket18SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket18SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket18Dispatch = {
      "~;", Routines<                                     //
                &Impl::f7KeyRoutine,                      //
                &Impl::escLeftBracket18SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket18Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket18Dispatch);
  }

  // (S)-F8
  static constexpr CharacterDispatch escLeftBracket19Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f8KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket19Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket19Semicolon2Dispatch);
  }

  // (C)-F8
  static constexpr CharacterDispatch escLeftBracket19Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f8KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket19Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket19Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket19SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket19Semicolon2Routine,  //
                &Impl::escLeftBracket19Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket19SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket19SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket19Dispatch = {
      "~;", Routines<                                     //
                &Impl::f8KeyRoutine,                      //
                &Impl::escLeftBracket19SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket19Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket19Dispatch);
  }

  // Handle ESC [ 1 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket1Dispatch = {
      "~;5789", Routines<                                    //
                    &Impl::homeKeyRoutine,                   //
                    &Impl::escLeftBracket1SemicolonRoutine,  //
                    &Impl::escLeftBracket15Routine,          //
                    &Impl::escLeftBracket17Routine,          //
                    &Impl::escLeftBracket18Routine,          //
                    &Impl::escLeftBracket19Routine,          //
                    &Impl::escFailureRoutine                 //
                    >()};

  // Handle ESC [ 2 <more stuff> escape sequences
  //

  // (S)-F9
  static constexpr CharacterDispatch escLeftBracket20Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f9KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket20Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket20Semicolon2Dispatch);
  }

  // (C)-F9
  static constexpr CharacterDispatch escLeftBracket20Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f9KeyRoutine,      //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket20Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket20Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket20SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket20Semicolon2Routine,  //
                &Impl::escLeftBracket20Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket20SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket20SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket200Dispatch = {
      "~", Routines<                                //
               &Impl::bracketPasteStartKeyRoutine,  //
               &Impl::escFailureRoutine             //
               >()};
  char32_t escLeftBracket200Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket200Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket201Dispatch = {
      "~", Routines<                                 //
               &Impl::bracketPasteFinishKeyRoutine,  //
               &Impl::escFailureRoutine              //
               >()};
  char32_t escLeftBracket201Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket201Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket20Dispatch = {
      "~;01", Routines<                                     //
                  &Impl::f9KeyRoutine,                      //
                  &Impl::escLeftBracket20SemicolonRoutine,  //
                  &Impl::escLeftBracket200Routine,          //
                  &Impl::escLeftBracket201Routine,          //
                  &Impl::escFailureRoutine                  //
                  >()};
  char32_t escLeftBracket20Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket20Dispatch);
  }

  // (S)-F10
  static constexpr CharacterDispatch escLeftBracket21Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f10KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket21Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket21Semicolon2Dispatch);
  }

  // (C)-F10
  static constexpr CharacterDispatch escLeftBracket21Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f10KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket21Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket21Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket21SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket21Semicolon2Routine,  //
                &Impl::escLeftBracket21Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket21SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket21SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket21Dispatch = {
      "~;", Routines<                                     //
                &Impl::f10KeyRoutine,                     //
                &Impl::escLeftBracket21SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket21Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket21Dispatch);
  }

  // (S)-F11
  static constexpr CharacterDispatch escLeftBracket23Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f11KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket23Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket23Semicolon2Dispatch);
  }

  // (C)-F11
  static constexpr CharacterDispatch escLeftBracket23Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f11KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket23Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket23Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket23SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket23Semicolon2Routine,  //
                &Impl::escLeftBracket23Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket23SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket23SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket23Dispatch = {
      "~;", Routines<                                     //
                &Impl::f11KeyRoutine,                     //
                &Impl::escLeftBracket23SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket23Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket23Dispatch);
  }

  // (S)-F12
  static constexpr CharacterDispatch escLeftBracket24Semicolon2Dispatch = {
      "~", Routines<                     //
               &Impl::f12KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket24Semicolon2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_SHIFT;
    return doDispatch(c, escLeftBracket24Semicolon2Dispatch);
  }

  // (C)-F12
  static constexpr CharacterDispatch escLeftBracket24Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::f12KeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket24Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket24Semicolon5Dispatch);
  }

  static constexpr CharacterDispatch escLeftBracket24SemicolonDispatch = {
      "25", Routines<                                      //
                &Impl::escLeftBracket24Semicolon2Routine,  //
                &Impl::escLeftBracket24Semicolon5Routine,  //
                &Impl::escFailureRoutine                   //
                >()};
  char32_t escLeftBracket24SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket24SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket24Dispatch = {
      "~;", Routines<                                     //
                &Impl::f12KeyRoutine,                     //
                &Impl::escLeftBracket24SemicolonRoutine,  //
                &Impl::escFailureRoutine                  //
                >()};
  char32_t escLeftBracket24Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket24Dispatch);
  }

  // Handle ESC [ 2 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket2Dispatch = {
      "~0134", Routines<                            //
                   &Impl::insertKeyRoutine,         //
                   &Impl::escLeftBracket20Routine,  //
                   &Impl::escLeftBracket21Routine,  //
                   &Impl::escLeftBracket23Routine,  //
                   &Impl::escLeftBracket24Routine,  //
                   &Impl::escFailureRoutine         //
                   >()};

  // Handle ESC [ 3 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket3Dispatch = {
      "~", Routines<                     //
               &Impl::deleteKeyRoutine,  //
               &Impl::escFailureRoutine  //
               >()};

  // Handle ESC [ 4 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket4Dispatch = {
      "~", Routines<                     //
               &Impl::endKeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};

  // Handle ESC [ 5 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket5Semicolon5Dispatch = {
      "~", Routines<                     //
               &Impl::pageUpKeyRoutine,  //
               &Impl::escFailureRoutine  //
               >()};
  char32_t escLeftBracket5Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket5Semicolon5Dispatch);
  }
  static constexpr CharacterDispatch escLeftBracket5SemicolonDispatch = {
      "5", Routines<                                     //
               &Impl::escLeftBracket5Semicolon5Routine,  //
               &Impl::escFailureRoutine                  //
               >()};
  char32_t escLeftBracket5SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket5SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket5Dispatch = {
      "~;", Routines<                                    //
                &Impl::pageUpKeyRoutine,                 //
                &Impl::escLeftBracket5SemicolonRoutine,  //
                &Impl::escFailureRoutine                 //
                >()};

  // Handle ESC [ 6 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket6Semicolon5Dispatch = {
      "~", Routines<                       //
               &Impl::pageDownKeyRoutine,  //
               &Impl::escFailureRoutine    //
               >()};
  char32_t escLeftBracket6Semicolon5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    thisKeyMetaCtrl |= Replxx::KEY::BASE_CONTROL;
    return doDispatch(c, escLeftBracket6Semicolon5Dispatch);
  }
  static constexpr CharacterDispatch escLeftBracket6SemicolonDispatch = {
      "5", Routines<                                     //
               &Impl::escLeftBracket6Semicolon5Routine,  //
               &Impl::escFailureRoutine                  //
               >()};
  char32_t escLeftBracket6SemicolonRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket6SemicolonDispatch);
  }

  static constexpr CharacterDispatch escLeftBracket6Dispatch = {
      "~;", Routines<                                    //
                &Impl::pageDownKeyRoutine,               //
                &Impl::escLeftBracket6SemicolonRoutine,  //
                &Impl::escFailureRoutine                 //
                >()};

  // Handle ESC [ 7 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket7Dispatch = {
      "~", Routines<                     //
               &Impl::homeKeyRoutine,    //
               &Impl::escFailureRoutine  //
               >()};

  // Handle ESC [ 8 <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracket8Dispatch = {
      "~", Routines<                     //
               &Impl::endKeyRoutine,     //
               &Impl::escFailureRoutine  //
               >()};

  // Handle ESC [ <digit> escape sequences
  //
  char32_t escLeftBracket0Routine(char32_t c)
  {
    return escFailureRoutine(c);
  }
  char32_t escLeftBracket1Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket1Dispatch);
  }
  char32_t escLeftBracket2Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket2Dispatch);
  }
  char32_t escLeftBracket3Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket3Dispatch);
  }
  char32_t escLeftBracket4Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket4Dispatch);
  }
  char32_t escLeftBracket5Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket5Dispatch);
  }
  char32_t escLeftBracket6Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket6Dispatch);
  }
  char32_t escLeftBracket7Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket7Dispatch);
  }
  char32_t escLeftBracket8Routine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracket8Dispatch);
  }
  char32_t escLeftBracket9Routine(char32_t c)
  {
    return escFailureRoutine(c);
  }

  // Handle ESC [ <more stuff> escape sequences
  //
  static constexpr CharacterDispatch escLeftBracketDispatch = {
      "ABCDHFZ0123456789", Routines<                           //
                               &Impl::upArrowKeyRoutine,       //
                               &Impl::downArrowKeyRoutine,     //
                               &Impl::rightArrowKeyRoutine,    //
                               &Impl::leftArrowKeyRoutine,     //
                               &Impl::homeKeyRoutine,          //
                               &Impl::endKeyRoutine,           //
                               &Impl::shiftTabRoutine,         //
                               &Impl::escLeftBracket0Routine,  //
                               &Impl::escLeftBracket1Routine,  //
                               &Impl::escLeftBracket2Routine,  //
                               &Impl::escLeftBracket3Routine,  //
                               &Impl::escLeftBracket4Routine,  //
                               &Impl::escLeftBracket5Routine,  //
                               &Impl::escLeftBracket6Routine,  //
                               &Impl::escLeftBracket7Routine,  //
                               &Impl::escLeftBracket8Routine,  //
                               &Impl::escLeftBracket9Routine,  //
                               &Impl::escFailureRoutine        //
                               >()};

  // Handle ESC O <char> escape sequences
  //
  static constexpr CharacterDispatch escODispatch = {
      "ABCDHFPQRSabcd", Routines<                             //
                            &Impl::upArrowKeyRoutine,         //
                            &Impl::downArrowKeyRoutine,       //
                            &Impl::rightArrowKeyRoutine,      //
                            &Impl::leftArrowKeyRoutine,       //
                            &Impl::homeKeyRoutine,            //
                            &Impl::endKeyRoutine,             //
                            &Impl::f1KeyRoutine,              //
                            &Impl::f2KeyRoutine,              //
                            &Impl::f3KeyRoutine,              //
                            &Impl::f4KeyRoutine,              //
                            &Impl::ctrlUpArrowKeyRoutine,     //
                            &Impl::ctrlDownArrowKeyRoutine,   //
                            &Impl::ctrlRightArrowKeyRoutine,  //
                            &Impl::ctrlLeftArrowKeyRoutine,   //
                            &Impl::escFailureRoutine          //
                            >()};

  // Initial ESC dispatch -- could be a Meta prefix or the start of an escape
  // sequence
  //
  char32_t escLeftBracketRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escLeftBracketDispatch);
  }

  char32_t escORoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escODispatch);
  }
  // Special handling for the ESC key because it does double duty
  //
  char32_t setMetaRoutine(char32_t c)
  {
    thisKeyMetaCtrl = Replxx::KEY::BASE_META;
    if (c == 0x1B)
    {  // another ESC, stay in ESC processing mode
      c = callback.read_unicode_character();
      if (c == 0)
        return 0;
      return doDispatch(c, escDispatch);
    }
    return doDispatch(c, initialDispatch);
  }
  constexpr static CharacterDispatch escDispatch = {
      "[O", Routines<                          //
                &Impl::escLeftBracketRoutine,  //
                &Impl::escORoutine,            //
                &Impl::setMetaRoutine          //
                >()};

  // Initial dispatch -- we are not in the middle of anything yet
  //
  char32_t escRoutine(char32_t c)
  {
    c = callback.read_unicode_character();
    if (c == 0)
      return 0;
    return doDispatch(c, escDispatch);
  }
  constexpr static CharacterDispatch initialDispatch = {
      "\x1B\x7F", Routines<                      //
                      &Impl::escRoutine,         //
                      &Impl::deleteCharRoutine,  //
                      &Impl::normalKeyRoutine    //
                      >()};
};
}  // namespace

char32_t doDispatch(EscapeSequenceProcessingCallback& callback, char32_t c)
{
  return Impl{callback}.doDispatch(c, Impl::initialDispatch);
}

}  // namespace replxx::EscapeSequenceProcessing

#endif /* #ifndef _WIN32 */
