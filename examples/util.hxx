#pragma once

// strrpbrk would suffice but is not portable.

#include <string_view>

int context_len( std::string_view );
int utf8str_codepoint_len( std::string_view, int );

