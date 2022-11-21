/*
See LICENSE file in root folder
*/
#ifndef ___CU_RgbaColour_H___
#define ___CU_RgbaColour_H___

#include "CastorUtils/Data/TextLoader.hpp"
#include "CastorUtils/Data/TextWriter.hpp"
#include "CastorUtils/Graphics/ColourComponent.hpp"
#include "CastorUtils/Graphics/HdrColourComponent.hpp"
#include "CastorUtils/Math/Point.hpp"

#undef RGB

namespace castor
{
	/**
	 *\~english
	 *\brief		Retrieves predefined colour name
	 *\param[in]	predefined	The predefined colour
	 *\return		The colour name
	 *\~french
	 *\brief		Récupère le nom d'une couleur prédéfinie
	 *\param[in]	predefined	La couleur prédéfinie
	 *\return		Le nom de la couleur
	 */
	inline static String getPredefinedName( PredefinedRgbaColour predefined );
	/**
	 *\~english
	 *\brief		Retrieves predefined colour from a name
	 *\param[in]	name	The predefined colour name
	 *\return		The predefined colour
	 *\~french
	 *\brief		Récupère une couleur prédéfinie à partir de son nom
	 *\param[in]	name	Le nom de la couleur prédéfinie
	 *\return		La couleur prédéfinie
	 */
	inline static PredefinedRgbaColour getPredefinedRgba( String const & name );

	template< typename ComponentType >
	class RgbaColourT
	{
	private:
		CU_DeclareArray( float, RgbaComponent::eCount, Float4 );
		using ColourComponentArray = std::array< ComponentType, size_t( RgbaComponent::eCount ) >;
		using ColourComponentArrayIt = typename ColourComponentArray::iterator;
		using ColourComponentArrayRIt = typename ColourComponentArray::reverse_iterator;
		using ColourComponentArrayConstIt = typename ColourComponentArray::const_iterator;
		using ColourComponentArrayConstRIt = typename ColourComponentArray::const_reverse_iterator;

