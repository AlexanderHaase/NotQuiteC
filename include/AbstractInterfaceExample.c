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

#include "InterfaceAPI.h"


/** And finally, an example! */

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
	DEFER( EXPAND( method0, ## __VA_ARGS__ ) )

/* Third, we properties. */
#define Class0__property_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( int, property0, 0, ## __VA_ARGS__ ) )

/* Now we can define the interface */
INTERFACE_DEFINE( Class0 );

/* And forward declare an implementation */
INTERFACE_IMPLEMENT( Class0, Subclass0 )

/* Why not implement a method? */
INTERFACE_METHOD_IMPLEMENT( Class0, method0, Subclass0 ) 
{
	return self->property0++;
}

/* setup an interface instance. */
struct Subclass0Private {
	INTERFACE_INHERIT( Class0 );
	void * privateData;
	int someCounter;
};

void Subclass0Init( struct Subclass0Private * object )
{
	Class0InitAsSubclass0( INTERFACE_CAST( Class0, object ) );
	object->privateData = 0;
	object->someCounter = 1;
	INVOKE( INTERFACE_CAST( Class0, object ), method0 );
}


int main( void ) {
	return 0;
}


