/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextOverlayCategory_H___
#define ___C3D_TextOverlayCategory_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OverlayCategory >
		: public TextWriterT< castor3d::OverlayCategory >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::OverlayCategory const & overlay
			, TextFile & file )override;
	};
}

#endif
