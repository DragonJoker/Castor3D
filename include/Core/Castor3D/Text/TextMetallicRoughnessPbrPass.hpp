/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextMetallicRoughnessPbrPass_H___
#define ___C3D_TextMetallicRoughnessPbrPass_H___

#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::MetallicRoughnessPbrPass >
		: public TextWriterT< castor3d::MetallicRoughnessPbrPass >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::MetallicRoughnessPbrPass const & pass
			, TextFile & file )override;
	};
}

#endif
