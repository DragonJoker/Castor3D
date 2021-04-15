/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextWriterOverlay_H___
#define ___C3D_TextWriterOverlay_H___

#include "Castor3D/Overlay/Overlay.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Overlay >
		: public TextWriterT< castor3d::Overlay >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::Overlay const & overlay
			, castor::StringStream & file );
	};
}

#endif
