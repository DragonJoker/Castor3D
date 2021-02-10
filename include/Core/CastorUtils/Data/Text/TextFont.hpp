/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextFont_H___
#define ___CU_TextFont_H___

#include "CastorUtils/Graphics/Font.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template<>
	class TextWriter< Font >
		: public TextWriterT< Font >
	{
	public:
		CU_API explicit TextWriter( String const & tabs );
		CU_API bool operator()( Font const & object, TextFile & file )override;
	};
}

#endif
