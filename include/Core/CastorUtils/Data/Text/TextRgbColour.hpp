/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextRgbColour_H___
#define ___CU_TextRgbColour_H___

#include "CastorUtils/Graphics/RgbColour.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template< typename ComponentT >
	class TextWriter< RgbColourT< ComponentT > >
		: public TextWriterT< RgbColourT< ComponentT > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( RgbColourT< ComponentT > const & colour, StringStream & file )override;
	};
}

#include "TextRgbColour.inl"

#endif
