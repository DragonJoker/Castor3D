/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextWriterOverlay_H___
#define ___CSE_TextWriterOverlay_H___

#include <Castor3D/Overlay/Overlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Overlay >
		: public TextWriterT< castor3d::Overlay >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::Overlay const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
