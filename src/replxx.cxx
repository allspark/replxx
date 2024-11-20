/*
 * Copyright (c) 2017-2018, Marcin Konarski (amok at codestation.org)
 * Copyright (c) 2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *	 * Redistributions of source code must retain the above copyright notice,
 *		 this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *		 notice, this list of conditions and the following disclaimer in the
 *		 documentation and/or other materials provided with the distribution.
 *	 * Neither the name of Redis nor the names of its contributors may be used
 *		 to endorse or promote products derived from this software without
 *		 specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * line editing lib needs to be 20,000 lines of C code.
 *
 * You can find the latest source code at:
 *
 *	 http://github.com/antirez/linenoise
 *
 * Does a number of crazy assumptions that happen to be true in 99.9999% of
 * the 2010 UNIX computers around.
 *
 * References:
 * - http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
 * - http://www.3waylabs.com/nw/WWW/products/wizcon/vt220.html
 *
 * Todo list:
 * - Switch to gets() if $TERM is something we can't support.
 * - Filter bogus Ctrl+<char> combinations.
 * - Win32 support
 *
 * Bloat:
 * - Completion?
 * - History search like Ctrl+r in readline?
 *
 * List of escape sequences used by this program, we do everything just
 * with three sequences. In order to be so cheap we may have some
 * flickering effect with some slow terminal, but the lesser sequences
 * the more compatible.
 *
 * CHA (Cursor Horizontal Absolute)
 *		Sequence: ESC [ n G
 *		Effect: moves cursor to column n (1 based)
 *
 * EL (Erase Line)
 *		Sequence: ESC [ n K
 *		Effect: if n is 0 or missing, clear from cursor to end of line
 *		Effect: if n is 1, clear from beginning of line to cursor
 *		Effect: if n is 2, clear entire line
 *
 * CUF (Cursor Forward)
 *		Sequence: ESC [ n C
 *		Effect: moves cursor forward of n chars
 *
 * The following are used to clear the screen: ESC [ H ESC [ 2 J
 * This is actually composed of two sequences:
 *
 * cursorhome
 *		Sequence: ESC [ H
 *		Effect: moves the cursor to upper left corner
 *
 * ED2 (Clear entire screen)
 *		Sequence: ESC [ 2 J
 *		Effect: clear the whole screen
 *
 */

#include <algorithm>
#include <cstdarg>

#ifdef _WIN32

#  include <io.h>
#  define STDIN_FILENO 0

#else /* _WIN32 */

#  include <signal.h>
#  include <sys/stat.h>
#  include <unistd.h>

#endif /* _WIN32 */

#include "history.hxx"
#include "replxx.hxx"
#include "replxx_impl.hxx"

using namespace std;
using namespace std::placeholders;
using namespace replxx;

