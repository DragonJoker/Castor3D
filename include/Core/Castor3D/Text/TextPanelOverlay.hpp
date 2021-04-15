/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextPanelOverlay_H___
#define ___C3D_TextPanelOverlay_H___

#include "Castor3D/Overlay/PanelOverlay.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PanelOverlay >
		: public TextWriterT< castor3d::PanelOverlay >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::PanelOverlay const & overlay
			, castor::StringStream & file );
	};
}

#endif
