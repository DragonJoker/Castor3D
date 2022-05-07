/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextOverlay_H___
#define ___CSE_TextTextOverlay_H___

#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextOverlay >
		: public TextWriterT< castor3d::TextOverlay >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::TextOverlay const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
