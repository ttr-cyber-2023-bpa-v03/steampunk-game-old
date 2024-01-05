#pragma once

#include <iostream>

#define SG_STRINGIZE_IMPL(x) #x
#define SG_STRINGIZE(x) SG_STRINGIZE_IMPL(x)
#define SG_LOCATE std::cout << __func__ << " at " << SG_STRINGIZE(__LINE__) << ":" << __FILE__ << std::endl