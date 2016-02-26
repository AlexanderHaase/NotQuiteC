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
#include "../include/InterfaceAPI.h"

/** Mutex status type */
typedef enum {
	AllocatorStatusSuccess,
	AllocatorStatusFailure,
} AllocatorStatusType;

typedef const char * CallTrace;

/** Signatures for allocating memory. 
 *
 * @param self interface implementation instance.
 * @param allocationPtr pointer to pointer for allocated memory.
 * @param size bytes to allocate.
 * @param trace debugging trace.
 * @return appropriate AllocationStatusType.
 */
#define Allocator__signature_allocate( name )	\
	AllocatorStatusType (name)( Allocator * const restrict self, void * restrict * const restrict allocationPtr, const size_t size, const CallTrace trace )

/** Signatures for freeing memory. 
 *
 * @param self interface implementation instance.
 * @param allocationPtr pointer to pointer for allocated memory.
 * @param trace debugging trace.
 * @return appropriate AllocationStatusType.
 */
#define Allocator__signature_free( name )	\
	AllocatorStatusType (name)( Allocator * const restrict self, void * restrict * const restrict allocationPtr, const CallTrace trace )

/** Allocator vtable xmacro. */
#define Allocator__vtable_xmacro( EXPAND, ... )	\
	APPLY( EXPAND, allocate, ## __VA_ARGS__ )	\
	APPLY( EXPAND, free, ## __VA_ARGS__ )

/** Allocator property xmacro. */
#define Allocator__property_xmacro( EXPAND, ... )	\
	APPLY( EXPAND, const char *, name, NULL, ## __VA_ARGS__ )

/** Mutex Factory interface. 
 *
 * Defines an abstract interface for creating/removing mutexes.
 *
 * Methods:
 *  - create
 *  - remove
 *
 * Properties:
 *  - name
 */
INTERFACE_DEFINE( Allocator );

