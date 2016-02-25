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
#include <stdio.h>
#include <string.h>

/*
 * Let's create a basic polymorphic interface. Say, we want an abstract base 
 * class for animals. As with any other language, we'll have to decide what 
 * methods and properties belong in the base class.
 *
 * Class Animal:
 *   Properties:
 *    - mass: How much it weighs(kg)
 *    - volume: How much of it there is(m^3)
 *    - surfaceArea: How much wrapping is around it(m^2)
 *    - name: Human factors
 *
 *   Virtual Methods:
 *    - speak(): Make some noise!
 *    - fiscalBurden(): Pesky bookeeping...
 *
 *   Concrete Methods:
 *    - compactness(): (surfaceArea)/(volume)
 *
 * Since we're targeting C, we've already had to decide which methods are
 * virtual(runtime) ploymorphic. Before we get too far, let's go ahead and take
 * a look at what this looks like in raw ISO C:
 */

/** An animal base class, of some sort, in c */
struct RawAnimal {
	float mass;	/**< How much it weighs (kg). */
	float volume;	/**< How much of it there is (m^3). */
	float surfaceArea;	/**< How much of it is a front (m^2). */
	const char * name;	/**< d'awww. */

	/** Virtual method speak().
	 *
	 * Oh hey, a function *signature*....it makes sense to dictate what the
	 * the virtual method looks like. We'll talk more about this shortly...
	 *
	 * @param animal the animal instance at hand.
	 * @param text an output buffer for giving animals voice.
	 * @param capacity how much an animal can safely voice(including NULL).
	 * @return number of characters used, not including NULL.
	 */
	size_t (*speak)( struct RawAnimal * animal, char * text, size_t capacity );

	/** Virtual method fiscalBurden()
	 *
	 * Each animal can do it's own internal bookeeping, and expose a high
	 * level sum of food, vet, medication, and legal settlement costs.
	 *
	 * @param animal the cute money pit at hand.
	 * @return financial cost today in US cents.
	 */
	size_t (*fiscalBurden)( struct RawAnimal * animal );
};


/** Concrete method compactness(), for RawAnimal.
 *
 * No matter what, we can't declare concrete methods inside structs in C.
 *
 * @param animal of interest.
 * @return ratio of surface area to volume.
 */
float rawAnimalCompactness( struct RawAnimal * animal )
{
	return animal->surfaceArea / animal->volume;
}


/** Now we can look at what subclassing looks like. */
struct CatAnimal {
	struct RawAnimal base;	/**< Base class of cat. C doesn't do much work for us. */
	size_t foodEatenToday;	/**< Nickles and dimes. */
	size_t vetVisitsToday;	/**< Our vet charges a flat fee per visit. Lucky us? */
};


/** CatAnimal method speak().
 *
 * Some observations about subclass methods:
 *  - Naming conventions tend to enter java levels of run-on verbosity.
 *  - Notice we didn't typedef the signature of RawAnimal::speak()? C doesn't
 *    allow us to implement a signature. Sadly, this isn't valid c:
 *
 *      typedef size_t (RawAnimal__signature_speak)
 *        (struct RawAnimal *  animal, char * text, size_t capacity);
 *
 *      RawAnimal__signature_speak rawAnimal__subclass_catAnimal_speak
 *      {
 *        // <function body>
 *      }
 *
 *  - We can use macros to define signatures:
 *
 *      #define RawAnimal__signature_speak( funcName )	\
 *        size_t (funcName)(struct RawAnimal * animal, char * text, size_t capcity)
 *
 *      RawAnimal__signature_speak( rawAnimal__subclass_catAnimal_speak )
 *      {
 *        // Arguments are rigidly named by macro:
 *        //  - animal
 *        //  - capactiy
 *
 *        // <function body>
 *      }
 *  - It's really tedious to match signatures across implementations in OOP--
 *    Many languages provide some level of correction and checking for us, just
 *    not C.
 *  - Documentation mostly duplicates the original.
 *
 * Anywho, back in raw C land, we'll have this implementation of cat emote 
 * according to fiscal burden to demonstrate pitfalls of virtual methods in C.
 *
 * @param animal the animal instance at hand.
 * @param text an output buffer for giving animals voice.
 * @param capacity how much an animal can safely voice(including NULL).
 * @return number of characters used, not including NULL.
 */
size_t rawAnimal__subclass_catAnimal_speak( struct RawAnimal * animal, char * text, size_t capacity )
{
	/*
	 * Calling virtual methods in raw C is a bit yucky, since C doesn't
         * supply the 'this' argument for us:
	 *   C-language: animal->fiscalBurden( animal )
	 *   OOP-language: animal->fiscalBurden()
	 *
	 * Passing 'this' gets really ridiculous with nested structs...
	 *   pets->cats->fred->fiscalBurden( pets->cats->fred )
	 */   	
	const char * disposition;

	if( animal->fiscalBurden( animal ) > 10000 )
	{
	 	disposition = "Mew:) Purrrrrrr....";
	}
	else
	{
		disposition = "Mrow!?!?!?!?!";
	}

	strncpy( text, disposition, capacity );
	return strnlen( text, capacity );
}


