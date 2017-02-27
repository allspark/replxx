#ifndef REPLXX_IO_HXX_INCLUDED
#define REPLXX_IO_HXX_INCLUDED 1

#ifdef _WIN32
#include <windows.h>
#endif

namespace replxx {

int write32( int fd, char32_t* text32, int len32 );
int getScreenColumns(void);
int getScreenRows(void);
void setDisplayAttribute(bool enhancedDisplay);
int enableRawMode(void);
void disableRawMode(void);
char32_t readUnicodeCharacter(void);
void beep();
char32_t read_char(void);

#ifdef _WIN32
extern HANDLE console_out;
#endif

}

#endif

