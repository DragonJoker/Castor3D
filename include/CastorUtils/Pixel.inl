#include <emmintrin.h>

namespace Castor
{
	namespace detail
	{
		/*!
		\author 	Sylvain DOREMUS
		\date 		27/05/2013
		\~english
		\brief		Holds colour/depth/stencil component helper functions
		\~french
		\brief		Contient des fonctions d'aides à la conversion de composantes couleur/profondeur/stencil
		*/
		template< TPL_PIXEL_FORMAT PF > struct component;
		//!\~english	Specialisation for ePIXEL_FORMAT_L8	\~french	Spécialisation pour ePIXEL_FORMAT_L8
		template<> struct component< ePIXEL_FORMAT_L8 >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	AB	( uint8_t const *			) { return 0xFF; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_L16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_L16F32F
		template<> struct component< ePIXEL_FORMAT_L16F32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		AF	( uint8_t const *			) { return 1.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_L32F	\~french	Spécialisation pour ePIXEL_FORMAT_L32F
		template<> struct component< ePIXEL_FORMAT_L32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		AF	( uint8_t const *			) { return 1.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_A8L8	\~french	Spécialisation pour ePIXEL_FORMAT_A8L8
		template<> struct component< ePIXEL_FORMAT_A8L8 >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return p_pSrc[0]; }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return p_pSrc[1]; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_AL16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_AL16F32F
		template<> struct component< ePIXEL_FORMAT_AL16F32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[1]; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_AL32F	\~french	Spécialisation pour ePIXEL_FORMAT_AL32F
		template<> struct component< ePIXEL_FORMAT_AL32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[0]; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return ((float*)p_pSrc)[1]; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_A1R5G5B5	\~french	Spécialisation pour ePIXEL_FORMAT_A1R5G5B5
		template<> struct component< ePIXEL_FORMAT_A1R5G5B5 >
		{
#define src	((uint16_t*)p_pSrc)[0]
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0x001F) << 3 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0x03E0) >> 2 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0x7C00) >> 7 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return p_pSrc[1] & 0x8000 ? 0xFF : 0x00; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
#undef src
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_A4R4G4B4	\~french	Spécialisation pour ePIXEL_FORMAT_A4R4G4B4
		template<> struct component< ePIXEL_FORMAT_A4R4G4B4 >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( (p_pSrc[0] & 0x0F) << 4 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( (p_pSrc[0] & 0xF0) << 0 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( (p_pSrc[1] & 0x0F) << 4 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( (p_pSrc[1] & 0xF0) << 0 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_R5G6B5	\~french	Spécialisation pour ePIXEL_FORMAT_R5G6B5
		template<> struct component< ePIXEL_FORMAT_R5G6B5 >
		{
#define src	((uint16_t*)p_pSrc)[0]
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0x001F) << 3 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0x03D0) >> 2 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( (src & 0xF800) >> 8 ); }
			static inline uint8_t	AB	( uint8_t const *			) { return 0xFF; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
#undef src
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_R8G8B8	\~french	Spécialisation pour ePIXEL_FORMAT_R8G8B8
		template<> struct component< ePIXEL_FORMAT_R8G8B8 >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[0] ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[1] ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[2] ); }
			static inline uint8_t	AB	( uint8_t const *			) { return 0xFF; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_A8R8G8B8	\~french	Spécialisation pour ePIXEL_FORMAT_A8R8G8B8
		template<> struct component< ePIXEL_FORMAT_A8R8G8B8 >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[0] ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[1] ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[2] ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( p_pSrc[3] ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_RGB16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_RGB16F32F
		template<> struct component< ePIXEL_FORMAT_RGB16F32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return float( RF( p_pSrc ) * 0.30 + GF( p_pSrc ) * 0.59 + BF( p_pSrc ) * 0.11); }
			static inline float		RF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[1]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[2]; }
			static inline float		AF	( uint8_t const *			) { return 1.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_ARGB16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_ARGB16F32F
		template<> struct component< ePIXEL_FORMAT_ARGB16F32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return float( RF( p_pSrc ) * 0.30 + GF( p_pSrc ) * 0.59 + BF( p_pSrc ) * 0.11); }
			static inline float		RF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[1]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[2]; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[3]; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_RGB32F	\~french	Spécialisation pour ePIXEL_FORMAT_RGB32F
		template<> struct component< ePIXEL_FORMAT_RGB32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return float( RF( p_pSrc ) * 0.30 + GF( p_pSrc ) * 0.59 + BF( p_pSrc ) * 0.11); }
			static inline float		RF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[1]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[2]; }
			static inline float		AF	( uint8_t const *			) { return 1.0f; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_ARGB32F	\~french	Spécialisation pour ePIXEL_FORMAT_ARGB32F
		template<> struct component< ePIXEL_FORMAT_ARGB32F >
		{
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( LF( p_pSrc ) * 255 ); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( RF( p_pSrc ) * 255 ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( GF( p_pSrc ) * 255 ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( BF( p_pSrc ) * 255 ); }
			static inline uint8_t	AB	( uint8_t const * p_pSrc	) { return uint8_t( AF( p_pSrc ) * 255 ); }
			static inline float		LF	( uint8_t const * p_pSrc	) { return float( RF( p_pSrc ) * 0.30 + GF( p_pSrc ) * 0.59 + BF( p_pSrc ) * 0.11); }
			static inline float		RF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[0]; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[1]; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[2]; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return reinterpret_cast< float const * >( p_pSrc )[3]; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_YUY2	\~french	Spécialisation pour ePIXEL_FORMAT_YUY2
		template<> struct component< ePIXEL_FORMAT_YUY2 >
		{
#	define YUV_Y(x)		(((x)[0] & 0xF0) & (((x)[1] & 0xF0) >> 2))
#	define YUV_U(x)		((x)[0] & 0x0F)
#	define YUV_V(x)		((x)[1] & 0x0F)
#	define YUV_C(x)		(YUV_Y(x) -  16)
#	define YUV_D(x)		(YUV_U(x) - 128)
#	define YUV_E(x)		(YUV_V(x) - 128)
#	define YUV_clip(x)	static_cast<uint8_t>( (x) > 255 ? 255 : (x) < 0 ? 0 : (x))
			static inline uint8_t	LB	( uint8_t const * p_pSrc	) { return uint8_t( RB( p_pSrc ) * 0.30 + GB( p_pSrc ) * 0.59 + BB( p_pSrc ) * 0.11); }
			static inline uint8_t	RB	( uint8_t const * p_pSrc	) { return uint8_t( YUV_clip(( 298 * YUV_C(p_pSrc)						 + 409 * YUV_E(p_pSrc) + 128) >> 8) ); }
			static inline uint8_t	GB	( uint8_t const * p_pSrc	) { return uint8_t( YUV_clip(( 298 * YUV_C(p_pSrc) - 100 * YUV_D(p_pSrc) - 208 * YUV_E(p_pSrc) + 128) >> 8) ); }
			static inline uint8_t	BB	( uint8_t const * p_pSrc	) { return uint8_t( YUV_clip(( 298 * YUV_C(p_pSrc) + 516 * YUV_D(p_pSrc)				       + 128) >> 8) ); }
			static inline uint8_t	AB	( uint8_t const *			) { return 0xFF; }
			static inline float		LF	( uint8_t const * p_pSrc	) { return LB( p_pSrc ) / 255.0f; }
			static inline float		RF	( uint8_t const * p_pSrc	) { return RB( p_pSrc ) / 255.0f; }
			static inline float		GF	( uint8_t const * p_pSrc	) { return GB( p_pSrc ) / 255.0f; }
			static inline float		BF	( uint8_t const * p_pSrc	) { return BB( p_pSrc ) / 255.0f; }
			static inline float		AF	( uint8_t const * p_pSrc	) { return AB( p_pSrc ) / 255.0f; }
#	undef YUV_Y
#	undef YUV_U
#	undef YUV_V
#	undef YUV_C
#	undef YUV_D
#	undef YUV_E
#	undef YUV_clip
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH16	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH16
		template<> struct component< ePIXEL_FORMAT_DEPTH16 >
		{
			static inline uint16_t	DB16	( uint8_t const * p_pSrc ) { return *(uint16_t*)p_pSrc; }
			static inline uint32_t	DB24	( uint8_t const * p_pSrc ) { return uint32_t( *(uint16_t*)p_pSrc ); }
			static inline uint32_t	DB24S8	( uint8_t const * p_pSrc ) { return uint32_t( *(uint16_t*)p_pSrc ); }
			static inline uint32_t	DB32	( uint8_t const * p_pSrc ) { return uint32_t( *(uint16_t*)p_pSrc ); }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH24	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH24
		template<> struct component< ePIXEL_FORMAT_DEPTH24 >
		{
			static inline uint16_t	DB16	( uint8_t const * p_pSrc ) { return uint16_t( ((*(uint32_t*)p_pSrc) & 0x00FFFFFF) >> 8); }
			static inline uint32_t	DB24	( uint8_t const * p_pSrc ) { return uint32_t( (*(uint32_t*)p_pSrc) & 0x00FFFFFF ); }
			static inline uint32_t	DB24S8	( uint8_t const * p_pSrc ) { return uint32_t( (*(uint32_t*)p_pSrc) & 0xFFFFFF00 ) | 0x000000FF; }
			static inline uint32_t	DB32	( uint8_t const * p_pSrc ) { return uint32_t( (*(uint32_t*)p_pSrc) & 0x00FFFFFF ); }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH24S8	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH24S8
		template<> struct component< ePIXEL_FORMAT_DEPTH24S8 >
		{
			static inline uint8_t	BIT		( uint8_t const  p_bySrc ) { return ((p_bySrc >> 0) & 0x01) | ((p_bySrc >> 1) & 0x01) | ((p_bySrc >> 2) & 0x01) | ((p_bySrc >> 3) & 0x01) | ((p_bySrc >> 4) & 0x01) | ((p_bySrc >> 5) & 0x01) | ((p_bySrc >> 6) & 0x01) | ((p_bySrc >> 7) & 0x01); }
			static inline uint16_t	DB16	( uint8_t const * p_pSrc ) { return uint16_t( ((*(uint32_t*)p_pSrc) & 0xFFFFFF00) >> 8 ); }
			static inline uint32_t	DB24	( uint8_t const * p_pSrc ) { return uint32_t( ((*(uint32_t*)p_pSrc) & 0xFFFFFF00) >> 8 ); }
			static inline uint32_t	DB24S8	( uint8_t const * p_pSrc ) { return *(uint32_t*)p_pSrc; }
			static inline uint32_t	DB32	( uint8_t const * p_pSrc ) { return uint32_t( ((*(uint32_t*)p_pSrc) & 0xFFFFFF00) >> 8 ); }
			static inline uint8_t	S1		( uint8_t const * p_pSrc ) { return (BIT( p_pSrc[3] ) << 0) | (BIT( p_pSrc[7] ) << 1) | (BIT( p_pSrc[11] ) << 2) | (BIT( p_pSrc[15] ) << 3) | (BIT( p_pSrc[19] ) << 4) | (BIT( p_pSrc[23] ) << 5) | (BIT( p_pSrc[27] ) << 6) | (BIT( p_pSrc[31] ) << 7); }
			static inline uint8_t	S8		( uint8_t const * p_pSrc ) { return p_pSrc[3]; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH32	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH32
		template<> struct component< ePIXEL_FORMAT_DEPTH32 >
		{
			static inline uint16_t	DB16	( uint8_t const * p_pSrc ) { return uint16_t( (*(uint32_t*)p_pSrc) >> 16 ); }
			static inline uint32_t	DB24	( uint8_t const * p_pSrc ) { return uint32_t( (*(uint32_t*)p_pSrc) >> 8 ); }
			static inline uint32_t	DB24S8	( uint8_t const * p_pSrc ) { return (*(uint32_t*)p_pSrc) | 0x000000FF; }
			static inline uint32_t	DB32	( uint8_t const * p_pSrc ) { return *(uint32_t*)p_pSrc; }
		};
		//!\~english	Specialisation for ePIXEL_FORMAT_STENCIL8	\~french	Spécialisation pour ePIXEL_FORMAT_STENCIL8
		template<> struct component< ePIXEL_FORMAT_STENCIL8 >
		{
			static inline uint8_t	BIT		( uint8_t const  p_bySrc ) { return ((p_bySrc >> 0) & 0x01) | ((p_bySrc >> 1) & 0x01) | ((p_bySrc >> 2) & 0x01) | ((p_bySrc >> 3) & 0x01) | ((p_bySrc >> 4) & 0x01) | ((p_bySrc >> 5) & 0x01) | ((p_bySrc >> 6) & 0x01) | ((p_bySrc >> 7) & 0x01); }
			static inline uint32_t	DB24S8	( uint8_t const * p_pSrc ) { return 0xFFFFFF00 + *p_pSrc; }
			static inline uint8_t	S1		( uint8_t const * p_pSrc ) { return (BIT( p_pSrc[3] ) << 0) | (BIT( p_pSrc[7] ) << 1) | (BIT( p_pSrc[11] ) << 2) | (BIT( p_pSrc[15] ) << 3) | (BIT( p_pSrc[19] ) << 4) | (BIT( p_pSrc[23] ) << 5) | (BIT( p_pSrc[27] ) << 6) | (BIT( p_pSrc[31] ) << 7); }
			static inline uint8_t	S8		( uint8_t const * p_pSrc ) { return *p_pSrc; }
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		27/05/2013
		\~english
		\brief		Structure used to convert one pixel from one format to another one
		\~french
		\brief		Structure utilisée pour convertir un pixel d'un format vers un autre
		*/
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst, typename Enable=void > struct pixel_converter;
		//!\~english	Specialisation when source and destination formats are the same	\~french	Spécialisation quand les formats source et destination sont identiques
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc == PFDst >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				std::memcpy( p_pDst, p_pSrc, pixel_definitions< PFSrc >::size );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_L8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = component< PFSrc >::LB( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_L16F32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_L16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< float* >( p_pDst ) = component< PFSrc >::LF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_L32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_L32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< float* >( p_pDst ) = component< PFSrc >::LF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_A8L8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_A8L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A8L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = component< PFSrc >::LB( p_pSrc );
				p_pDst[1] = component< PFSrc >::AB( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_AL16F32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_AL16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_AL16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float* >( p_pDst );
				l_pDst[0] = component< PFSrc >::LF( p_pSrc );
				l_pDst[1] = component< PFSrc >::AF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_AL32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_AL32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_AL32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float* >( p_pDst );
				l_pDst[0] = component< PFSrc >::LF( p_pSrc );
				l_pDst[1] = component< PFSrc >::AF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_A1R5G5B5	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_A1R5G5B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A1R5G5B5 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast<uint16_t*>(p_pDst) =	0x8000 |
														((component< PFSrc >::RB( p_pSrc ) >> 3) << 10) |
														((component< PFSrc >::GB( p_pSrc ) >> 3) <<  5) |
														((component< PFSrc >::BB( p_pSrc ) >> 3) <<  0);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_A4R4G4B4	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_A4R4G4B4
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A4R4G4B4 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = (component< PFSrc >::RB( p_pSrc ) & 0xF0) | (component< PFSrc >::GB( p_pSrc ) >> 4);
				p_pDst[1] = (component< PFSrc >::AB( p_pSrc ) & 0xF0) | (component< PFSrc >::BB( p_pSrc ) >> 4);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_R5G6B5	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_R5G6B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_R5G6B5 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast<uint16_t*>(p_pDst) =	((component< PFSrc >::RB( p_pSrc ) >> 3) << 11) |
														((component< PFSrc >::GB( p_pSrc ) >> 3) <<  5) |
														((component< PFSrc >::BB( p_pSrc ) >> 3) <<  0);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_R8G8B8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_R8G8B8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = component< PFSrc >::RB( p_pSrc );
				p_pDst[1] = component< PFSrc >::GB( p_pSrc );
				p_pDst[2] = component< PFSrc >::BB( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_A8R8G8B8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_A8R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A8R8G8B8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = component< PFSrc >::RB( p_pSrc );
				p_pDst[1] = component< PFSrc >::GB( p_pSrc );
				p_pDst[2] = component< PFSrc >::BB( p_pSrc );
				p_pDst[3] = component< PFSrc >::AB( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_RGB16F32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_RGB16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_RGB16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float * >( p_pDst );
				l_pDst[0] = component< PFSrc >::RF( p_pSrc );
				l_pDst[1] = component< PFSrc >::GF( p_pSrc );
				l_pDst[2] = component< PFSrc >::BF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_ARGB16F32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_ARGB16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_ARGB16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float * >( p_pDst );
				l_pDst[0] = component< PFSrc >::RF( p_pSrc );
				l_pDst[1] = component< PFSrc >::GF( p_pSrc );
				l_pDst[2] = component< PFSrc >::BF( p_pSrc );
				l_pDst[3] = component< PFSrc >::AF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_RGB32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_RGB32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_RGB32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float * >( p_pDst );
				l_pDst[0] = component< PFSrc >::RF( p_pSrc );
				l_pDst[1] = component< PFSrc >::GF( p_pSrc );
				l_pDst[2] = component< PFSrc >::BF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_ARGB32F	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_ARGB32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_ARGB32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				float * l_pDst = reinterpret_cast< float * >( p_pDst );
				l_pDst[0] = component< PFSrc >::RF( p_pSrc );
				l_pDst[1] = component< PFSrc >::GF( p_pSrc );
				l_pDst[2] = component< PFSrc >::BF( p_pSrc );
				l_pDst[3] = component< PFSrc >::AF( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_YUY2	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_YUY2
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_YUY2 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = (((((66 * component< PFSrc >::RB( p_pSrc ) + 129 * component< PFSrc >::GB( p_pSrc ) +  25 * component< PFSrc >::BB( p_pSrc ) + 128) >> 8) +  16) & 0x0F) << 4) | ((((-38 * component< PFSrc >::RB( p_pSrc ) - 74 * component< PFSrc >::GB( p_pSrc ) + 112 * component< PFSrc >::BB( p_pSrc ) + 128) >> 8) + 128) & 0x0F);
				p_pDst[1] = (((((66 * component< PFSrc >::RB( p_pSrc ) + 129 * component< PFSrc >::GB( p_pSrc ) +  25 * component< PFSrc >::BB( p_pSrc ) + 128) >> 8) +  16) & 0x0F) << 4) | ((((112 * component< PFSrc >::RB( p_pSrc ) - 94 * component< PFSrc >::GB( p_pSrc ) -  18 * component< PFSrc >::BB( p_pSrc ) + 128) >> 8) + 128) & 0x0F);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_DEPTH16	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH16
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH16 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< uint16_t * >( p_pDst ) = component< PFDst >::DB16( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_DEPTH24	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH24
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH24 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< uint32_t * >( p_pDst ) = component< PFSrc >::DB24( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_DEPTH24S8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFSrc != ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_DEPTH24S8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< uint32_t * >( p_pDst ) = (component< PFSrc >::DB24S8( p_pSrc ) << 8 | 0x000000FF);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_DEPTH32	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH32
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH32 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*reinterpret_cast< uint32_t * >( p_pDst ) = component< PFSrc >::DB32( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_STENCIL1	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_STENCIL1
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFDst == ePIXEL_FORMAT_STENCIL1 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*p_pDst = component< PFSrc >::S1( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for converting to ePIXEL_FORMAT_STENCIL8	\~french	Spécialisation pour convertir vers ePIXEL_FORMAT_STENCIL8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFSrc != ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_STENCIL8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*p_pDst = component< PFSrc >::S8( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for convertions from ePIXEL_FORMAT_STENCIL1 to ePIXEL_FORMAT_STENCIL8	\~french	Spécialisation pour convertir de ePIXEL_FORMAT_STENCIL1 vers ePIXEL_FORMAT_STENCIL8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFSrc == ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_STENCIL8 >::type >
		{
			inline uint8_t BIT( uint8_t const src, uint8_t uiIndex ) { return ((src >> uiIndex) & 0x01); }

			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = (BIT( *p_pSrc, 0 ) << 0) | (BIT( *p_pSrc, 0 ) << 1) | (BIT( *p_pSrc, 0 ) << 2) | (BIT( *p_pSrc, 0 ) << 3) | (BIT( *p_pSrc, 0 ) << 4) | (BIT( *p_pSrc, 0 ) << 5) | (BIT( *p_pSrc, 0 ) << 6) | (BIT( *p_pSrc, 0 ) << 7);
				p_pDst[1] = (BIT( *p_pSrc, 1 ) << 0) | (BIT( *p_pSrc, 1 ) << 1) | (BIT( *p_pSrc, 1 ) << 2) | (BIT( *p_pSrc, 1 ) << 3) | (BIT( *p_pSrc, 1 ) << 4) | (BIT( *p_pSrc, 1 ) << 5) | (BIT( *p_pSrc, 1 ) << 6) | (BIT( *p_pSrc, 1 ) << 7);
				p_pDst[2] = (BIT( *p_pSrc, 2 ) << 0) | (BIT( *p_pSrc, 2 ) << 1) | (BIT( *p_pSrc, 2 ) << 2) | (BIT( *p_pSrc, 2 ) << 3) | (BIT( *p_pSrc, 2 ) << 4) | (BIT( *p_pSrc, 2 ) << 5) | (BIT( *p_pSrc, 2 ) << 6) | (BIT( *p_pSrc, 2 ) << 7);
				p_pDst[3] = (BIT( *p_pSrc, 3 ) << 0) | (BIT( *p_pSrc, 3 ) << 1) | (BIT( *p_pSrc, 3 ) << 2) | (BIT( *p_pSrc, 3 ) << 3) | (BIT( *p_pSrc, 3 ) << 4) | (BIT( *p_pSrc, 3 ) << 5) | (BIT( *p_pSrc, 3 ) << 6) | (BIT( *p_pSrc, 3 ) << 7);
				p_pDst[4] = (BIT( *p_pSrc, 4 ) << 0) | (BIT( *p_pSrc, 4 ) << 1) | (BIT( *p_pSrc, 4 ) << 2) | (BIT( *p_pSrc, 4 ) << 3) | (BIT( *p_pSrc, 4 ) << 4) | (BIT( *p_pSrc, 4 ) << 5) | (BIT( *p_pSrc, 4 ) << 6) | (BIT( *p_pSrc, 4 ) << 7);
				p_pDst[5] = (BIT( *p_pSrc, 5 ) << 0) | (BIT( *p_pSrc, 5 ) << 1) | (BIT( *p_pSrc, 5 ) << 2) | (BIT( *p_pSrc, 5 ) << 3) | (BIT( *p_pSrc, 5 ) << 4) | (BIT( *p_pSrc, 5 ) << 5) | (BIT( *p_pSrc, 5 ) << 6) | (BIT( *p_pSrc, 5 ) << 7);
				p_pDst[6] = (BIT( *p_pSrc, 6 ) << 0) | (BIT( *p_pSrc, 6 ) << 1) | (BIT( *p_pSrc, 6 ) << 2) | (BIT( *p_pSrc, 6 ) << 3) | (BIT( *p_pSrc, 6 ) << 4) | (BIT( *p_pSrc, 6 ) << 5) | (BIT( *p_pSrc, 6 ) << 6) | (BIT( *p_pSrc, 6 ) << 7);
				p_pDst[7] = (BIT( *p_pSrc, 7 ) << 0) | (BIT( *p_pSrc, 7 ) << 1) | (BIT( *p_pSrc, 7 ) << 2) | (BIT( *p_pSrc, 7 ) << 3) | (BIT( *p_pSrc, 7 ) << 4) | (BIT( *p_pSrc, 7 ) << 5) | (BIT( *p_pSrc, 7 ) << 6) | (BIT( *p_pSrc, 7 ) << 7);
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		//!\~english	Specialisation for convertions from ePIXEL_FORMAT_STENCIL1 to ePIXEL_FORMAT_DEPTH24S8	\~french	Spécialisation pour convertir de ePIXEL_FORMAT_STENCIL1 vers ePIXEL_FORMAT_DEPTH24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc != PFDst && PFSrc == ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_DEPTH24S8 >::type >
		{
			inline uint8_t BIT( uint8_t const src, uint8_t uiIndex ) { return ((src >> uiIndex) & 0x01); }

			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				uint32_t * l_pDst = reinterpret_cast< uint32_t * >( &p_pDst[0] );
				l_pDst[0] = 0xFFFFFF00 + ((BIT( *p_pSrc, 0 ) << 0) | (BIT( *p_pSrc, 0 ) << 1) | (BIT( *p_pSrc, 0 ) << 2) | (BIT( *p_pSrc, 0 ) << 3) | (BIT( *p_pSrc, 0 ) << 4) | (BIT( *p_pSrc, 0 ) << 5) | (BIT( *p_pSrc, 0 ) << 6) | (BIT( *p_pSrc, 0 ) << 7));
				l_pDst[1] = 0xFFFFFF00 + ((BIT( *p_pSrc, 1 ) << 0) | (BIT( *p_pSrc, 1 ) << 1) | (BIT( *p_pSrc, 1 ) << 2) | (BIT( *p_pSrc, 1 ) << 3) | (BIT( *p_pSrc, 1 ) << 4) | (BIT( *p_pSrc, 1 ) << 5) | (BIT( *p_pSrc, 1 ) << 6) | (BIT( *p_pSrc, 1 ) << 7));
				l_pDst[2] = 0xFFFFFF00 + ((BIT( *p_pSrc, 2 ) << 0) | (BIT( *p_pSrc, 2 ) << 1) | (BIT( *p_pSrc, 2 ) << 2) | (BIT( *p_pSrc, 2 ) << 3) | (BIT( *p_pSrc, 2 ) << 4) | (BIT( *p_pSrc, 2 ) << 5) | (BIT( *p_pSrc, 2 ) << 6) | (BIT( *p_pSrc, 2 ) << 7));
				l_pDst[3] = 0xFFFFFF00 + ((BIT( *p_pSrc, 3 ) << 0) | (BIT( *p_pSrc, 3 ) << 1) | (BIT( *p_pSrc, 3 ) << 2) | (BIT( *p_pSrc, 3 ) << 3) | (BIT( *p_pSrc, 3 ) << 4) | (BIT( *p_pSrc, 3 ) << 5) | (BIT( *p_pSrc, 3 ) << 6) | (BIT( *p_pSrc, 3 ) << 7));
				l_pDst[4] = 0xFFFFFF00 + ((BIT( *p_pSrc, 4 ) << 0) | (BIT( *p_pSrc, 4 ) << 1) | (BIT( *p_pSrc, 4 ) << 2) | (BIT( *p_pSrc, 4 ) << 3) | (BIT( *p_pSrc, 4 ) << 4) | (BIT( *p_pSrc, 4 ) << 5) | (BIT( *p_pSrc, 4 ) << 6) | (BIT( *p_pSrc, 4 ) << 7));
				l_pDst[5] = 0xFFFFFF00 + ((BIT( *p_pSrc, 5 ) << 0) | (BIT( *p_pSrc, 5 ) << 1) | (BIT( *p_pSrc, 5 ) << 2) | (BIT( *p_pSrc, 5 ) << 3) | (BIT( *p_pSrc, 5 ) << 4) | (BIT( *p_pSrc, 5 ) << 5) | (BIT( *p_pSrc, 5 ) << 6) | (BIT( *p_pSrc, 5 ) << 7));
				l_pDst[6] = 0xFFFFFF00 + ((BIT( *p_pSrc, 6 ) << 0) | (BIT( *p_pSrc, 6 ) << 1) | (BIT( *p_pSrc, 6 ) << 2) | (BIT( *p_pSrc, 6 ) << 3) | (BIT( *p_pSrc, 6 ) << 4) | (BIT( *p_pSrc, 6 ) << 5) | (BIT( *p_pSrc, 6 ) << 6) | (BIT( *p_pSrc, 6 ) << 7));
				l_pDst[7] = 0xFFFFFF00 + ((BIT( *p_pSrc, 7 ) << 0) | (BIT( *p_pSrc, 7 ) << 1) | (BIT( *p_pSrc, 7 ) << 2) | (BIT( *p_pSrc, 7 ) << 3) | (BIT( *p_pSrc, 7 ) << 4) | (BIT( *p_pSrc, 7 ) << 5) | (BIT( *p_pSrc, 7 ) << 6) | (BIT( *p_pSrc, 7 ) << 7));
				p_pSrc += pixel_definitions< PFSrc >::size;
				p_pDst += pixel_definitions< PFDst >::size;
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct buffer_converter
		{
			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				uint8_t const * l_pSrc = &p_pSrcBuffer[0];
				uint8_t * l_pDst = &p_pDstBuffer[0];
				uint32_t l_uiSrcCount = 0;
				uint32_t l_uiCount = p_uiSrcSize / pixel_definitions< PFSrc >::size;

				for( uint32_t i = 0; i < l_uiCount; i++ )
				{
					pixel_converter< PFSrc, PFDst >()( l_pSrc, l_pDst );
				}
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Specialisation for YUY2 to A8R8G8B8 conversion
		\~french
		\brief		Spécialisation pour la conversion YUY2 vers A8R8G8B8
		*/
		template<> struct buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >
		{
			static const __m128i amask;
			static const __m128i umask;
			static const __m128i vmask;
			static const __m128i ymask;
			static const __m128i uvsub;
			static const __m128i ysub;
			static const __m128i facy;
			static const __m128i facrv;
			static const __m128i facgu;
			static const __m128i facgv;
			static const __m128i facbu;
			static const __m128i zero;

			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				__m128i yuv;
				__m128i y;
				__m128i	u;
				__m128i	v;
				__m128i rv0;
				__m128i gu0;
				__m128i gv0;
				__m128i bu0;
				__m128i r0;
				__m128i g0;
				__m128i b0;
				__m128i r1;
				__m128i gb;
				__m128i argb0123;
				__m128i argb4567;

				__m128i const *	l_pBufferIn		= l_pBufferIn = reinterpret_cast< __m128i const * >( p_pSrcBuffer );
				__m128i const *	l_pBufferInEnd	= reinterpret_cast< __m128i const * >( p_pSrcBuffer + p_uiSrcSize );
				__m128i const *	l_pLineIn		= l_pBufferIn;
				__m128i *		l_pBufferOut	= reinterpret_cast< __m128i * >( p_pDstBuffer );

				while( l_pBufferIn != l_pBufferInEnd )
				{
					// On récupère les composantes YUV
					yuv			= _mm_load_si128( l_pLineIn++ );
					// On récupère dans Y les Y uniquement
					y			= _mm_and_si128( yuv, ymask );
					y			= _mm_sub_epi16( y, ysub );
					// dans U les U uniquement
					u			= _mm_and_si128( yuv, umask );
					// que l'on duplique pour en avoir autant que des Y
					u			= _mm_or_si128( _mm_srli_epi32( u, 8 ), _mm_slli_epi32( u, 8 ) );
					u			= _mm_sub_epi16( u, uvsub );
					// dans V les V uniquement
					v			= _mm_and_si128( yuv, vmask );
					// que l'on duplique pour en avoir autant que des Y
					v			= _mm_or_si128( _mm_srli_epi32( v, 8 ), _mm_srli_epi32( v, 24 ) );
					v			= _mm_sub_epi16( v, uvsub );

					// On calcule les facteurs finaux YUV
					y			= _mm_mullo_epi16( facy, y );
					rv0			= _mm_mullo_epi16( facrv, v );
					gu0			= _mm_mullo_epi16( facgu, u );
					gv0			= _mm_mullo_epi16( facgv, v );
					bu0			= _mm_mullo_epi16( facbu, u );

					// Maintenant on calcule les facteurs RGB
					r0			= _mm_srai_epi16( _mm_add_epi16( y, rv0 ), 6 );
					g0			= _mm_srai_epi16( _mm_sub_epi16( y, _mm_add_epi16( gu0, gv0 ) ), 6 );
					b0			= _mm_srai_epi16( _mm_add_epi16( y, bu0 ), 6 );

					// Qu'on sature (clamp entre 0 et 255)
					r0			= _mm_packus_epi16( r0, zero );		// 00000000rrrrrrrr saturated
					g0			= _mm_packus_epi16( g0, zero );		// 00000000gggggggg saturated
					b0			= _mm_packus_epi16( b0, zero );		// 00000000bbbbbbbb saturated

					// Et que l'on ordonne
					r1			= _mm_unpacklo_epi8( zero, b0 );	// b0b0b0b0b0b0b0b0
					gb			= _mm_unpacklo_epi8( g0, r0 );		// rgrgrgrgrgrgrgrg
					argb0123	= _mm_unpacklo_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb4567	= _mm_unpackhi_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb0123	= _mm_or_si128( amask, argb0123 );	// rgbFrgbFrgbFrgbF
					argb4567	= _mm_or_si128( amask, argb4567 );	// rgbFrgbFrgbFrgbF

					// Enfin on fout tout ce bordel dans le buffer
					std::memcpy( l_pBufferOut, &argb0123, sizeof( __m128i ) );	l_pBufferOut++;
					std::memcpy( l_pBufferOut, &argb4567, sizeof( __m128i ) );	l_pBufferOut++;
				}
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Specialisation for YUY2 to R8G8B8 conversion
		\~french
		\brief		Spécialisation pour la conversion YUY2 vers R8G8B8
		*/
		template<> struct buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >
		{
			static const __m128i umask;
			static const __m128i vmask;
			static const __m128i ymask;
			static const __m128i uvsub;
			static const __m128i ysub;
			static const __m128i facy;
			static const __m128i facrv;
			static const __m128i facgu;
			static const __m128i facgv;
			static const __m128i facbu;
			static const __m128i zero;

			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				__m128i yuv;
				__m128i y;
				__m128i	u;
				__m128i	v;
				__m128i rv0;
				__m128i gu0;
				__m128i gv0;
				__m128i bu0;
				__m128i r0;
				__m128i g0;
				__m128i b0;
				__m128i r1;
				__m128i gb;
				__m128i argb0123;
				__m128i argb4567;

				__m128i const *	l_pBufferIn		= l_pBufferIn = reinterpret_cast< __m128i const * >( p_pSrcBuffer );
				__m128i const *	l_pBufferInEnd	= reinterpret_cast< __m128i const * >( p_pSrcBuffer + p_uiSrcSize );
				__m128i const *	l_pLineIn		= l_pBufferIn;
				uint8_t *		l_pBufferOut	= p_pDstBuffer;

				while( l_pBufferIn != l_pBufferInEnd )
				{
					// On récupère les composantes YUV
					yuv			= _mm_load_si128( l_pLineIn++ );
					// On récupère dans Y les Y uniquement
					y			= _mm_and_si128( yuv, ymask );
					y			= _mm_sub_epi16( y, ysub );
					// dans U les U uniquement
					u			= _mm_and_si128( yuv, umask );
					// que l'on duplique pour en avoir autant que des Y
					u			= _mm_or_si128( _mm_srli_epi32( u, 8 ), _mm_slli_epi32( u, 8 ) );
					u			= _mm_sub_epi16( u, uvsub );
					// dans V les V uniquement
					v			= _mm_and_si128( yuv, vmask );
					// que l'on duplique pour en avoir autant que des Y
					v			= _mm_or_si128( _mm_srli_epi32( v, 8 ), _mm_srli_epi32( v, 24 ) );
					v			= _mm_sub_epi16( v, uvsub );

					// On calcule les facteurs finaux YUV
					y			= _mm_mullo_epi16( facy, y );
					rv0			= _mm_mullo_epi16( facrv, v );
					gu0			= _mm_mullo_epi16( facgu, u );
					gv0			= _mm_mullo_epi16( facgv, v );
					bu0			= _mm_mullo_epi16( facbu, u );

					// Maintenant on calcule les facteurs RGB
					r0			= _mm_srai_epi16( _mm_add_epi16( y, rv0 ), 6 );
					g0			= _mm_srai_epi16( _mm_sub_epi16( y, _mm_add_epi16( gu0, gv0 ) ), 6 );
					b0			= _mm_srai_epi16( _mm_add_epi16( y, bu0 ), 6 );

					// Qu'on sature (clamp entre 0 et 255)
					r0			= _mm_packus_epi16( r0, zero );		// 00000000rrrrrrrr saturated
					g0			= _mm_packus_epi16( g0, zero );		// 00000000gggggggg saturated
					b0			= _mm_packus_epi16( b0, zero );		// 00000000bbbbbbbb saturated

					// Et que l'on ordonne
					r1			= _mm_unpacklo_epi8( zero, b0 );	// b0b0b0b0b0b0b0b0
					gb			= _mm_unpacklo_epi8( g0, r0 );		// rgrgrgrgrgrgrgrg
					argb0123	= _mm_unpacklo_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb4567	= _mm_unpackhi_epi16( r1, gb );		// rgb0rgb0rgb0rgb0

					// Enfin on fout tout ce bordel dans le buffer
					argb0123 = _mm_srli_si128( argb0123, 1 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );	l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );	l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );	l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );	l_pBufferOut += 3;

					argb4567 = _mm_srli_si128( argb4567, 1 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );	l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );	l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );	l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );	l_pBufferOut += 3;
				}
			}
		};
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for colour pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel couleurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicColourConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_L8:			pixel_converter< PF,	ePIXEL_FORMAT_L8			>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_L16F32F:		pixel_converter< PF,	ePIXEL_FORMAT_L16F32F		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_L32F:		pixel_converter< PF,	ePIXEL_FORMAT_L32F			>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_A8L8:		pixel_converter< PF,	ePIXEL_FORMAT_A8L8			>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_AL16F32F:	pixel_converter< PF,	ePIXEL_FORMAT_AL16F32F		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_AL32F:		pixel_converter< PF,	ePIXEL_FORMAT_AL32F			>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_A1R5G5B5:	pixel_converter< PF,	ePIXEL_FORMAT_A1R5G5B5		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_A4R4G4B4:	pixel_converter< PF,	ePIXEL_FORMAT_A4R4G4B4		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_R5G6B5:		pixel_converter< PF,	ePIXEL_FORMAT_R5G6B5		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_R8G8B8:		pixel_converter< PF,	ePIXEL_FORMAT_R8G8B8		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_A8R8G8B8:	pixel_converter< PF,	ePIXEL_FORMAT_A8R8G8B8		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_RGB16F32F:	pixel_converter< PF,	ePIXEL_FORMAT_RGB16F32F		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_ARGB16F32F:	pixel_converter< PF,	ePIXEL_FORMAT_ARGB16F32F	>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_RGB32F:		pixel_converter< PF,	ePIXEL_FORMAT_RGB32F		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_ARGB32F:		pixel_converter< PF,	ePIXEL_FORMAT_ARGB32F		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_YUY2:		pixel_converter< PF,	ePIXEL_FORMAT_YUY2			>()( p_pSrc, p_pDst );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );								break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for depth pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel profondeurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_DEPTH16:		pixel_converter< PF,	ePIXEL_FORMAT_DEPTH16		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_DEPTH24:		pixel_converter< PF,	ePIXEL_FORMAT_DEPTH24		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_DEPTH24S8:	pixel_converter< PF,	ePIXEL_FORMAT_DEPTH24S8		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_DEPTH32:		pixel_converter< PF,	ePIXEL_FORMAT_DEPTH32		>()( p_pSrc, p_pDst );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );								break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for stencil pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel stencil, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_DEPTH24S8:	pixel_converter< PF,	ePIXEL_FORMAT_DEPTH24S8		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_STENCIL1:	pixel_converter< PF,	ePIXEL_FORMAT_STENCIL1		>()( p_pSrc, p_pDst );	break;
			case ePIXEL_FORMAT_STENCIL8:	pixel_converter< PF,	ePIXEL_FORMAT_STENCIL8		>()( p_pSrc, p_pDst );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );		      					break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicColourBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch( p_eDstFormat )
			{
			case ePIXEL_FORMAT_L8:			buffer_converter< PF,	ePIXEL_FORMAT_L8			>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_L16F32F:		buffer_converter< PF,	ePIXEL_FORMAT_L16F32F		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_L32F:		buffer_converter< PF,	ePIXEL_FORMAT_L32F			>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_A8L8:		buffer_converter< PF,	ePIXEL_FORMAT_A8L8			>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_AL16F32F:	buffer_converter< PF,	ePIXEL_FORMAT_AL16F32F		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_AL32F:		buffer_converter< PF,	ePIXEL_FORMAT_AL32F			>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_A1R5G5B5:	buffer_converter< PF,	ePIXEL_FORMAT_A1R5G5B5		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_A4R4G4B4:	buffer_converter< PF,	ePIXEL_FORMAT_A4R4G4B4		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_R5G6B5:		buffer_converter< PF,	ePIXEL_FORMAT_R5G6B5		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_R8G8B8:		buffer_converter< PF,	ePIXEL_FORMAT_R8G8B8		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_A8R8G8B8:	buffer_converter< PF,	ePIXEL_FORMAT_A8R8G8B8		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_RGB16F32F:	buffer_converter< PF,	ePIXEL_FORMAT_RGB16F32F		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_ARGB16F32F:	buffer_converter< PF,	ePIXEL_FORMAT_ARGB16F32F	>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_RGB32F:		buffer_converter< PF,	ePIXEL_FORMAT_RGB32F		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_ARGB32F:		buffer_converter< PF,	ePIXEL_FORMAT_ARGB32F		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_YUY2:		buffer_converter< PF,	ePIXEL_FORMAT_YUY2			>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );																		break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch( p_eDstFormat )
			{
			case ePIXEL_FORMAT_DEPTH16:		buffer_converter< PF,	ePIXEL_FORMAT_DEPTH16		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_DEPTH24:		buffer_converter< PF,	ePIXEL_FORMAT_DEPTH24		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_DEPTH24S8:	buffer_converter< PF,	ePIXEL_FORMAT_DEPTH24S8		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_DEPTH32:		buffer_converter< PF,	ePIXEL_FORMAT_DEPTH32		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );																		break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch( p_eDstFormat )
			{
			case ePIXEL_FORMAT_DEPTH24S8:	buffer_converter< PF,	ePIXEL_FORMAT_DEPTH24S8		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_STENCIL1:	buffer_converter< PF,	ePIXEL_FORMAT_STENCIL1		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			case ePIXEL_FORMAT_STENCIL8:	buffer_converter< PF,	ePIXEL_FORMAT_STENCIL8		>()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );	break;
			default:						UNSUPPORTED_ERROR( "No conversion defined" );																		break;
			}
		}
	}

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_L8	\~french	Spécialisation pour ePIXEL_FORMAT_L8
	template <> struct pixel_definitions< ePIXEL_FORMAT_L8 >
	{
		enum eSIZE : uint8_t { size = 1 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "8 bits luminosity" ); }
		static inline String to_str() { return cuT( "l8" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_L8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_L8, PF >()( p_pSrc, p_pDst); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_L16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_L16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_L16F32F >
	{
		enum eSIZE : uint8_t { size = 4 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "Half floats luminosity" ); }
		static inline String to_str() { return cuT( "l16f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_L16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_L16F32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_L32F	\~french	Spécialisation pour ePIXEL_FORMAT_L32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_L32F >
	{
		enum eSIZE : uint8_t { size = 4 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "Floats luminosity" ); }
		static inline String to_str() { return cuT( "l32f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_L32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_L32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_A8L8	\~french	Spécialisation pour ePIXEL_FORMAT_A8L8
	template <> struct pixel_definitions< ePIXEL_FORMAT_A8L8 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L8;
		static inline String to_string() { return cuT( "16 bits luminosity and alpha" ); }
		static inline String to_str() { return cuT( "al16" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_A8L8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A8L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_A8L8, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_AL32F	\~french	Spécialisation pour ePIXEL_FORMAT_AL32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_AL32F >
	{
		enum eSIZE : uint8_t { size = 8 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L32F;
		static inline String to_string() { return cuT( "Floats luminosity and alpha" ); }
		static inline String to_str() { return cuT( "al32f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_AL32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_AL32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_AL32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_AL16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_AL16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_AL16F32F >
	{
		enum eSIZE : uint8_t { size = 8 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L16F32F;
		static inline String to_string() { return cuT( "Half floats luminosity and alpha" ); }
		static inline String to_str() { return cuT( "al16f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_AL16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_AL16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_AL16F32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_A1R5G5B5	\~french	Spécialisation pour ePIXEL_FORMAT_A1R5G5B5
	template <> struct pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R5G6B5;
		static inline String to_string() { return cuT( "16 bits 5551 ARGB" ); }
		static inline String to_str() { return cuT( "argb1555" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_A1R5G5B5 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A1R5G5B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_A1R5G5B5, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_R5G6B5	\~french	Spécialisation pour ePIXEL_FORMAT_R5G6B5
	template <> struct pixel_definitions< ePIXEL_FORMAT_R5G6B5 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "16 bits 565 RGB" ); }
		static inline String to_str() { return cuT( "rgb565" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_R5G6B5 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_R5G6B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_R5G6B5, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_A4R4G4B4	\~french	Spécialisation pour ePIXEL_FORMAT_A4R4G4B4
	template <> struct pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R5G6B5;
		static inline String to_string() { return cuT( "16 bits 4444 ARGB" ); }
		static inline String to_str() { return cuT( "argb16" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_A4R4G4B4 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A4R4G4B4 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_A4R4G4B4, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_R8G8B8	\~french	Spécialisation pour ePIXEL_FORMAT_R8G8B8
	template <> struct pixel_definitions< ePIXEL_FORMAT_R8G8B8 >
	{
		enum eSIZE : uint8_t { size = 3 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "24 bits 888 RGB" ); }
		static inline String to_str() { return cuT( "rgb24" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_R8G8B8, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_A8R8G8B8	\~french	Spécialisation pour ePIXEL_FORMAT_A8R8G8B8
	template <> struct pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >
	{
		enum eSIZE : uint8_t { size = 4 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R8G8B8;
		static inline String to_string() { return cuT( "32 bits 8888 ARGB" ); }
		static inline String to_str() { return cuT( "argb32" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_A8R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A8R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_A8R8G8B8, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_RGB16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_RGB16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_RGB16F32F >
	{
		enum eSIZE : uint8_t { size = 12 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "Half floating point RGB" ); }
		static inline String to_str() { return cuT( "rgb16f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_RGB32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_ARGB16F32F	\~french	Spécialisation pour ePIXEL_FORMAT_ARGB16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >
	{
		enum eSIZE : uint8_t { size = 16 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_RGB16F32F;
		static inline String to_string() { return cuT( "Half floating point ARGB" ); }
		static inline String to_str() { return cuT( "argb16f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_ARGB32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_RGB32F	\~french	Spécialisation pour ePIXEL_FORMAT_RGB32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_RGB32F >
	{
		enum eSIZE : uint8_t { size = 12 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "Floating point RGB" ); }
		static inline String to_str() { return cuT( "rgb32f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_RGB32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_ARGB32F	\~french	Spécialisation pour ePIXEL_FORMAT_ARGB32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_ARGB32F >
	{
		enum eSIZE : uint8_t { size = 16 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_RGB32F;
		static inline String to_string() { return cuT( "Floating point ARGB" ); }
		static inline String to_str() { return cuT( "argb32f" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_ARGB32F, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DXTC1	\~french	Spécialisation pour ePIXEL_FORMAT_DXTC1
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC1 >
	{
		enum eSIZE : uint8_t { size = 1 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static inline String to_string() { return cuT( "DXT1 8 bits compressed format" ); }
		static inline String to_str() { return cuT( "dxtc1" ); }
		static inline void convert( uint8_t const * , uint8_t * , ePIXEL_FORMAT ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DXTC1, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DXTC3	\~french	Spécialisation pour ePIXEL_FORMAT_DXTC3
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC3 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_DXTC3;
		static inline String to_string() { return cuT( "DXT3 16 bits compressed format" ); }
		static inline String to_str() { return cuT( "dxtc3" ); }
		static inline void convert( uint8_t const * , uint8_t * , ePIXEL_FORMAT ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DXTC3, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DXTC5	\~french	Spécialisation pour ePIXEL_FORMAT_DXTC5
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC5 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_DXTC5;
		static inline String to_string() { return cuT( "DXT5 16 bits compressed format" ); }
		static inline String to_str() { return cuT( "dxtc5" ); }
		static inline void convert( uint8_t const * , uint8_t * , ePIXEL_FORMAT ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DXTC5, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_YUY2	\~french	Spécialisation pour ePIXEL_FORMAT_YUY2
	template <> struct pixel_definitions< ePIXEL_FORMAT_YUY2 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static inline String to_string() { return cuT( "YUY2(4:2:2) 16 bits compressed format" ); }
		static inline String to_str() { return cuT( "yuy2" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicColourConversion< ePIXEL_FORMAT_YUY2 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicColourBufferConversion< ePIXEL_FORMAT_YUY2 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_YUY2, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH16	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH16
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH16 >
	{
		enum eSIZE : uint8_t { size = 2 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "16 bits depth" ); }
		static inline String to_str() { return cuT( "depth16" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH16 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH16 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DEPTH16, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH24	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH24
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH24 >
	{
		enum eSIZE : uint8_t { size = 3 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "24 bits depth" ); }
		static inline String to_str() { return cuT( "depth24" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH24 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH24 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DEPTH24, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH24S8	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH24S8
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >
	{
		enum eSIZE : uint8_t { size = 4 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "24 bits depth, 8 bits stencil" ); }
		static inline String to_str() { return cuT( "depth24s8" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH24S8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH24S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter<ePIXEL_FORMAT_DEPTH24S8, PF>()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_DEPTH32	\~french	Spécialisation pour ePIXEL_FORMAT_DEPTH32
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH32 >
	{
		enum eSIZE : uint8_t { size = 4 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "32 bits depth" ); }
		static inline String to_str() { return cuT( "depth32" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH32 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH32 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_DEPTH32, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_STENCIL1	\~french	Spécialisation pour ePIXEL_FORMAT_STENCIL1
	template <> struct pixel_definitions< ePIXEL_FORMAT_STENCIL1 >
	{
		enum eSIZE : uint8_t { size = 1 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = false;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "1 bit stencil" ); }
		static inline String to_str() { return cuT( "stencil1" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicStencilConversion< ePIXEL_FORMAT_STENCIL1 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicStencilBufferConversion< ePIXEL_FORMAT_STENCIL1 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_STENCIL1, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	//!\~english	Specialisation for ePIXEL_FORMAT_STENCIL8	\~french	Spécialisation pour ePIXEL_FORMAT_STENCIL8
	template <> struct pixel_definitions< ePIXEL_FORMAT_STENCIL8 >
	{
		enum eSIZE : uint8_t { size = 1 };
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = false;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string() { return cuT( "8 bits stencil" ); }
		static inline String to_str() { return cuT( "stencil8" ); }
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst ) { detail::DynamicStencilConversion< ePIXEL_FORMAT_STENCIL8 >( p_pSrc, p_pDst, p_ePixelFmtDst ); }
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize ) { detail::DynamicStencilBufferConversion< ePIXEL_FORMAT_STENCIL8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize ); }
		template< ePIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst ) { detail::pixel_converter< ePIXEL_FORMAT_STENCIL8, PF >()( p_pSrc, p_pDst ); }
	};

	//*************************************************************************************************

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> struct PxOperators
	{
		static void Add( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			PxOperators<FT, FT>::Add( p_pxA, l_pxB);
		}

		static void Substract( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			PxOperators<FT, FT>::Substract( p_pxA, l_pxB);
		}

		static void Multiply( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			PxOperators<FT, FT>::Multiply( p_pxA, l_pxB);
		}

		static void Divide( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			PxOperators<FT, FT>::Divide( p_pxA, l_pxB);
		}

		static void Assign( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			PxOperators<FT, FT>::Assign( p_pxA, l_pxB);
		}

		static bool Equals( Pixel< FT > const & p_pxA, Pixel< FU > const & p_pxB)
		{
			Pixel< FT > & l_pxB( p_pxB);
			return PxOperators<FT, FT>::Equals( p_pxA, l_pxB);
		}
	};

	template< ePIXEL_FORMAT FT > struct PxOperators<FT, FT>
	{
		static void Add( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				p_pxA[i] += p_pxB[i];
			}
		}

		static void Substract( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				p_pxA[i] -= p_pxB[i];
			}
		}
		static void Multiply( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				p_pxA[i] *= p_pxB[i];
			}
		}

		static void Divide( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				p_pxA[i] = (p_pxB[i] == 0 ? p_pxA[i] : p_pxA[i] / p_pxB[i]);
			}
		}

		static void Assign( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				p_pxA[i] = p_pxB[i];
			}
		}

		static bool Equals( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB)
		{
			bool l_bReturn = true;

			for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
			{
				l_bReturn = p_pxA[i] == p_pxB[i];
			}

			return l_bReturn;
		}
	};

	//*************************************************************************************************

	template< ePIXEL_FORMAT FT >
	Pixel< FT > :: Pixel( bool p_bInit )
	{
		if( p_bInit )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
			std::memset( m_pComponents.get(), 0, pixel_definitions< FT >::size );
		}
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > :: Pixel( uint8_t * p_components )
		:	m_pComponents	( p_components, g_dummyDtor )
	{
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > :: Pixel( array< uint8_t, pixel_definitions< FU >::size > const & p_components )
		:	m_pComponents	( new uint8_t[pixel_definitions< FT >::size] )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > :: Pixel( Pixel< FT > const & p_pxl )
	{
		if( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
			std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::size );
		}
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > :: Pixel( Pixel && p_pxl )
	{
		m_pComponents = std::move( p_pxl.m_pComponents	);
		p_pxl.m_pComponents.reset();
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > :: Pixel( Pixel< FU > const & p_pxl )
	{
		if( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
			uint8_t const * l_pSrc = p_pxl.const_ptr();
			uint8_t * l_pDst = m_pComponents.get();
			pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
		}
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > :: ~Pixel()
	{
		clear();
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > & Pixel< FT > :: operator =( Pixel< FT > const & p_pxl )
	{
		if( m_pComponents.get() )
		{
			if( p_pxl.const_ptr() )
			{
				std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::size );
			}
			else
			{
				clear();
			}
		}
		else if( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
			std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::size );
		}

		return * this;
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > & Pixel< FT > :: operator =( Pixel< FT > && p_pxl )
	{
		if( this != &p_pxl )
		{
			clear();
			m_pComponents = std::move( p_pxl.m_pComponents	);
			p_pxl.m_pComponents.reset();
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT > :: operator =( Pixel< FU > const & p_pxl )
	{
		if( m_pComponents.get() )
		{
			if( p_pxl.const_ptr() )
			{
				uint8_t const * l_pSrc = p_pxl.const_ptr();
				uint8_t * l_pDst = m_pComponents.get();
				pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
			}
			else
			{
				clear();
			}
		}
		else if( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
			uint8_t const * l_pSrc = p_pxl.const_ptr();
			uint8_t * l_pDst = m_pComponents.get();
			pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT > :: operator +=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::Add( *this, p_px );
		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT > :: operator +=( U const & p_t )
	{
		for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i]) + p_t);
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT > :: operator -=( Pixel< FU > const & p_px )
	{
		PxOperators<FT, FU>::Substract( * this, p_px);
		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT > :: operator -=( U const & p_t )
	{
		for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i]) - p_t);
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT > :: operator /=( Pixel< FU > const & p_px )
	{
		PxOperators<FT, FU>::Divide( * this, p_px);
		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT > :: operator /=( U const & p_t )
	{
		for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
		{
			m_pComponents.get()[i] = (p_t == 0 ? m_pComponents.get()[i] : T( U( m_pComponents.get()[i]) + p_t));
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT > :: operator *=( Pixel< FU > const & p_px )
	{
		PxOperators<FT, FU>::Multiply( * this, p_px);
		return *this;
	}

	template< ePIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT > :: operator *=( U const & p_t )
	{
		for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i]) * p_t);
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	Pixel< FT > & Pixel< FT > :: operator >>( uint8_t p_t )
	{
		for (uint8_t i = 0; i < pixel_definitions< FT >::size; i++)
		{
			m_pComponents.get()[i] = m_pComponents.get()[i] >> p_t;
		}

		return *this;
	}

	template< ePIXEL_FORMAT FT >
	void Pixel< FT > :: clear()
	{
		m_pComponents.reset();
	}

	template< ePIXEL_FORMAT FT >
	void Pixel< FT > :: link( uint8_t * p_pComponents )
	{
		clear();
		m_pComponents = std::shared_ptr< uint8_t >( p_pComponents, g_dummyDtor );
	}

	template< ePIXEL_FORMAT FT >
	void Pixel< FT > :: unlink()
	{
		m_pComponents.reset( new uint8_t[pixel_definitions< FT >::size] );
		std::memset( m_pComponents.get(), 0, pixel_definitions< FT >::size );
	}

	template< ePIXEL_FORMAT FT >
	template< typename U >
	void Pixel< FT > :: sum( U & p_uResult )const
	{
		U l_base = 0;
		p_uResult = std::accumulate( begin(), end(), l_base );
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FU > Pixel< FT > :: mul( Pixel< FU > const & p_px )const
	{
		Pixel< FU > l_pxReturn( * this);
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	Pixel< FU > Pixel< FT > :: mul( uint8_t const & p_val )const
	{
		Pixel< FU > l_pxReturn( * this);
		l_pxReturn *= p_val;
		return l_pxReturn;
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	void Pixel< FT > :: set( array< uint8_t, pixel_definitions< FU >::size > const & p_components )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	void Pixel< FT > :: set( uint8_t const * p_components )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< ePIXEL_FORMAT FT >
	template< ePIXEL_FORMAT FU >
	void Pixel< FT > :: set( Pixel< FU > const & p_px )
	{
		uint8_t const * l_pSrc = p_px.const_ptr();
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

//*************************************************************************************************

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	bool operator ==( Pixel< FT > const & p_pixel, Pixel< FU > const & p_pxl)
	{
		return PxOperators<FT, FU>::Equals( p_pixel, p_pxl);
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel< FT > operator +( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn += p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel< FT > operator +( Pixel< FT > const & p_pixel, U const & p_t)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn += p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel< FT > operator -( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn -= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel< FT > operator -( Pixel< FT > const & p_pixel, U const & p_t)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn -= p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel< FT > operator /( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn /= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel< FT > operator /( Pixel< FT > const & p_pixel, U const & p_t)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn /= p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel< FT > operator *( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel< FT > operator *( Pixel< FT > const & p_pixel, U const & p_t)
	{
		Pixel< FT > l_pxReturn( p_pixel);
		l_pxReturn *= p_t;
		return l_pxReturn;
	}

	//*************************************************************************************************

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > :: PxBuffer( Size const & p_size, uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
		:	PxBufferBase	( p_size, FT, p_pBuffer, p_eBufferFormat	)
		,	m_column		( p_size.width()							)
	{
		init( p_pBuffer, p_eBufferFormat );
	}

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > :: PxBuffer( PxBuffer< FT > const & p_pixelBuffer )
		:	PxBufferBase	( p_pixelBuffer.dimensions(), FT	)
		,	m_column		( p_pixelBuffer.width()				)
	{
		init( p_pixelBuffer.begin(), p_pixelBuffer.format() );
	}

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > :: PxBuffer( PxBuffer< FT > && p_pixelBuffer )
		:	PxBufferBase	( std::move( p_pixelBuffer			)	)
		,	m_column		( std::move( p_pixelBuffer.m_column	)	)
	{
		p_pixelBuffer.m_column.clear();
	}

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > :: ~PxBuffer()
	{
		clear();
	}

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > & PxBuffer< FT > :: operator =( PxBuffer< FT > const & p_pixelBuffer)
	{
		PxBufferBase::operator=( p_pixelBuffer);
		return * this;
	}

	template< ePIXEL_FORMAT FT >
	PxBuffer< FT > & PxBuffer< FT > :: operator =( PxBuffer< FT > && p_pixelBuffer)
	{
		PxBufferBase::operator=( p_pixelBuffer);
		m_column = std::move( p_pixelBuffer.m_column );
		p_pixelBuffer.m_column.clear();
		return * this;
	}

	template< ePIXEL_FORMAT FT >
	void PxBuffer< FT > :: swap( PxBuffer< FT > & p_pixelBuffer)
	{
		PxBufferBase::swap( p_pixelBuffer );
	}

	template< ePIXEL_FORMAT FT >
	void PxBuffer< FT > :: assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		uint8_t l_uiSize = PF::GetBytesPerPixel( p_eBufferFormat );
		uint32_t l_uiDstMax = count();
		uint32_t l_uiSrcMax = uint32_t( p_pBuffer.size() / l_uiSize );

		if( p_pBuffer.size() > 0 && l_uiSrcMax == l_uiDstMax )
		{
			PF::ConvertBuffer( p_eBufferFormat, p_pBuffer.data(), uint32_t( p_pBuffer.size() ), format(), m_pBuffer, size() );
		}
	}

	template< ePIXEL_FORMAT FT >
	std::shared_ptr< PxBufferBase > PxBuffer< FT > :: clone()const
	{
		return std::make_shared< PxBuffer >( *this );
	}

	template< ePIXEL_FORMAT FT >
	uint8_t * PxBuffer< FT > :: get_at( uint32_t x, uint32_t y)
	{
		CASTOR_ASSERT( x < width() && y < height());
		return &m_pBuffer[(x * height() + y) * pixel_definitions< FT >::size];
	}

	template< ePIXEL_FORMAT FT >
	uint8_t const * PxBuffer< FT > :: get_at( uint32_t x, uint32_t y)const
	{
		CASTOR_ASSERT( x < width() && y < height());
		return &m_pBuffer[(x * height() + y) * pixel_definitions< FT >::size];
	}

	template< ePIXEL_FORMAT FT >
	void PxBuffer< FT > :: flip()
	{
		uint32_t 	l_uiWidth		= width() * pixel_definitions< FT >::size;
		uint32_t	l_uiHeight		= height();
		uint8_t * 	l_pBufferTop	= &m_pBuffer[0];
		uint8_t * 	l_pBufferBottom	= &m_pBuffer[(l_uiHeight - 1) * l_uiWidth - 1];

		for( uint32_t i = 0; i < l_uiHeight / 2; i++ )
		{
			for( uint32_t j = 0; j < l_uiWidth; j++ )
			{
				std::swap( l_pBufferTop[j], l_pBufferBottom[j] );
			}

			l_pBufferTop	+= l_uiWidth;
			l_pBufferBottom	-= l_uiWidth;
		}
	}

	template< ePIXEL_FORMAT FT >
	void PxBuffer< FT > :: mirror()
	{
		uint32_t 	l_uiWidth		= width() * pixel_definitions< FT >::size;
		uint32_t	l_uiHeight		= height();
		uint8_t			*l_pxA, *l_pxB;

		for( uint32_t i = 0; i < l_uiHeight; i++ )
		{
			l_pxA = &m_pBuffer[i * l_uiWidth];
			l_pxB = &m_pBuffer[(i + 1) * l_uiWidth - 1];

			for( uint32_t j = 0; j < l_uiWidth / 2; j += pixel_definitions< FT >::size )
			{
				for( uint32_t k = 0; k < pixel_definitions< FT >::size; k++)
				{
					std::swap( l_pxA[k], l_pxB[k] );
				}

				l_pxA += pixel_definitions< FT >::size;
				l_pxB -= pixel_definitions< FT >::size;
			}
		}
	}

	template< ePIXEL_FORMAT FT >
	void PxBuffer< FT > :: do_init_column( uint32_t p_uiColumn )
	{
		if( p_uiColumn < width() )
		{
			uint8_t * l_pBuffer = &m_pBuffer[p_uiColumn * width() * pixel_definitions< FT >::size];
			m_column = column( height() );

			for (uint32_t j = 0; j < height(); j++)
			{
				m_column[j].link( l_pBuffer );
				l_pBuffer += pixel_definitions< FT >::size;
			}
		}
	}

	//*************************************************************************************************
}
