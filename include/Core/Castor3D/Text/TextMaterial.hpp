/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextMaterial_H___
#define ___C3D_TextMaterial_H___

#include "Castor3D/Material/Material.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Material >
		: public TextWriterT< castor3d::Material >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Material const & material
			, TextFile & file )override;
	};
}

#endif
