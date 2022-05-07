/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextOverlayCategory_H___
#define ___CSE_TextOverlayCategory_H___

#include <Castor3D/Overlay/OverlayCategory.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OverlayCategory >
		: public TextWriterT< castor3d::OverlayCategory >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::OverlayCategory const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
