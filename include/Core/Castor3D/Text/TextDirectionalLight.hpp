/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextDirectionalLight_H___
#define ___C3D_TextDirectionalLight_H___

#include "Castor3D/Scene/Light/DirectionalLight.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::DirectionalLight >
		: public TextWriterT< castor3d::DirectionalLight >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::DirectionalLight const & light
			, castor::StringStream & file )override;
	};
}

#endif
