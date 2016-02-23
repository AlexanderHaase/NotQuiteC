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

/*
 * Helper macros for quickly standing up objects.
 *
 * Key concepts:
 *  - Automate building vtables and setting up objects
 *  - Simplify indirection and method infocation
 *
 * Objects: Collection of properties, methods, in struct form. We use uniform naming
 * to allow for quickly building up subclasses.
 */

#define DEFER( ... ) __VA_ARGS__
#define CAT2_IMPL( _a_, _b_ ) _a_ ## _b_
#define CAT2( _a_, _b_ ) CAT2_IMPL( _a_, _b_ )
#define CAT3_IMPL( _a_, _b_, _c_ ) _a_ ## _b_ ## _c_
#define CAT3( _a_, _b_, _c_ ) CAT3_IMPL( _a_, _b_, _c_ ) 
#define CAT4_IMPL( _a_, _b_, _c_, _d_ ) _a_ ## _b_ ## _c_ ## _d_
#define CAT4( _a_, _b_, _c_, _d_ ) CAT4_IMPL( _a_, _b_, _c_, _d_ )
#define CAT5_IMPL( _a_, _b_, _c_, _d_, _e_ ) _a_ ## _b_ ## _c_ ## _d_ ## _e_
#define CAT5( _a_, _b_, _c_, _d_, _e_ ) CAT5_IMPL( _a_, _b_, _c_, _d_, _e_ )

#ifndef restrict
#define restrict __restrict
#endif

/** Interfaces
 *
 * Base interface mechanics: For automation to work, we must impose a strict
 * formulation of interface and interface components. Interfaces must inject
 * several symbols into the preprocessing namespace:
 *
 *  - Method signatures for each interface method.
 *  - Vtable xmacro binding method signatures together.
 *  - Property xmacro describing interface variables.
 *
 * See discussion below for how and why to name each type of symbol:
 *
 *  - Method siganture macros: For each interface method, the author must
 *    provide a signature macro. The macro must adher to naming conventions:
 *
 *    #define MyInterface__signature__myMethod( name ) \
 *        returnType ( name )( INTERFACE_TYPE( MyInterface ) *, ... )
 *
 *    Sadly, INTERFACE_TYPE() is required for method and property references to
 *    the interface--can't forward declare the typedef.
 *
 *    Conforming to this convention allows INTERFACE_METHOD_SIGNATURE() to 
 *    locate the method signature, and INTERFACE_METHOD_IMPLEMENT() to generate
 *    the function signature.
 *
 *  - The author must provide a vtable xmacro, again strictly adhearing to
 *    convention to allow for method autogeneration.
 *
 *    #define MyInterface__vtable_xmacro( EXPAND, ... )	\
 *        DEFER( EXPAND( myMethod, ## __VA_ARGS__ ) )	\
 *        DEFER( EXPAND( anotherMethod, ## __VA_ARGS__ ) )	\
 *        DEFER( EXPAND( etc, ## __VA_ARGS__ ) )
 *
 *    This xmacro allows both INTERFACE_DEFINE() and INTERFACE_IMPLEMENT() to
 *    generate interface method- and vtable- related boilerplate.
 *
 *  - The author must provide a property xmacro, though it may be empty.
 *
 *    #define MyInterface__property_xmacro( EXPAND, ... )	\
 *        DEFER( EXPAND( int, property0, default0, ## __VA_ARGS__ ) )	\
 *
 *    -- OR --
 *
 *    #define MyInterface__vtable_xmacro( EXPAND, ... )	// No properties!
 *
 *    Again, naming is key.
 *
 * DEFER(...) and __VA_ARGS__ play a crucial role in these xmacros: Windows
 * requires DEFER(...) around __VA_ARGS__ to expand them as expected, and
 * __VA_ARGS__ allow passing out-of-band infomation to xmacro EXPAND(...) 
 * functions. As such, the row pattern: 
 *
 *   DEFER( EXPAND( column0, column1, ..., ## __VA_ARGS__ ) )
 *
 * should be strictly followed, and columns must be supplied as dictated.
 */


