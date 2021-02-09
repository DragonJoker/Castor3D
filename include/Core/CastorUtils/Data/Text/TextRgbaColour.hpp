/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextRgbaColour_H___
#define ___CU_TextRgbaColour_H___

#include "CastorUtils/Graphics/RgbaColour.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template< typename ComponentT >
	class TextWriter< RgbaColourT< ComponentT > >
		: public castor::TextWriterT< RgbaColourT< ComponentT > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( RgbaColourT< ComponentT > const & colour, TextFile & file )override;
	};
}

#include "TextRgbaColour.inl"

#endif
