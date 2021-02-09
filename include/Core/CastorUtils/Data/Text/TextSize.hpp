/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextSize_H___
#define ___CU_TextSize_H___

#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template<>
	class TextWriter< Size >
		: public TextWriterT< Size >
	{
	public:
		CU_API explicit TextWriter( String const & tabs );
		CU_API bool operator()( Size const & object, TextFile & file )override;
	};
}

#endif
