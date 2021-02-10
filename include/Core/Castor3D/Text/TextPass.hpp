/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextPass_H___
#define ___C3D_TextPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Pass >
		: public TextWriterT< castor3d::Pass >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Pass const & pass
			, TextFile & file )override;
	};
}

#endif
