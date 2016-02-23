/* 

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

*/

#pragma once
#include "../include/InterfaceAPI.h"

/** Mutex status type */
typedef enum {
	MutexStatusSuccess,
	MutexStatusFailure,
} MutexStatusType;

// Forward declare Mutex for factory pattern
INTERFACE_DECLARE( Mutex );

/** Factory signature for creating/removing Mutex instances.
 *
 * @param self interface implementation instance.
 * @param mutexPtr pointer to pointer for relevant mute.
 * @return appropriate MutexStatusType.
 */
#define MutexFactory__template_lifecycleFunc( name )	\
	MutexStatusType (name)( MutexFactory * const restrict self, Mutex * restrict * const restrict mutexPtr )

/* Signatures for each Mutex lifecycle function in MutexFactory. */
#define MutexFactory__signature_create( name )	MutexFactory__template_lifecycleFunc( name )
#define MutexFactory__signature_remove( name )	MutexFactory__template_lifecycleFunc( name )

/** Factory vtable xmacro. */
#define MutexFactory__vtable_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( create, ## __VA_ARGS__ ) )	\
	DEFER( EXPAND( remove, ## __VA_ARGS__ ) )

/** Factory property xmacro. */
#define MutexFactory__property_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( const char *, name, NULL, ## __VA_ARGS__ ) )

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
INTERFACE_DEFINE( MutexFactory );


/** Mutex signature for modifying mutex state. 
 *
 * @param self interface implementation instance.
 * @return appropriate MutexStatusType
 */
#define Mutex__template_action( name )	\
	MutexStatusType (name)( Mutex * const restrict self )

/* Acquire/release method signatures. */
#define Mutex__signature_acquire( name )	Mutex__template_action( name )
#define Mutex__signature_release( name )	Mutex__template_action( name )

/** Mutex interface vtable. */
#define Mutex__vtable_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( acquire, ## __VA_ARGS__ ) )	\
	DEFER( EXPAND( release, ## __VA_ARGS__ ) )

/** Properties for Mutex interface. */
#define Mutex__property_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( MutexFactory * restrict, factory, NULL, ## __VA_ARGS__ ) )

/** Mutex interface.
 *
 * Abstract interface for syncronization.
 *
 * Methods:
 *  - acquire
 *  - release
 *
 * Properties
 *  - factory: MutexFactory instance that was the source for this Mutex.
 */
INTERFACE_DEFINE( Mutex );
