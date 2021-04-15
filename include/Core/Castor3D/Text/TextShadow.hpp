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
	class TextWriter< castor3d::ShadowConfig >
		: public TextWriterT< castor3d::ShadowConfig >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::ShadowConfig const & config
			, StringStream & file )override;
	};
}

#endif
