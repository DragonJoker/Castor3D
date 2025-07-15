/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextOverlayCategory_H___
#define ___CSE_TextOverlayCategory_H___

#include <Castor3D/Overlay/OverlayCategory.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< OverlayCategory >
		: public TextWriterT< OverlayCategory >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( OverlayCategory const & overlay
			, StringStream & file )override;
	};
}

#endif
