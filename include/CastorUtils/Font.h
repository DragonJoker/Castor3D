#ifndef ___Castor_Font___
#define ___Castor_Font___

#include "Resource.h"
#include "ResourceLoader.h"
#include <ft2build.h>
#include <freetype/freetype.h>

namespace Castor
{	namespace Resources
{
	class Font;

	//! Font resource loader
	/*!
	Loads fonts
	\author Sylvain DOREMUS
	\date 17/01/2011
	*/
	class FontLoader : ResourceLoader<Font, FontManager>, public MemoryTraced<FontLoader>
	{
	public:
		FontPtr LoadFromFile( FontManager * p_pManager, const String & p_strName, const String & p_strPath, int p_uiHeight, ImageManager * p_pImageManager);
	};
	//! Font resource
	/*!
	Representation of a font : face, height, and others
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 17/01/2011
	*/
	class Font : public Resource<Font, FontManager>, public MemoryTraced<Font>
	{
	protected:
		friend class FontLoader;
		size_t m_uiHeight;		// Holds The Height Of The Font.
		String m_strPath;
		ImagePtr m_pImage;
		size_t m_uiNbRows;
		size_t m_uiNbColumns;

	protected:
		Font( FontManager * p_pManager);
		Font( FontManager * p_pManager, const String & p_strName, const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);

	public:
		void Initialise( const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);
		void Cleanup();

		const unsigned char *	operator []( unsigned char p_char)const;
		unsigned char *			operator []( unsigned char p_char);

	private:
		void _loadChar( FT_Face p_ftFace, Buffer<unsigned char> & p_buffer, char p_char);
	};
	//! Fonts manager
	/*!
	Manages all the fonts loaded in the engine.
	\author Sylvain DOREMUS
	\date 17/01/2011
	*/
	class FontManager : public Castor::Templates::Manager<String, Font, FontManager>, public MemoryTraced<FontManager>
	{
		friend class Castor::Templates::Manager<String, Font, FontManager>;

	public:
		/**
		* Constructor
		*/
		FontManager(){}
		/**
		* Destructor
		*/
		~FontManager(){}
		/**
		* Creates a font with the name
		*@param p_name : [in] The font face
		*@return The created font
		*/
		FontPtr CreateFont( const String & p_strName, const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);
	};
}
}

#endif