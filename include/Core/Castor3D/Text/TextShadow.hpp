/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextShadow_H___
#define ___C3D_TextShadow_H___

#include "Castor3D/Scene/Shadow.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Shadow >
		: public TextWriterT< castor3d::Shadow >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Shadow const & config
			, TextFile & file )override;
	};
}

#endif
