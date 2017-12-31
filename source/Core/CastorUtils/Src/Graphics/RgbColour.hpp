/*
See LICENSE file in root folder
*/
#ifndef ___CU_RgbColour_H___
#define ___CU_RgbColour_H___

#include "Math/Point.hpp"
#include "Data/TextLoader.hpp"
#include "Data/TextWriter.hpp"
#include "ColourComponent.hpp"
#include "HdrColourComponent.hpp"

#undef RGB

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Predefined colours enumeration
	\~french
	\brief		Enumération de couleurs prédéfinies
	*/
	enum class PredefinedRgbColour
		: uint32_t
	{
		eBlack = 0x000000FF,
		eDarkBlue = 0x00007FFF,
		eBlue = 0x0000FFFF,
		eDarkGreen = 0x007F00FF,
		eGreen = 0x00FF00FF,
		eDarkRed = 0x7F0000FF,
		eRed = 0xFF0000FF,
		eLightBlue = 0x7F7FFFFF,
		eLightGreen = 0x7FFF7FFF,
		eLightRed = 0xFF7F7FFF,
		eWhite = 0xFFFFFFFF,
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		RgbColourT components enumeration
	\~french
	\brief		Enumération des composantes d'une couleur
	*/
	enum class RgbComponent
		: uint8_t
	{
		//!\~english	Red PixelComponents
		//!\~french		Composante rouge
		eRed,
		//!\~english	Green PixelComponents
		//!\~french		Composante verte
		eGreen,
		//!\~english	Blue PixelComponents
		//!\~french		Composante bleue
		eBlue,
		CASTOR_SCOPED_ENUM_BOUNDS( eRed )
	};
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
	inline static String getPredefinedName( PredefinedRgbColour predefined );
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
	inline static PredefinedRgbColour getPredefinedRgb( String const & name );
	/*!
	\author		Sylvain DOREMUS
	\date		14/08/2010
	\~english
	\brief		RGB colour representation
	\remark		Predefines numerous colours
				<br />Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\~french
	\brief		Représentation d'une couleur RGB
	\remark		Prédéfinit differentes couleurs
				<br />Permet la conversion entre différents types de couleurs (RGB, ARGB, BGR, ...) et différents types de données (long, float, char, ...)
	*/
	template< typename ComponentType >
	class RgbColourT
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RgbColourT text loader
		\~french
		\brief		Loader de RgbColourT à partir d'un texte
		*/
		class TextLoader
			: public castor::TextLoader< RgbColourT< ComponentType > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			inline TextLoader();
			/**
			 *\~english
			 *\brief			Reads a colour from a text file
			 *\param[in,out]	file	The file from where we read the colour
			 *\param[in,out]	colour	The colour to read
			 *\~french
			 *\brief			Lit une couleur à partir d'un fichier texte
			 *\param[in,out]	file	Le fichier dans lequel on lit la couleur
			 *\param[in,out]	colour	La couleur à lire
			 */
			inline virtual bool operator()( RgbColourT< ComponentType > & colour, TextFile & file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RgbColourT text Writer
		\~french
		\brief		Writer de RgbColourT à partir d'un texte
		*/
		class TextWriter
			: public castor::TextWriter< RgbColourT< ComponentType > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			inline explicit TextWriter( String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a colour into a text file
			 *\param[in,out]	file		The file into which colour is written
			 *\param[in]		colour	The colour to write
			 *\~french
			 *\brief			Ecrit une couleur dans un fichier texte
			 *\param[in,out]	file		Le fichier dans lequel on écrit la couleur
			 *\param[in]		colour	La couleur à écrire
			 */
			inline virtual bool operator()( RgbColourT< ComponentType > const & colour, TextFile & file );
		};

	private:
		DECLARE_ARRAY( float, RgbComponent::eCount, Float4 );
		using ColourComponentArray = std::array< ComponentType, size_t( RgbComponent::eCount ) >;
		using ColourComponentArrayIt = typename ColourComponentArray::iterator;
		using ColourComponentArrayRIt = typename ColourComponentArray::reverse_iterator;
		using ColourComponentArrayConstIt = typename ColourComponentArray::const_iterator;
		using ColourComponentArrayConstRIt = typename ColourComponentArray::const_reverse_iterator;

	public:
		/**
		 *\~english
		 *\brief		Default Constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		inline RgbColourT();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	rhs	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	rhs	Couleur à copier
		 */
		inline RgbColourT( RgbColourT< ComponentType > const & rhs );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	rhs	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	rhs	Couleur à déplacer
		 */
		inline RgbColourT( RgbColourT< ComponentType > && rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	rhs	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	rhs	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		inline RgbColourT< ComponentType > & operator=( RgbColourT< ComponentType > const & rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	rhs	The object to copy
		 *\return		Reference to this colour
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	rhs	Couleur à déplacer
		 *\return		Référence sur cette couleur
		 */
		inline RgbColourT< ComponentType > & operator=( RgbColourT< ComponentType > && rhs );
		/**
		 *\~english
		 *\brief		Constructor from components
		 *\param[in]	r, g, b	The colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir des composantes
		 *\param[in]	r, g, b	Les composantes de la couleur
		 *\return		La RgbColourT construite
		 */
		template< typename T1, typename T2, typename T3 >
		static RgbColourT< ComponentType > fromComponents( T1 const & r, T2 const & g, T3 const & b )
		{
			RgbColourT result;
			result.m_components[size_t( RgbComponent::eRed )] = r;
			result.m_components[size_t( RgbComponent::eGreen )] = g;
			result.m_components[size_t( RgbComponent::eBlue )] = b;
			return result;
		}
		/**
		 *\~english
		 *\brief		Constructor from a predefined colour
		 *\param[in]	predefined
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'une couleur prédéfinie
		 *\param[in]	predefined
		 *\return		La RgbColourT construite
		 */
		static RgbColourT< ComponentType > fromPredefined( PredefinedRgbColour predefined )
		{
			return fromRGBA( ( uint32_t )predefined );
		}
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGB( Point3ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGR( Point3ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGBA( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGRA( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromARGB( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromABGR( Point4ub const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGB( Point3f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGR( Point3f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGBA( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromARGB( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromABGR( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGRA( Point4f const & colour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGB( uint8_t const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGR( uint8_t const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGBA( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGRA( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromARGB( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromABGR( uint8_t const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGB( float const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGR( float const( & colour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGBA( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromARGB( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromABGR( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	colour	The point containing the colour components
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	colour	Le point contenant les composantes
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGRA( float const( & colour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGB components
		 *\param[in]	colour	The uint32_t containing the colour (0x00RRGGBB)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGB
		 *\param[in]	colour	L'uint32_t contenant la couleur (0x00RRGGBB)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGB( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGR components
		 *\param[in]	colour	The uint32_t containing the colour (0x00BBGGRR)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGR
		 *\param[in]	colour	L'uint32_t contenant la couleur (0x00BBGGRR)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGR( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ARGB components
		 *\param[in]	colour	The uint32_t containing the colour (0xAARRGGBB)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ARGB
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xAARRGGBB)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromARGB( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGBA components
		 *\param[in]	colour	The uint32_t containing the colour (0xRRGGBBAA)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGBA
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xRRGGBBAA)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromRGBA( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ABGR components
		 *\param[in]	colour	The uint32_t containing the colour (0xAABBGGRR)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ABGR
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xAABBGGRR)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromABGR( uint32_t colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGRA components
		 *\param[in]	colour	The uint32_t containing the colour (0xBBGGRRAA)
		 *\return		The built RgbColourT
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGRA
		 *\param[in]	colour	L'uint32_t contenant la couleur (0xBBGGRRAA)
		 *\return		La RgbColourT construite
		 */
		inline static RgbColourT< ComponentType > fromBGRA( uint32_t colour );
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
			return &m_values[0];
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
			return &m_values[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked PixelComponents value
		 *\param[in]	component	The asked PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType & operator[]( RgbComponent component )
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked PixelComponents value
		 *\param[in]	component	The asked PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & operator[]( RgbComponent component )const
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked PixelComponents value
		 *\param[in]	component	The asked PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType & get( RgbComponent component )
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked PixelComponents value
		 *\param[in]	component	The asked PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	component	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & get( RgbComponent component )const
		{
			return m_components[size_t( component )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the red PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ComponentType & red()
		{
			return get( RgbComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the red PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & red()const
		{
			return get( RgbComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ComponentType & green()
		{
			return get( RgbComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & green()const
		{
			return get( RgbComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ComponentType & blue()
		{
			return get( RgbComponent::eBlue );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue PixelComponents value
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ComponentType const & blue()const
		{
			return get( RgbComponent::eBlue );
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
		inline RgbColourT< ComponentType > & operator+=( RgbColourT const & rhs );
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
		inline RgbColourT< ComponentType > & operator-=( RgbColourT const & rhs );
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
		RgbColourT< ComponentType > & operator+=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
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
		RgbColourT< ComponentType > & operator-=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
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
		RgbColourT< ComponentType > & operator*=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
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
		RgbColourT< ComponentType > & operator/=( T scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
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
		inline RgbColourT< ComponentType > & operator+=( ComponentType const & component );
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
		inline RgbColourT< ComponentType > & operator-=( ComponentType const & component );
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
		inline RgbColourT< ComponentType > & operator*=( ComponentType const & component );
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
		inline RgbColourT< ComponentType > & operator/=( ComponentType const & component );

	private:
		ColourComponentArray m_components;
		Float4Array m_values;
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
	inline bool operator==( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	lhs, rhs	The colours to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	lhs, rhs	Les couleurs à comparer
	 */
	template< typename ComponentType >
	inline bool operator!=( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs );
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
	inline RgbColourT< ComponentType > operator+( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs );
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
	inline RgbColourT< ComponentType > operator-( RgbColourT< ComponentType > const & lhs, RgbColourT< ComponentType > const & rhs );
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
	RgbColourT< ComponentType > operator+( RgbColourT< ComponentType > const & lhs, T rhs )
	{
		RgbColourT< ComponentType > result( lhs );
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
	RgbColourT< ComponentType > operator-( RgbColourT< ComponentType > const & lhs, T rhs )
	{
		RgbColourT< ComponentType > result( lhs );
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
	RgbColourT< ComponentType > operator*( RgbColourT< ComponentType > const & lhs, T rhs )
	{
		RgbColourT< ComponentType > result( lhs );
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
	RgbColourT< ComponentType > operator/( RgbColourT< ComponentType > const & lhs, T rhs )
	{
		RgbColourT< ComponentType > result( lhs );
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
	inline Point3ub toRGBByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point3ub toBGRByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point4ub toRGBAByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point4ub toBGRAByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point4ub toARGBByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point4ub toABGRByte( RgbColourT< ColourComponent > const & colour );
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
	inline Point3f toRGBFloat( RgbColourT< ComponentType > const & colour );
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
	inline Point3f toBGRFloat( RgbColourT< ComponentType > const & colour );
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
	inline Point4f toRGBAFloat( RgbColourT< ComponentType > const & colour );
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
	inline Point4f toARGBFloat( RgbColourT< ComponentType > const & colour );
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
	inline Point4f toABGRFloat( RgbColourT< ComponentType > const & colour );
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
	inline Point4f toBGRAFloat( RgbColourT< ComponentType > const & colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the RGB format (0x00RRGGBB).
	 *\param[in]	colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGB (0x00RRGGBB).
	 *\param[in]	colour	La couleur.
	 */
	inline uint32_t toRGBPacked( RgbColourT< ColourComponent > const & colour );
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
	inline uint32_t toBGRPacked( RgbColourT< ColourComponent > const & colour );
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
	inline uint32_t toARGBPacked( RgbColourT< ColourComponent > const & colour );
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
	inline uint32_t toRGBAPacked( RgbColourT< ColourComponent > const & colour );
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
	inline uint32_t toABGRPacked( RgbColourT< ColourComponent > const & colour );
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
	inline uint32_t toBGRAPacked( RgbColourT< ColourComponent > const & colour );
}

#include "RgbColour.inl"

#endif
