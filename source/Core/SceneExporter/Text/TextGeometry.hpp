/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextGeometry_H___
#define ___CSE_TextGeometry_H___

#include <Castor3D/Scene/Geometry.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Geometry >
		: public TextWriterT< castor3d::Geometry >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Geometry const & geometry
			, castor::StringStream & file )override;
	};
}

#endif
