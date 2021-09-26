/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextLight_H___
#define ___C3D_TextLight_H___

#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Light >
		: public TextWriterT< castor3d::Light >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Light const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
