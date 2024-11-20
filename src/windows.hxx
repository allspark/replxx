#ifndef REPLXX_WINDOWS_HXX_INCLUDED
#define REPLXX_WINDOWS_HXX_INCLUDED 1

#include <conio.h>
#include <io.h>
#include <windows.h>

namespace replxx
{

static int const FOREGROUND_WHITE =
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static int const BACKGROUND_WHITE =
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
static int const INTENSITY = FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;

class WinAttributes
{
public:
  WinAttributes()
  {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    _defaultAttribute = info.wAttributes & INTENSITY;
    _defaultColor = info.wAttributes & FOREGROUND_WHITE;
    _defaultBackground = info.wAttributes & BACKGROUND_WHITE;

    _consoleAttribute = _defaultAttribute;
    _consoleColor = _defaultColor | _defaultBackground;
  }

public:
  int _defaultAttribute;
  int _defaultColor;
  int _defaultBackground;

  int _consoleAttribute;
  int _consoleColor;
};

int win_write(HANDLE, bool, char const*, int);

extern WinAttributes WIN_ATTR;

}  // namespace replxx

#endif
