/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextPosition_H___
#define ___CU_TextPosition_H___

#include "CastorUtils/Graphics/Position.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template<>
	class TextWriter< Position >
		: public TextWriterT< Position >
	{
	public:
		CU_API explicit TextWriter( String const & tabs );
		CU_API bool operator()( Position const & object, StringStream & file )override;
	};
}

#endif