/** CatAnimal method fisalBurden().
 *
 * We'll assume a cat's cost is a function of vet visits and food consumed.
 * To compute that, we'll have to access the subclass's private data.
 *
 * @param animal the cute money pit at hand.
 * @return financial cost today in US cents.
 */
size_t rawAnimal__subclass_cat_fiscalBurden( struct RawAnimal * animal )
{
	// There's really no way of getting around composition casting in C.
	struct CatAnimal * cat = container_of( animal, struct CatAnimal, base );

	const size_t foodCostPerUnit = 1;	// Kibbles are cheap.
	const size_t vetCostPerUnit = 10000;	// Vets are not.

	return cat->foodEatenToday * foodCostPerUnit 
		+ cat->vetVisitsToday * vetCostPerUnit;
}


/** And finally, let's initialize an instance of our base class to complete
 * the example. */
struct CatAnimal officeCat = {
	.base = {
		.mass = 7.3,	// A little on the heavy size.
		.volume = .1,	// A shot in the dark. Cubic meters are big!
		.surfaceArea = 1,	// Another guess...getting creeeepy.
		.name = "Catattack",	// Portmanteau of cat and heart attack.

		/* ...yuck... */
		.speak = rawAnimal__subclass_catAnimal_speak,
		.fiscalBurden = rawAnimal__subclass_cat_fiscalBurden,
	},
	.foodEatenToday = 100,	// Round number!
	.vetVisitsToday = 2,	// Poor {kitty,wallet}.
};


/* Whew! We have a prototype for animal, a cat subclass, an an office cat 
 * instance. That was a lot of work, but it provides valuable context as a
 * *simple* polymorphism case in C. It turns out things can get much more
 * complex:
 *
 *  - Using vtables can provide several advantages:
 *     - They can dramatically reduce storage overhead, with lovely 
 *       implications for cache locality, especially with large virtual 
 *       vocabularies.
 *     - Type checking becomes easier: animal->vtable == &CatAnimal__vtable.
 *     - It's easy to make vtables immutable and useful:
 *          const struct RawAnimal__vtable CatAnimal__vtable = {...};
 *     - However, they only amplify readability problems.
 *        - Those can be mitigate via macros: 
 *            INVOKE( &officeCat, speak, text, 100 );
 *     - They also amplify boilerplate for setting up classes and subclasses.
 *
 *  - What about initializing the base object?
 *  - Who's in charge of naming conventions?
 *  - How does this impact readability?
 *
 * As you've likely gathered, all this serves as pretext justifying an
 * InterfaceAPI based approach. InterfaceAPI aims to deliver as much 
 * functionality as is reasonably possible in C, while automating boilerplate.
 *
 * So, let's take another look at Animal and CatAnimal (reproduced here for
 * reference):
 *
 * Class Animal:
 *   Properties:
 *    - mass: How much it weighs(kg)
 *    - volume: How much of it there is(m^3)
 *    - surfaceArea: How much wrapping is around it(m^2)
 *    - name: Human factors
 *
 *   Virtual Methods:
 *    - speak(): Make some noise!
 *    - fiscalBurdenToday(): Pesky bookeeping...
 *
 *   Concrete Methods:
 *    - compactness(): (surfaceArea)/(volume)
 */

/** Animal signature speak().
 *
 * An InterfaceAPI approach focuses on function signatures--structs work well
 * enough for objects rwithout virtual methods!
 *
 * InterfaceAPI requires us to provide a signature macro per virtual method. It
 * also requires signatures(and all other macros) to follow a naming convention
 * to allow for automated lookup.
 *
 * Function signature macros are all of the form:
 *
 *   #define InterfaceName__signature_methodName( name )	\
 *      ( InterfaceName * self, ... )
 *
 * Notes:
 *  - We use 'self' so that InterfaceAPI based code is C++ compatible.
 *  - No trailing semicolon! This macro is used to both *declare* and *define*!
 *
 * By convention, we'll use this documentation block to document the virtual
 * method signature, rather than the macro.
 *
 * @param self the animal instance at hand.
 * @param text an output buffer for giving animals voice.
 * @param capacity how much an animal can safely voice(including NULL).
 * @return number of characters used, not including NULL.
 */
#define Animal__signature_speak( name )	\
	size_t (name)( Animal * self, char * text, size_t capacity )


/** Animal signature fiscalBurden()
 *
 * Same principles as above.
 *
 * @param animal the cute money pit at hand.
 * @return financial cost today in US cents.
 */
#define Animal__signature_fiscalBurden( name )	\
	size_t (name)( Animal * self )


