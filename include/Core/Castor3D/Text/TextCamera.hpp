/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextCamera_H___
#define ___C3D_TextCamera_H___

#include "Castor3D/Scene/Camera.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Camera >
		: public TextWriterT< castor3d::Camera >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Camera const & camera
			, TextFile & file )override;
	};
}

#endif
