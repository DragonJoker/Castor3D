/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBorderPanelOverlay_H___
#define ___CSE_TextBorderPanelOverlay_H___

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< BorderPanelOverlay >
			: public TextWriterT< BorderPanelOverlay >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( BorderPanelOverlay const & overlay
			, StringStream & file )override;
	};
}

#endif
