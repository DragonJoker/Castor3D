/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextTextOverlay_H___
#define ___C3D_TextTextOverlay_H___

#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextOverlay >
		: public TextWriterT< castor3d::TextOverlay >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::TextOverlay const & overlay
			, castor::TextFile & file );
	};
}

#endif