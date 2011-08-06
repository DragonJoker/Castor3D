#ifndef ___Castor_Font___
#define ___Castor_Font___

#include "Resource.hpp"
#include "Loader.hpp"
#include "Collection.hpp"
#include "Serialisable.hpp"
#include "Point.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>

namespace Castor
{	namespace Resources
{
	class Font;
	//! Font factory
	/*!
	Creates fonts
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Font>
	{
	private:
		static unsigned int m_uiHeight;
		static int			m_iMaxHeight;
		static int			m_iMaxTop;
		static int			m_iMaxWidth;

	public:
		static bool Load( Font & p_font, Utils::Path const & p_path, unsigned int p_uiHeight);

	private:
		/**
		 * Creates a font from a name
		 *@param p_strName : [in/out] The font name
		 */
		static bool Load( Font & p_font, Utils::Path const & p_path);
	};
	//! Font character representation
	/*!
	Holds position, size and data of a character
	\author Sylvain DOREMUS
	\version 0.7.0.0
	\date 18/08/2011
	*/
	class Glyph
	{
	private:
		Math::Point2i				m_ptPosition;
		Math::Point2i				m_size;
		std::vector<unsigned char>	m_bitmap;

	public:
		/**@name Construction / Destruction */
		//@{
		Glyph();
		Glyph( Glyph const & p_glyph);
		Glyph & operator =( Glyph const & p_glyph);
		~Glyph();
		//@}

		/**@name Accessors */
		//@{
		inline Math::Point2i const &				GetSize		()const { return m_size; }
		inline Math::Point2i const &				GetPosition	()const { return m_ptPosition; }
		inline std::vector<unsigned char> const &	GetBitmap	()const { return m_bitmap; }
		inline std::vector<unsigned char> &			GetBitmap	()		{ return m_bitmap; }

		inline void	SetBitmap	( std::vector<unsigned char> const & val)	{ m_bitmap = val; }
		inline void SetPosition	( Math::Point2i const & val)				{ m_ptPosition = val; }
		inline void SetSize		( Math::Point2i const & val)				{ m_size = val; }
		//@}
	};
	//! Font resource
	/*!
	Representation of a font : face, precision, and others
	A generated font will be put in an image
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 17/01/2011
	*/
	class Font : public Resource<Font>, public Utils::Serialisable, public MemoryTraced<Font>
	{
	protected:
		unsigned int		m_uiHeight;		// Holds the height of the font.
		Utils::Path			m_pathFile;
		std::vector<Glyph>	m_arrayChars;
		int					m_iMaxHeight;
		int					m_iMaxTop;
		int					m_iMaxWidth;

	protected:
		DECLARE_INVARIANT_BLOCK()
		DECLARE_SERIALISE_MAP()

	public:
		Font( String const & p_strName, unsigned int p_uiHeight);
		Font( Font const & p_font);
		Font & operator =( Font const & p_font);
		virtual ~Font();
		void Initialise( Math::Point<unsigned int, 2> const & p_ptSize, std::vector<unsigned char> const & p_buffer, Utils::Path const & p_strPath);
		void Cleanup();

		inline void	SetMaxHeight	( int p_iHeight)	{ m_iMaxHeight = p_iHeight; }
		inline void	SetMaxWidth		( int p_iWidth)		{ m_iMaxWidth = p_iWidth; }

		std::vector<unsigned char> const &	operator []	( wchar_t p_char)const;
		std::vector<unsigned char> &		operator []	( wchar_t p_char);
		inline Glyph const &	GetGlyphAt				( wchar_t p_char)const	{ return m_arrayChars[p_char]; }
		inline Glyph &			GetGlyphAt				( wchar_t p_char)		{ return m_arrayChars[p_char]; }
		inline unsigned int		GetHeight				()const					{ return m_uiHeight; }
		inline int				GetMaxHeight			()const					{ return m_iMaxHeight; }
		inline int				GetMaxWidth				()const					{ return m_iMaxWidth; }
	};
}
}

#endif
