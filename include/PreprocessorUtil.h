/******************************************************************************

Copyright (c) 2016, Alexander Haase
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of NotQuiteC nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#pragma once
#define DEFER( ... ) __VA_ARGS__
#define STR_IMPL( value ) #value
#define STR( value )	STR_IMPL( value )
#define CAT2_IMPL( _a_, _b_ ) _a_ ## _b_
#define CAT2( _a_, _b_ ) CAT2_IMPL( _a_, _b_ )
#define CAT3_IMPL( _a_, _b_, _c_ ) _a_ ## _b_ ## _c_
#define CAT3( _a_, _b_, _c_ ) CAT3_IMPL( _a_, _b_, _c_ ) 
#define CAT4_IMPL( _a_, _b_, _c_, _d_ ) _a_ ## _b_ ## _c_ ## _d_
#define CAT4( _a_, _b_, _c_, _d_ ) CAT4_IMPL( _a_, _b_, _c_, _d_ )
#define CAT5_IMPL( _a_, _b_, _c_, _d_, _e_ ) _a_ ## _b_ ## _c_ ## _d_ ## _e_
#define CAT5( _a_, _b_, _c_, _d_, _e_ ) CAT5_IMPL( _a_, _b_, _c_, _d_, _e_ )

#define CAST_TYPE( type, symbol, ptr )	\
	type symbol = (type) ptr


#ifndef container_of
#define container_of( ptr, type, member )	\
	(( (type*) ((size_t)ptr - offsetof( type, member ) ) ))
#endif /* container_of */

#ifndef offsetof
#define offsetof( type, member )	\
	(( (size_t) &((type*)0)->member ))

#endif /* offsetof */

#ifndef restrict
#define restrict __restrict
#endif
