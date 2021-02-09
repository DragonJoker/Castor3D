/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextBorderPanelOverlay_H___
#define ___C3D_TextBorderPanelOverlay_H___

#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::BorderPanelOverlay >
			: public TextWriterT< castor3d::BorderPanelOverlay >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs );
		C3D_API bool operator()( castor3d::BorderPanelOverlay const & overlay
			, castor::TextFile & file );
	};
}

#endif
