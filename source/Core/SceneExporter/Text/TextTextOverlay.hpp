/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextOverlay_H___
#define ___CSE_TextTextOverlay_H___

#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< TextOverlay >
		: public TextWriterT< TextOverlay >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( TextOverlay const & overlay
			, StringStream & file )override;
	};
}

#endif
