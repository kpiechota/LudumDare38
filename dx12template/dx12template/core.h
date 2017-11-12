#pragma once

#ifdef _DEBUG
#define ASSERT( condition ) assert( condition )
#define ASSERT_STR( condition, msg ) assert( condition && msg )
#define CT_ASSERT( condition ) static_assert( condition )
#else
#define ASSERT( condition ) 
#define ASSERT_STR( condition, msg ) 
#define CT_ASSERT( condition ) 
#endif

#define NOVTABLE __declspec(novtable)
#define FORCE_INLINE __forceinline
#define FLAG( f ) (1 << f)

#include "types.h"
#include "array.h"
#include "math.h"
