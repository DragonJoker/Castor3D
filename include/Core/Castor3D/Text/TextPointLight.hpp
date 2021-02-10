/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextPointLight_H___
#define ___C3D_TextPointLight_H___

#include "Castor3D/Scene/Light/PointLight.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PointLight >
		: public TextWriterT< castor3d::PointLight >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::PointLight const & light
			, castor::TextFile & file );
	};
}

#endif