/** Vtable xmacro for Animal.
 *
 * Xmacros turn out to be incredibly useful for generating code. This one
 * defines all virtual methods for our Animal interface.
 *
 * As with function signatures, InterfaceAPI requires strict naming and 
 * formulation to allow for auto-generating code.
 */
#define Animal__vtable_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( speak, ## __VA_ARGS__ ) )	\
	DEFER( EXPAND( fiscalBurden, ## __VA_ARGS__ ) )

/** Property xmacro for Animal.
 *
 * Not unexpectedly, we're using another xmacro to define properties.
 *
 * Columns in this xmacro:
 *  - type: C type for the property.
 *  - name: symbol name for the property.
 *  - default: Currently unimplemented, but soon to be addressed.
 */
#define Animal__property_xmacro( EXPAND, ... )	\
	DEFER( EXPAND( float,	mass,		0,	## __VA_ARGS__ ) )	\
	DEFER( EXPAND( float,	volume,		0,	## __VA_ARGS__ ) )	\
	DEFER( EXPAND( float,	surfaceArea, 	0,	## __VA_ARGS__ ) )	\
	DEFER( EXPAND( const char *, name, 	NULL,	## __VA_ARGS__ ) )

/** Animal interface.
 *
 * Now we can define our abstract base class:
 * Class Animal:
 *   Properties:
 *    - mass: How much it weighs(kg)
 *    - volume: How much of it there is(m^3)
 *    - surfaceArea: How much wrapping is around it(m^2)
 *    - name: Human factors
 *
 *   Virtual Methods:
 *    - speak(): Make some noise!
 *    - fiscalBurdenToday(): Pesky bookeeping...
 *
 *   Concrete Methods:
 *    - compactness(): (surfaceArea)/(volume)
 */
INTERFACE_DEFINE( Animal );


/** Cat Implementation.
 *
 * Subclassing is a breeze. Vtables, etc., are handled in one call.
 */
INTERFACE_IMPLEMENT( Animal, Cat );


/** Cat method speak().
 *
 * We only need supply an implementation of each virtual method.
 */
INTERFACE_IMPLEMENT_METHOD( Animal, Cat, speak )
{
	// Arguments are per signature macro:
	//	( Animal * self, const char * text, size_t capacity )
	const char * disposition;

	// calling virtual methods is easy
	if( INVOKE( self, fiscalBurden ) > 1000 )
	{
	 	disposition = "Mew:) Purrrrrrr....";
	}
	else
	{
		disposition = "Mrow!?!?!?!?!";
	}

	strncpy( text, disposition, capacity );
	return strnlen( text, capacity );
}


/** We can quickly stand up private data via composing types */
typedef struct {
	INTERFACE_INHERIT( Animal );	/**< Animal's name is generated for us. */
	size_t foodEatenToday;	/**< Other properties may be defined as usual. */
	size_t vetVisitsToday;	/**< Casting is easy--see fiscalBurden for Cat. */
} Cat;
	

/** Cat method fiscalBurden().
 *
 * Accessing private data is very straight forward.
 */
INTERFACE_IMPLEMENT_METHOD( Animal, Cat, fiscalBurden )
{
	// INTERFACE_API provides helpful container_of equivalent. 
	Cat * cat = INTERFACE_CONTAINER( Animal, Cat, self );

	// otherwise, it's buisness as usual.
	const size_t foodCostPerUnit = 1;	// Kibbles are cheap.
	const size_t vetCostPerUnit = 10000;	// Vets are not.

	return cat->foodEatenToday * foodCostPerUnit 
		+ cat->vetVisitsToday * vetCostPerUnit;
}


/** Animal method compactness().
 * We can also embed concrete methods in the interface namespace, though it's
 * slightly cumbersome.
 */
float INTERFACE_METHOD_NAME( Animal, compactness )( Animal * self )
{
	return self->surfaceArea / self->volume;
}

/* Now we're all set to work with cats... */
int main( void )
{
	char text[ 100 ];

	// Setup a cat...
	Cat cat;
	cat.foodEatenToday = 100;	// Round number!
	cat.vetVisitsToday = 2;		// Poor {kitty,wallet}.

	INTERFACE_INIT_AS( Animal, Cat, &cat );

	Animal * animal = INTERFACE_CAST( Animal, &cat );

	animal->mass = 7.3;	// A little on the heavy size.
	animal->volume = .1;	// A shot in the dark. Cubic meters are big!
	animal->surfaceArea = 1;	// Another guess...getting creeeepy.
	animal->name = "Catattack";	// Portmanteau of cat and heart attack.

	// Work with the cat...
	INVOKE( animal, speak, text, 100 );
	printf( "%s (compactness: %02f)\n", text, CALL( Animal, compactness, animal ) );

	// Prints: "Mrow!?!?!?!?! (compactness: 10.000000)\n"

	return 0;
}