namespace replxx
{

Replxx::Replxx(void)
  : _impl(new Replxx::ReplxxImpl(nullptr, nullptr, nullptr))
{
}

Replxx::~Replxx() = default;

void Replxx::set_completion_callback(completion_callback_t fn)
{
  _impl->set_completion_callback(fn);
}

void Replxx::set_modify_callback(modify_callback_t fn)
{
  _impl->set_modify_callback(fn);
}

void Replxx::set_highlighter_callback(highlighter_callback_t fn)
{
  _impl->set_highlighter_callback(fn);
}

void Replxx::set_hint_callback(hint_callback_t fn)
{
  _impl->set_hint_callback(fn);
}

Utf8String const* Replxx::input(std::string_view prompt)
{
  return (_impl->input(std::string{prompt}));
}

void Replxx::history_add(std::string_view line)
{
  _impl->history_add(std::string{line});
}

bool Replxx::history_sync(std::filesystem::path const& filename)
{
  return (_impl->history_sync(filename));
}

bool Replxx::history_save(std::filesystem::path const& filename)
{
  return (_impl->history_save(filename));
}

void Replxx::history_save(std::ostream& out)
{
  _impl->history_save(out);
}

bool Replxx::history_load(std::filesystem::path const& filename)
{
  return (_impl->history_load(filename));
}

void Replxx::history_load(std::istream& in)
{
  _impl->history_load(in);
}

void Replxx::history_clear(void)
{
  _impl->history_clear();
}

std::size_t Replxx::history_size(void) const
{
  return (_impl->history_size());
}

Replxx::HistoryScan Replxx::history_scan(void) const
{
  return (_impl->history_scan());
}

void Replxx::set_preload_buffer(std::string preloadText)
{
  _impl->set_preload_buffer(preloadText);
}

void Replxx::set_word_break_characters(std::string_view wordBreakers)
{
  _impl->set_word_break_characters(wordBreakers.data());
}

void Replxx::set_max_hint_rows(int count)
{
  _impl->set_max_hint_rows(count);
}

void Replxx::set_hint_delay(int milliseconds)
{
  _impl->set_hint_delay(milliseconds);
}

void Replxx::set_completion_count_cutoff(int count)
{
  _impl->set_completion_count_cutoff(count);
}

void Replxx::set_double_tab_completion(bool val)
{
  _impl->set_double_tab_completion(val);
}

void Replxx::set_complete_on_empty(bool val)
{
  _impl->set_complete_on_empty(val);
}

void Replxx::set_beep_on_ambiguous_completion(bool val)
{
  _impl->set_beep_on_ambiguous_completion(val);
}

void Replxx::set_immediate_completion(bool val)
{
  _impl->set_immediate_completion(val);
}

void Replxx::set_unique_history(bool val)
{
  _impl->set_unique_history(val);
}

void Replxx::set_no_color(bool val)
{
  _impl->set_no_color(val);
}

void Replxx::set_indent_multiline(bool val)
{
  _impl->set_indent_multiline(val);
}

void Replxx::set_max_history_size(int len)
{
  _impl->set_max_history_size(len);
}

void Replxx::clear_screen(void)
{
  _impl->clear_screen(0);
}

void Replxx::emulate_key_press(char32_t keyPress_)
{
  _impl->emulate_key_press(keyPress_);
}

Replxx::ACTION_RESULT Replxx::invoke(ACTION action_, char32_t keyPress_)
{
  return (_impl->invoke(action_, keyPress_));
}

void Replxx::bind_key(char32_t keyPress_, key_press_handler_t handler_)
{
  _impl->bind_key(keyPress_, handler_);
}

void Replxx::bind_key_internal(char32_t keyPress_, std::string_view actionName_)
{
  _impl->bind_key_internal(keyPress_, actionName_.data());
}

Replxx::State Replxx::get_state(void) const
{
  return (_impl->get_state());
}

void Replxx::set_state(Replxx::State const& state_)
{
  _impl->set_state(state_);
}

void Replxx::set_ignore_case(bool val)
{
  _impl->set_ignore_case(val);
}

int Replxx::install_window_change_handler(void)
{
  return (_impl->install_window_change_handler());
}

void Replxx::enable_bracketed_paste(void)
{
  _impl->enable_bracketed_paste();
}

void Replxx::disable_bracketed_paste(void)
{
  _impl->disable_bracketed_paste();
}

void Replxx::print(char const* format_, ...)
{
  ::std::va_list ap;
  va_start(ap, format_);
  int size = static_cast<int>(vsnprintf(nullptr, 0, format_, ap));
  va_end(ap);
  va_start(ap, format_);
  unique_ptr<char[]> buf(new char[size + 1]);
  vsnprintf(buf.get(), static_cast<size_t>(size + 1), format_, ap);
  va_end(ap);
  return (_impl->print(buf.get(), size));
}

void Replxx::write(std::string_view str)
{
  return (_impl->print(str.data(), str.size()));
}

void Replxx::set_prompt(std::string prompt)
{
  return (_impl->set_prompt(std::move(prompt)));
}

namespace color
{

Replxx::Color operator|(Replxx::Color color1_, Replxx::Color color2_)
{
  return static_cast<Replxx::Color>(static_cast<int unsigned>(color1_) | static_cast<int unsigned>(color2_));
}

Replxx::Color bg(Replxx::Color color_)
{
  return static_cast<Replxx::Color>(((static_cast<int unsigned>(color_) & 0xFFu) << 8) | color::BACKGROUND_COLOR_SET);
}

Replxx::Color bold(Replxx::Color color_)
{
  return static_cast<Replxx::Color>(static_cast<int unsigned>(color_) | color::BOLD);
}

Replxx::Color underline(Replxx::Color color_)
{
  return static_cast<Replxx::Color>(static_cast<int unsigned>(color_) | color::UNDERLINE);
}

Replxx::Color grayscale(int level_)
{
  assert((level_ >= 0) && (level_ < 24));
  return static_cast<Replxx::Color>(abs(level_) % 24 + static_cast<int unsigned>(color::GRAYSCALE));
}

Replxx::Color rgb666(int red_, int green_, int blue_)
{
  assert((red_ >= 0) && (red_ < 6) && (green_ >= 0) && (green_ < 6) && (blue_ >= 0) && (blue_ < 6));
  return static_cast<Replxx::Color>(
      (abs(red_) % 6) * 36 + (abs(green_) % 6) * 6 + (abs(blue_) % 6) + static_cast<int unsigned>(color::RGB666));
}

}  // namespace color

}  // namespace replxx
