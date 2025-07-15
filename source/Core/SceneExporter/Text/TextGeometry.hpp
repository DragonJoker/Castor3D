/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextGeometry_H___
#define ___CSE_TextGeometry_H___

#include <Castor3D/Scene/Geometry.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Geometry >
		: public TextWriterT< Geometry >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Geometry const & geometry
			, StringStream & file )override;
	};
}

#endif
