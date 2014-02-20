#ifndef ___Castor_Font___
#define ___Castor_Font___

#include "Resource.hpp"
#include "Loader.hpp"
#include "Collection.hpp"
#include "Point.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>

namespace Castor
{
	class Font;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/08/2011
	\~english
	\brief		Font character (glyph) representation
	\remark		Holds position, size and data of a character
	\~french
	\brief		Représentation d'un caractère d'une Font (glyphe)
	\remark		Contient la position, taille et pixels d'un caractère
	*/
	class Glyph
	{
	private:
		//!\~english	Glyph position, in texture	\~french	Poisition de la glyphe, dans la texture
		Position m_position;
		//!\~english	Glyph dimensions	\~french	Dimensions de la glyphe
		Size m_size;
		//!\~english	Pixels to advance in order to go next character	\~french	Nombre de pixels pour placer le caractère suivant
		Size m_advance;
		//!\~english	Glyph image	\~french	Image de la glyphe
		ByteArray m_bitmap;
		//!\~english	Glyph character	\~french	Caractère de la glyphe
		wchar_t m_wcCharacter;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_wcChar	The glyph character
		 *\param[in]	p_size		The glyph dimensions
		 *\param[in]	p_position	The glyph position
		 *\param[in]	p_advance	Pixels to advance in order to go next character
		 *\param[in]	p_bitmap	The glyph image
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_wcChar	Le caractère de la glyphe
		 *\param[in]	p_size		Les dimensions de la glyphe
		 *\param[in]	p_position	La position de la glyphe
		 *\param[in]	p_advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		Glyph( wchar_t p_wcChar=0, Size const & p_size = Size( ), Position const & p_position = Position( ), Size const & p_advance = Size( ), ByteArray const & p_bitmap = ByteArray( ) );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_glyph	The Glyph object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_glyph	L'objet Glyph à copier
		 */
		Glyph( Glyph const & p_glyph );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_glyph	The Glyph object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_glyph	L'objet Glyph à déplacer
		 */
		Glyph( Glyph && p_glyph );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_glyph	The Glyph object to copy
		 *\return		A reference to this Glyph object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_glyph	L'objet Glyph à copier
		 *\return		Une référence sur cet objet Glyph
		 */
		Glyph & operator =( Glyph const & p_glyph );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_glyph	The Glyph object to move
		 *\return		A reference to this Glyph object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_glyph	L'objet Glyph à déplacer
		 *\return		Une référence sur cet objet Glyph
		 */
		Glyph & operator =( Glyph && p_glyph );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Glyph();
		/**
		 *\~english
		 *\brief		Retrieves the glyph character
		 *\return		The value
		 *\~french
		 *\brief		Récupère le caractère de la glyphe
		 *\return		La valeur
		 */
		inline wchar_t GetCharacter()const { return m_wcCharacter; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph dimensions
		 *\return		The glyph dimensions
		 *\~french
		 *\brief		Récupère les dimensions de la glyphe
		 *\return		Les dimensions de la glyphe
		 */
		inline Size const & GetSize()const { return m_size; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph position
		 *\return		The glyph position
		 *\~french
		 *\brief		Récupère la position de la glyphe
		 *\return		La position de la glyphe
		 */
		inline Position const & GetPosition()const { return m_position; }
		/**
		 *\~english
		 *\brief		Retrieves the number of pixels to go before drawing next glyph
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de pixels pour place la prchaine glyphe
		 *\return		La valeur
		 */
		inline Size const & GetAdvance()const { return m_advance; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph image
		 *\return		A constant reference to the image of the glyph
		 *\~french
		 *\brief		Récupère l'image de la glyphe
		 *\return		Une référence constante sur l'image de la glyphe
		 */
		inline ByteArray const & GetBitmap()const { return m_bitmap; }
		/**
		 *\~english
		 *\brief		Adjusts the glyph position
		 *\param[in]	x, y	The glyph position decal
		 *\~french
		 *\brief		Ajuste la position de la glyphe
		 *\param[in]	x, y	Le décalage de position de la glyphe
		 */
		inline void AdjustPosition( uint32_t x, uint32_t y ) { m_position.offset( x, y ); }
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		17/01/2011
	\~english
	\brief		Font resource
	\remark		Representation of a font : face, precision, and others
				<br />The generated font will be put in an image
	\~french
	\brief		Ressource Font
	\remark		Représentation d'une font : face, précision, et autres
				<br />La font chargée sera placée dans une image
	*/
	class Font : public Resource< Font >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		03/01/2011
		\~english
		\brief		Font loader
		\remark		Uses FreeType to load font
		\~french
		\brief		Loader de Font
		\remark		Utilise FreeType pour charger la police
		*/
		class BinaryLoader : public Loader< Font, eFILE_TYPE_BINARY, BinaryFile >
		{
		private:
			//!\~english	Font wanted height	\~french	Hauteur voulue pour la police
			uint32_t	m_uiHeight;

		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a font
			 *\param[in,out]	p_font		The font to load
			 *\param[in]		p_path		The path of the font file
			 *\param[in]		p_uiHeight	The font precision
			 *\~french
			 *\brief			Charge une font
			 *\param[in,out]	p_font		La font à charger
			 *\param[in]		p_path		Le chemin du fichier contenant la font
			 *\param[in]		p_uiHeight	La précision de la font
			 */
			bool operator ()( Font & p_font, Path const & p_path, uint32_t p_uiHeight);

		private:
			virtual bool operator ()( Font & p_font, Path const & p_path);
			void DoLoadGlyph( FT_Face p_ftFace, wchar_t p_char, Font & p_font, int & p_iMaxTop );
		};
		DECLARE_VECTOR( Glyph, Glyph );
	protected:
		//!\~english	The height of the font			\~french	 La hauteur de la police
		uint32_t m_uiHeight;
		//!\~english	The path of the font file		\~french	 Le chemin du fichier de la police
		Path m_pathFile;
		//!\~english	The array of glyphs				\~french	 Le tableau de glyphes
		GlyphArray m_arrayGlyphs;
		//!\~english	The max height of the glyphs	\~french	 La hauteur maximale des glyphes
		int m_iMaxHeight;
		//!\~english	The max top of the glyphs		\~french	 La position haute maximale des glyphes
		int m_iMaxTop;
		//!\~english	The max width of the glyphs		\~french	 La largeur maximale des glyphes
		int m_iMaxWidth;

	protected:
		DECLARE_INVARIANT_BLOCK()

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_strName	The font name
		 *\param[in]	p_uiHeight	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_strName	Le nom de la police
		 *\param[in]	p_uiHeight	La hauteur des caractères de la police
		 */
		Font( String const & p_strName, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_font	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_font	L'objet à copier
		 */
		Font( Font const & p_font );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_font	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_font	L'objet à déplacer
		 */
		Font( Font && p_font );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_font	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_font	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Font & operator =( Font const & p_font );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_font	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_font	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		Font & operator =( Font && p_font );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Font();
		/**
		 *\~english
		 *\brief		Sets the glyph for given character
		 *\param[in]	p_char		The character
		 *\param[in]	p_size		The glyph dimensions
		 *\param[in]	p_position	The glyph position
		 *\param[in]	p_advance	Pixels to advance in order to go next character
		 *\param[in]	p_bitmap	The glyph image
		 *\~french
		 *\brief		Définit la glyphe pour le caractère donné
		 *\param[in]	p_char		Le caractère
		 *\param[in]	p_size		Les dimensions de la glyphe
		 *\param[in]	p_position	La position de la glyphe
		 *\param[in]	p_advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		void SetGlyphAt( wchar_t p_char, Size const & p_size, Position p_position, Size const & p_advance, ByteArray const & p_bitmap );
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A constant reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence constante sur la glyphe
		 */
		inline Glyph const & GetGlyphAt( wchar_t p_char )const { return m_arrayGlyphs[p_char]; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		inline Glyph & GetGlyphAt( wchar_t p_char ) { return m_arrayGlyphs[p_char]; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A constant reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence constante sur la glyphe
		 */
		inline Glyph const & operator []( wchar_t p_char )const { return m_arrayGlyphs[p_char]; }
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		inline Glyph & operator []( wchar_t p_char ) { return m_arrayGlyphs[p_char]; }
		/**
		 *\~english
		 *\brief		Retrieves the height of the font
		 *\return		The font height
		 *\~french
		 *\brief		Récupère la hauteur de la police
		 *\return		La hauteur de la police
		 */
		inline uint32_t GetHeight()const { return m_uiHeight; }
		/**
		 *\~english
		 *\brief		Retrieves the max height of the glyphs
		 *\return		The glyphs max height
		 *\~french
		 *\brief		Récupère la hauteur maximale des glyphes
		 *\return		La hauteur maximale des glyphes
		 */
		inline int GetMaxHeight	()const { return m_iMaxHeight; }
		/**
		 *\~english
		 *\brief		Retrieves the max width of the glyphs
		 *\return		The glyphs max width
		 *\~french
		 *\brief		Récupère la largeur maximale des glyphes
		 *\return		La largeur maximale des glyphes
		 */
		inline int GetMaxWidth()const { return m_iMaxWidth; }
		/**
		 *\~english
		 *\brief		Sets the max height of the glyphs
		 *\param[in]	p_iHeight	The new height
		 *\~french
		 *\brief		Définit la hauteur maximale des glyphes
		 *\param[in]	p_iHeight	La nouvelle hauteur
		 */
		inline void SetMaxHeight( int p_iHeight ) { m_iMaxHeight = p_iHeight; }
		/**
		 *\~english
		 *\brief		Sets the max width of the glyphs
		 *\param[in]	p_iWidth	The new width
		 *\~french
		 *\brief		Définit la largeur maximale des glyphes
		 *\param[in]	p_iWidth	La nouvelle largeur
		 */
		inline void SetMaxWidth( int p_iWidth ) { m_iMaxWidth = p_iWidth; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the first glyph
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la première glyphe
		 *\return		L'itérateur
		 */
		inline GlyphArray::iterator Begin() { return m_arrayGlyphs.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the first glyph
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la première glyphe
		 *\return		L'itérateur
		 */
		inline GlyphArray::const_iterator Begin()const { return m_arrayGlyphs.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the glyph array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de glyphes
		 *\return		L'itérateur
		 */
		inline GlyphArray::iterator End() { return m_arrayGlyphs.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the glyph array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de glyphes
		 *\return		L'itérateur
		 */
		inline GlyphArray::const_iterator End()const { return m_arrayGlyphs.end(); }
	};
}

#endif
