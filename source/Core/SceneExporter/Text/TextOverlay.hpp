/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextWriterOverlay_H___
#define ___CSE_TextWriterOverlay_H___

#include <Castor3D/Overlay/Overlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Overlay >
		: public TextWriterT< Overlay >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Overlay const & overlay
			, StringStream & file )override;
	};
}

#endif