/** Name for an interface method--used to name implementations.
 *
 * @param implementation name.
 * @param method name.
 */
#define INTERFACE_METHOD_NAME( implementation, method )	\
	CAT3( implementation, __method_, method )

/** Lookup macro for a method signature: resolves to author-supplied macros.
 *
 * @param interface
 * @param method
 */
#define INTERFACE_METHOD_SIGNATURE( interface, method )	\
	CAT3( interface,__signature_, method )

/** Name for a vtable--used both for interface vtable struct, and 
 * implementation vtable names.
 *
 * @param implementation
 */
#define INTERFACE_VTABLE_NAME( implementation )	\
	CAT2( implementation, __vtable )

/** Lookup macro for a vtable xmacro: resolves to author-supplied macros.
 *
 * @param interface
 */
#define INTERFACE_VTABLE_XMACRO( interface )	\
	CAT2( interface, __vtable_xmacro )

/** Lookup macro for a property xmacro: resolves to author-supplied macros.
 *
 * @param interface
 */
#define INTERFACE_PROPERTY_XMACRO( interface )	\
	CAT2( interface, __property_xmacro )

/** Converts a row in a property table to a variable declaration.
 *
 * Designed as an xmacro expander.
 *
 * @param type variable type.
 * @param name variable name.
 */
#define EXPAND_PROPERTY_AS_DECLARATION( type, name, ... )	type name;

/** Converts an interface vtable xmacro and method signatures to function pointers.
 *
 * Uses the out-of-band xmacro data to supply interface.
 *
 * @param method xmacro-column and name of resultant pointer. Must have corresponding signature macro.
 * @param interface out-of-band name of interface containing method.
 */
#define EXPAND_VTABLE_AS_POINTERS( method, interface )	INTERFACE_METHOD_SIGNATURE( interface, method )( *method );

/** Forward-declares implementations of each interface method.
 *
 * @param method xmacro column. Must have corresponding signature macro.
 * @param interface being implemented.
 * @param implementation name of implementation.
 */
#define EXPAND_VTABLE_AS_DECLARATIONS( method, interface, implementation )	\
	INTERFACE_METHOD_SIGNATURE( interface, method )( INTERFACE_METHOD_NAME( implementation, method ) );

/** Sometimes we'll need a type name inside the interface declaration.
 *
 * @param interface to generate typename for.
 */
#define INTERFACE_TYPE( interface )	\
	struct CAT2( interface, __struct )

/** Creates an interface type, and an interface vtable struct.
 *
 * TODO: consider typedef function pointers of each method type?
 *
 * @param interface name to be typedef'd. Must have corresponding vtable and
 *   property xmacro.
 */
#define INTERFACE_DEFINE( interface )	\
	typedef INTERFACE_TYPE( interface ) {	\
		const struct INTERFACE_VTABLE_NAME( interface ) {	\
			INTERFACE_VTABLE_XMACRO( interface )( EXPAND_VTABLE_AS_POINTERS, interface )	\
		} * restrict vtable;	\
		INTERFACE_PROPERTY_XMACRO( interface )( EXPAND_PROPERTY_AS_DECLARATION )	\
	} interface


/** Interface implementation
 *
 * Interface implementation is much more straight forward than declaration.
 * Specifically, it doesn't require injecting any preprocessing symbols. An
 * implementation needs to supply a function for each interface method using
 * the INTERFACE_METHOD_IMPLEMENT() macro, and define the implementation
 * objects via the INTERFACE_IMPLEMENT() macro. Implementations can be 
 * customized by breaking out the steps in the INTERFACE_IMPLEMENT() macro as
 * needed.
 */


/** Macro for naming and defining the signature of a interface method 
 * implementation.
 *
 * Uniform naming allows other macros to locate the method and automate
 * boilerplate. Can be used to declare or define a function.
 *
 * @param interface containing the method
 * @param method to be implemented--lookup for signature.
 * @param implementation name of implementation.
 */
#define INTERFACE_METHOD_IMPLEMENT( interface, method, implementation )	\
	INTERFACE_METHOD_SIGNATURE( interface, method )( INTERFACE_METHOD_NAME( implementation, method ) )

