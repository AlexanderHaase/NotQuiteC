#define DEFER(...) __VA_ARGS__
#define STR_IMPL( value ) #value
#define STR( value ) STR_IMPL(value)
#define CAT_2_IMPL( _a_, _b_ ) _a_ ## _b_
#define CAT_2( _a_, _b_ ) CAT_2_IMPL( _a_, _b_ )
#define CAT_3_IMPL( _a_, _b_, _c_ ) _a_ ## _b_ ## _c_
#define CAT_3( _a_, _b_, _c_ ) CAT_3_IMPL( _a_, _b_, _c_ ) 
#define CAT_4_IMPL( _a_, _b_, _c_, _d_ ) _a_ ## _b_ ## _c_ ## _d_
#define CAT_4( _a_, _b_, _c_, _d_ ) CAT_4_IMPL( _a_, _b_, _c_, _d_ )
#define APPLY( FN, ... ) DEFER( FN( __VA_ARGS__ ) )

