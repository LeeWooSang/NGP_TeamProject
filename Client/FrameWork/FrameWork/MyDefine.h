#pragma once
#include "MyInclude.h"
#define SAFE_DELETE(p) {if(p){ delete(p); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p){ delete[](p); (p) = NULL;}}



using std::cout;
