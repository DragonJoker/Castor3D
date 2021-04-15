/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextGeometry_H___
#define ___C3D_TextGeometry_H___

#include "Castor3D/Scene/Geometry.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Geometry >
		: public TextWriterT< castor3d::Geometry >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Geometry const & geometry
			, castor::StringStream & file )override;
	};
}

#endif
