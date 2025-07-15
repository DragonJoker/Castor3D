/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPanelOverlay_H___
#define ___CSE_TextPanelOverlay_H___

#include <Castor3D/Overlay/PanelOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< PanelOverlay >
		: public TextWriterT< PanelOverlay >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PanelOverlay const & overlay
			, StringStream & file )override;
	};
}

#endif
