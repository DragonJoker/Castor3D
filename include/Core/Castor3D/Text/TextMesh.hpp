/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextMesh_H___
#define ___C3D_TextMesh_H___

#include "Castor3D/Model/Mesh/Mesh.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Mesh >
		: public TextWriterT< castor3d::Mesh >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Mesh const & material
			, TextFile & file )override;
	};
}

#endif