	public:
		RgbaColourT() = default;
		RgbaColourT( RgbaColourT const & rhs ) = default;
		RgbaColourT & operator=( RgbaColourT const & rhs ) = default;
		RgbaColourT( RgbaColourT && rhs ) = default;
		RgbaColourT & operator=( RgbaColourT && rhs ) = default;
		~RgbaColourT() = default;
		/**
		 *\~english
		 *\brief		Constructor from other component type
		 *\param[in]	rhs		The colour value.
		 *\param[in]	gamma	The gamma correction value.
		 *\~french
		 *\brief		Constructeur depuis un autre type de composante.
		 *\param[in]	rhs		La valeur de la couleur.
		 *\param[in]	gamma	La valeur de la correction gamma.
		 */
		template< typename ComponentU >
		explicit RgbaColourT( RgbaColourT< ComponentU > const & rhs
			, float gamma = 2.2f );
		/**
		 *\~english
		 *\brief		Specified Constructor
		 *\~french
		 *\brief		Constructeur spécifié.
		 */
		inline RgbaColourT( float r, float g, float b, float a );
		/**
		 *\~english
		 *\brief		Constructor from components
		 *\param[in]	r, g, b, a	The colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir des composantes
		 *\param[in]	r, g, b, a	Les composantes de la couleur
		 *\return		La RgbaColourT construite
		 */
		template< typename T1, typename T2, typename T3, typename T4 >
		static RgbaColourT fromComponents( T1 const & r, T2 const & g, T3 const & b, T4 const & a )
		{
			RgbaColourT clrReturn;
			clrReturn.m_components[size_t( RgbaComponent::eRed )] = r;
			clrReturn.m_components[size_t( RgbaComponent::eGreen )] = g;
			clrReturn.m_components[size_t( RgbaComponent::eBlue )] = b;
			clrReturn.m_components[size_t( RgbaComponent::eAlpha )] = a;
			return clrReturn;
		}
		/**
		 *\~english
		 *\brief		Constructor from a predefined colour
		 *\param[in]	predefined
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'une couleur prédéfinie
		 *\param[in]	predefined
		 *\return		La RgbaColourT construite
		 */
		static RgbaColourT fromPredefined( PredefinedRgbaColour predefined )
		{
			return fromRGBA( uint32_t( predefined ) );
		}
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGB( Point3ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGR( Point3ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGBA( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGRA( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromARGB( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromABGR( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGB( Point3f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGR( Point3f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGBA( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromARGB( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromABGR( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGRA( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGB( uint8_t const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGR( uint8_t const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGBA( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGRA( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromARGB( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromABGR( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGB( float const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGR( float const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGBA( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromARGB( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromABGR( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGRA( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGB components
		 *\param[in]	colour	The uint32_t containing the colour (0x00RRGGBB)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGB
		 *\param[in]	colour	L'uint32_t contenant la couleur (0x00RRGGBB)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGB( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGR components
		 *\param[in]	colour	The uint32_t containing the colour (0x00BBGGRR)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGR
		 *\param[in]	colour	L'uint32_t contenant la couleur (0x00BBGGRR)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGR( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ARGB components
		 *\param[in]	colour	The uint32_t containing the colour (0xAARRGGBB)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ARGB
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xAARRGGBB)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromARGB( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGBA components
		 *\param[in]	colour	The uint32_t containing the colour (0xRRGGBBAA)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGBA
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xRRGGBBAA)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromRGBA( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ABGR components
		 *\param[in]	colour	The uint32_t containing the colour (0xAABBGGRR)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ABGR
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xAABBGGRR)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromABGR( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGRA components
		 *\param[in]	colour	The uint32_t containing the colour (0xBBGGRRAA)
		 *\return		The built RgbaColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGRA
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xBBGGRRAA)
		 *\return		La RgbaColourT construite
		 */
		inline static RgbaColourT fromBGRA( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an HSB components.
		 *\param[in]	hue, saturation, brightness	The HSB components.
		 *\return		The built RgbaColourT.
		 *\~french
		 *\brief		Constructeur à partir de composantes HSB
		 *\param[in]	hue, saturation, brightness	Les composantes HSB.
		 *\return		La RgbaColourT construite.
		 */
		inline static RgbaColourT fromHSB( float hue, float saturation, float brightness );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt begin()
		{
			return m_components.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt begin()const
		{
			return m_components.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt end()
		{
			return m_components.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt end()const
		{
			return m_components.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float const * constPtr()const
		{
			return &m_components[0].value();
		}
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float * ptr()
		{
			return &m_components[0].value();
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The components value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType & operator[]( RgbaComponent component )
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The components value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & operator[]( RgbaComponent component )const
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The components value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType & operator[]( size_t component )
		{
			return m_components[component];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The components value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & operator[]( size_t component )const
		{
			return m_components[component];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType & get( RgbaComponent component )
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	component	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & get( RgbaComponent component )const
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ComponentType & red()
		{
			return get( RgbaComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & red()const
		{
			return get( RgbaComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ComponentType & green()
		{
			return get( RgbaComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & green()const
		{
			return get( RgbaComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ComponentType & blue()
		{
			return get( RgbaComponent::eBlue );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & blue()const
		{
			return get( RgbaComponent::eBlue );
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ComponentType & alpha()
		{
			return get( RgbaComponent::eAlpha );
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & alpha()const
		{
			return get( RgbaComponent::eAlpha );
		}
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	rhs	The colours to add
		 *\return		Result of this + rhs
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	rhs	Les couleurs à ajouter
		 *\return		Resultat de this + rhs
		 */
		inline RgbaColourT & operator+=( RgbaColourT const & rhs );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	rhs	The colours to subtract
		 *\return		Result of this - rhs
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	rhs	Les couleurs à soustraire
		 *\return		Resultat de this - rhs
		 */
		inline RgbaColourT & operator-=( RgbaColourT const & rhs );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	scalar	The value to add
		 *\return		Result of this + scalar
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	scalar	La valeur à ajouter
		 *\return		Resultat de this + scalar
		 */
		template< typename T >
		RgbaColourT & operator+=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				m_components[i] += scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	scalar	The value to subtract
		 *\return		Result of this - scalar
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	scalar	La valeur à soustraire
		 *\return		Resultat de this - scalar
		 */
		template< typename T >
		RgbaColourT & operator-=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				m_components[i] -= scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	scalar	The value to multiply
		 *\return		Result of this * scalar
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	scalar	La valeur à multiplier
		 *\return		Resultat de this * scalar
		 */
		template< typename T >
		RgbaColourT & operator*=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				m_components[i] *= scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	scalar	The value to divide
		 *\return		Result of this / scalar
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	scalar	La valeur à diviser
		 *\return		Resultat de this / scalar
		 */
		template< typename T >
		RgbaColourT & operator/=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
			{
				m_components[i] /= scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	component	The value to add
		 *\return		Result of this + component
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	component	La valeur à ajouter
		 *\return		Resultat de this + component
		 */
		inline RgbaColourT & operator+=( ComponentType const & component );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	component	The value to subtract
		 *\return		Result of this - component
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	component	La valeur à soustraire
		 *\return		Resultat de this - component
		 */
		inline RgbaColourT & operator-=( ComponentType const & component );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	component	The value to multiply
		 *\return		Result of this * component
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	component	La valeur à multiplier
		 *\return		Resultat de this * component
		 */
		inline RgbaColourT & operator*=( ComponentType const & component );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	component	The value to divide
		 *\return		Result of this / component
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	component	La valeur à diviser
		 *\return		Resultat de this / component
		 */
		inline RgbaColourT & operator/=( ComponentType const & component );

	private:
		ColourComponentArray m_components;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	lhs, rhs	The colours to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	lhs, rhs	Les couleurs à comparer
	 */
	template< typename ComponentType >
	inline bool operator==( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	lhs, rhs	The colours to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	lhs, rhs	Les couleurs à comparer
	 */
	template< typename ComponentType >
	inline bool operator!=( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	lhs, rhs	The colours to add
	 *\return		Result of lhs + rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	lhs, rhs	Les couleurs à ajouter
	 *\return		Resultat de lhs + rhs
	 */
	template< typename ComponentType >
	inline RgbaColourT< ComponentType > operator+( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	lhs, rhs	The colours to subtract
	 *\return		Result of lhs - rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	lhs, rhs	Les couleurs à soustraire
	 *\return		Resultat de lhs - rhs
	 */
	template< typename ComponentType >
	inline RgbaColourT< ComponentType > operator-( RgbaColourT< ComponentType > const & lhs, RgbaColourT< ComponentType > const & rhs );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	lhs, rhs	The values to add
	 *\return		Result of lhs + rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	lhs, rhs	Les valeurs à ajouter
	 *\return		Resultat de lhs + rhs
	 */
	template< typename ComponentType, typename T >
	RgbaColourT< ComponentType > operator+( RgbaColourT< ComponentType > const & lhs, T rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result += rhs;
		return result;
	}
	/**
	 *\~english
	 *\brief		Subtraction operator
	 *\param[in]	lhs, rhs	The values to subtract
	 *\return		Result of lhs - rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	lhs, rhs	Les valeurs à soustraire
	 *\return		Resultat de lhs - rhs
	 */
	template< typename ComponentType, typename T >
	RgbaColourT< ComponentType > operator-( RgbaColourT< ComponentType > const & lhs, T rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result -= rhs;
		return result;
	}

	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs, rhs	The values to multiply
	 *\return		Result of lhs * rhs
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs, rhs	Les valeurs à multiplier
	 *\return		Resultat de lhs * rhs
	 */
	template< typename ComponentType, typename T >
	RgbaColourT< ComponentType > operator*( RgbaColourT< ComponentType > const & lhs, T rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result *= rhs;
		return result;
	}

	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	lhs, rhs	The values to divide
	 *\return		Result of lhs / rhs
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	lhs, rhs	Les valeurs à diviser
	 *\return		Resultat de lhs / rhs
	 */
	template< typename ComponentType, typename T >
	RgbaColourT< ComponentType > operator/( RgbaColourT< ComponentType > const & lhs, T rhs )
	{
		RgbaColourT< ComponentType > result( lhs );
		result /= rhs;
		return result;
	}
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGB format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point3ub toRGBByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGR format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point3ub toBGRByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGBA format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point4ub toRGBAByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGRA format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point4ub toBGRAByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ARGB format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point4ub toARGBByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ABGR format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	inline Point4ub toABGRByte( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGB format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point3f toRGBFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGR format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point3f toBGRFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGBA format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point4f toRGBAFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ARGB format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point4f toARGBFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ABGR format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point4f toABGRFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGRA format.
	 *\param[in]	colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA.
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	template< typename ComponentType >
	inline Point4f toBGRAFloat( RgbaColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the RGB format (0x00RRGGBB).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGB (0x00RRGGBB).
	 *\param[in]	colour	La couleur.
	 */
	inline uint32_t toRGBPacked( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the BGR format (0x00BBGGRR).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGR (0x00BBGGRR).
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	inline uint32_t toBGRPacked( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the ARGB format (0xAARRGGBB).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ARGB (0xAARRGGBB).
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	inline uint32_t toARGBPacked( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the RGBA format (0xRRGGBBAA).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGBA (0xRRGGBBAA).
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	inline uint32_t toRGBAPacked( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the ABGR format (0xAABBGGRR).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ABGR (0xAABBGGRR).
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	inline uint32_t toABGRPacked( RgbaColourT< ColourComponent > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the BGRA format (0xBBGGRRAA).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGRA (0xBBGGRRAA).
	 *\param[in]	colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	inline uint32_t toBGRAPacked( RgbaColourT< ColourComponent > const & colour );
}

#include "RgbaColour.inl"

#endif