/** Macro for naming and defining the signature of a interface method 
 * implementation.
 *
 * Uniform naming allows other macros to locate the method and automate
 * boilerplate. Defines a pointer that must be initialized. Pointer value will
 * propagate to implementation vtable.
 *
 * INTERFACE_METHOD_IMPLEMENT_POINTER( Class, doSomething, SubclassA ) = INTERFACE_METHOD_NAME( SubclassB, doSomething )
 *
 * @param interface containing the method
 * @param method to be implemented--lookup for signature.
 * @param implementation name of implementation.
 */
#define INTERFACE_METHOD_IMPLEMENT_POINTER( interface, method, implementation ) \
	INTERFACE_METHOD_SIGNATURE( interface, method )( * const restrict INTERFACE_METHOD_NAME( implementation, method ) )

/** Macro for naming and defining the signature of a interface method 
 * implementation.
 *
 * Uniform naming allows other macros to locate the method and automate
 * boilerplate. Borrows the implementation from the other class.
 *
 * INTERFACE_METHOD_IMPLEMENT_POINTER( Class, doSomething, SubclassA ) = INTERFACE_METHOD_NAME( SubclassB, doSomething )
 *
 * @param interface containing the method
 * @param method to be implemented--lookup for signature.
 * @param implementation name of implementation.
 * @param source name of implementation to inherit.
 */
#define INTERFACE_METHOD_IMPLEMENT_INHERIT( interface, method, implementation, source ) \
	extern INTERFACE_METHOD_IMPLEMENT( interface, method, source );	\
	INTERFACE_METHOD_IMPLEMENT_POINTER( interface, method, implementation ) = \
		INTERFACE_METHOD_NAME( from, method );

/** Converts an interface xmacro to a vtable initializer for an implementation.
 *
 * @param method xmacro-column method name.
 * @param implementation out-of-band name.
 */
#define EXPAND_AS_VTABLE_INIT( method, implementation )	\
	.method = INTERFACE_METHOD_NAME( implementation, method ),

/** Creates a vtable for the given implementation.
 *
 * All methods must be implemented or declared prior to use.
 *
 * @param interface defining vtable.
 * @param implementation.
 */
#define INTERFACE_VTABLE_IMPLEMENT( interface, implementation )	\
	const struct INTERFACE_VTABLE_NAME( interface ) INTERFACE_VTABLE_NAME( implementation ) = {	\
		INTERFACE_VTABLE_XMACRO( interface )( EXPAND_AS_VTABLE_INIT, implementation )	\
	}

/** Forward declares all methods, creates vtable, and creates initialization function for an implementation.
 *
 * TODO: Consider interface properties....
 *
 * @param interface being implemented
 * @param implementation name
 */
#define INTERFACE_IMPLEMENT( interface, implementation )	\
	INTERFACE_VTABLE_XMACRO( interface )( EXPAND_VTABLE_AS_DECLARATIONS, interface, implementation )	\
	INTERFACE_VTABLE_IMPLEMENT( Class0, Subclass0 );	\
	void CAT3( interface, InitAs, implementation )( INTERFACE_TYPE( interface ) * const restrict instance )	\
	{	\
		instance->vtable = &INTERFACE_VTABLE_NAME( implementation );	\
	}


#define INTERFACE_INHERIT( interface )	\
	interface CAT2( interface, __instance )

#define INTERFACE_CAST( interface, object )	\
	&(object)->CAT2( interface, __instance )

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
	int (name)( INTERFACE_TYPE( Class0 ) * self )

/* Second, we supply a vtable. Note, all the names must match... */
#define Class0__vtable_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( method0, ## __VA_ARGS__ ) )

/* Third, we properties. */
#define Class0__property_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( int, property0, 0, ## __VA_ARGS__ ) )

/* Now we can define the interface */
INTERFACE_DEFINE( Class0 );

/* And forward declare an implementation */
INTERFACE_IMPLEMENT( Class0, Subclass0 );

/* Why not implement a method? */
INTERFACE_METHOD_IMPLEMENT( Class0, method0, Subcalss0 ) 
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
}



