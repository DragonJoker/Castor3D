#include "FileParserContext.hpp"

namespace Castor
{
	FileParserContext::FileParserContext( TextFile * p_pFile )
		: pFile( p_pFile )
		, ui64Line( 0 )
	{
	}

	FileParserContext::~FileParserContext()
	{
	}
}
