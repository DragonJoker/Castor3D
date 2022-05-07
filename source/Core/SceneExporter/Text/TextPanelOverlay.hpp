/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPanelOverlay_H___
#define ___CSE_TextPanelOverlay_H___

#include <Castor3D/Overlay/PanelOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PanelOverlay >
		: public TextWriterT< castor3d::PanelOverlay >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::PanelOverlay const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
