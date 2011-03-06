#ifndef ___Castor_Font___
#define ___Castor_Font___

#include "Resource.h"
#include "ResourceLoader.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include "Path.h"

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
	class FontLoader : ResourceLoader<Font>, public MemoryTraced<FontLoader>
	{
	public:
		FontPtr LoadFromFile( Manager<Font> * p_pManager, const String & p_strName, const Utils::Path & p_strPath, int p_uiHeight, ImageManager * p_pImageManager);
	};
	//! Font resource
	/*!
	Representation of a font : face, height, and others
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 17/01/2011
	*/
	class Font : public Resource<Font>, public MemoryTraced<Font>
	{
	protected:
		friend class FontLoader;
		size_t m_uiHeight;		// Holds The Height Of The Font.
		Utils::Path m_strPath;
		ImagePtr m_pImage;
		size_t m_uiNbRows;
		size_t m_uiNbColumns;

	protected:
		Font( Manager<Font> * p_pManager);
		Font( Manager<Font> * p_pManager, const String & p_strName, const Utils::Path & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);

	public:
		void Initialise( const Utils::Path & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);
		void Cleanup();

		const unsigned char *	operator []( unsigned char p_char)const;
		unsigned char *			operator []( unsigned char p_char);

		inline const ImagePtr &	GetImage	()const { return m_pImage; }
		inline size_t 			GetHeight	()const { return m_uiHeight; }

	private:
		void _loadChar( FT_Face p_ftFace, Buffer<unsigned char> & p_buffer, unsigned char p_char);
	};
	//! Fonts manager
	/*!
	Manages all the fonts loaded in the engine.
	\author Sylvain DOREMUS
	\date 17/01/2011
	*/
	class FontManager : public Castor::Templates::Manager<Font>, public MemoryTraced<FontManager>
	{
		friend class Castor::Templates::Manager<Font>;

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
		FontPtr LoadFont( const String & p_strName, const Utils::Path & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager);
	};
}
}

#endif
