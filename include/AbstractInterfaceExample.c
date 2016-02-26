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

#include "InterfaceAPI.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * This example assumes the reader is already on board with virtual methods
 * and C. As such, it's more of a terse convention example.
 */

/** First, we supply a method signature.
 *
 * By convention, we stick the doxygen for the signature here, rather than the
 * doxygen for the macro. Signature macros are pretty boring; signatures are 
 * not.
 *
 * @param self pointer to interface instance.
 * @return some value.
 */
#define Class0__signature_method0( name ) \
	int (name)( Class0 * self )

/* Second, we supply a vtable. Note, all the names must match... */
#define Class0__vtable_xmacro( EXPAND, ... )	\
	APPLY( EXPAND, method0, ## __VA_ARGS__ )

/* Third, we supply properties. Again, names must match. */
#define Class0__property_xmacro( EXPAND, ... )	\
	APPLY( EXPAND, int, property0, 0, ## __VA_ARGS__ )

/** Now we can define the interface.
 *
 * Virtual Methods:
 *  - method0
 *
 * Properties:
 *  - property0
 *
 * static Methods:
 *  - method1
 */
INTERFACE_DEFINE( Class0 );

/* Setup static method in class0 namespace */
const char * INTERFACE_METHOD_NAME( Class0, name )( void )
{
	return STR( Class0 );
}

/* And forward declare an implementation */
INTERFACE_IMPLEMENT( Class0, Subclass0 );


/* Setup an interface instance, complete with subclass properties.
 *
 * Note: The name of the container need not match the implementation. Doing so
 * does help with readability though.
 */
typedef struct  {
	INTERFACE_INHERIT( Class0 );	// Macro names instance per convention.
	void * privateData;
	int someCounter;
} Subclass0;


/* Why not implement a method? */
INTERFACE_IMPLEMENT_METHOD( Class0, Subclass0, method0 ) 
{	
	// INTERFACE_CONTAINER(...) statically casts to the containing object.
	return INTERFACE_CONTAINER( Class0, Subclass0, self )->someCounter + self->property0++;
}



int main( void ) {

	/* setup a static instance */
	Subclass0 instance0 = {
		.INTERFACE_INSTANCE( Class0 ) =
		{
			.property0 = 0,
			INTERFACE_VTABLE_INITIALIZER( Subclass0 ),
		},
		.privateData = NULL,
		.someCounter = 1,
	};

	/* setup a dynamic instance */
	Subclass0 * instance1 = malloc( sizeof( Subclass0 ) );
	
	INTERFACE_INIT_AS( Class0, Subclass0, instance1 );	// setup vtable.
	instance1->INTERFACE_INSTANCE( Class0 ).property0 = 1;	// Access base class.
	instance1->privateData = instance1;
	instance1->someCounter = 2;

	/* cast to interface */
	Class0 * interfaceArray[ 2 ] = {
		INTERFACE_CAST( Class0, &instance0 ),
		INTERFACE_CAST( Class0, instance1 ),
	};

	/* use static methods */
	printf( "name(): %s\n", CALL( Class0, name ) ); 

	/* use virtual methods */
	int index;
	for( index = 0; index < 2; index++ ) {
		printf( "instance: %d, method0(): %d\n", index, INVOKE( interfaceArray[ index ], method0 ) );
	}

	return 0;
}


