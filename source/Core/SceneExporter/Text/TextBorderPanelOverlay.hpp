/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBorderPanelOverlay_H___
#define ___CSE_TextBorderPanelOverlay_H___

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::BorderPanelOverlay >
			: public TextWriterT< castor3d::BorderPanelOverlay >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::BorderPanelOverlay const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
