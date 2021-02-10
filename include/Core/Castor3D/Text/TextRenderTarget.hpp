/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextRenderTarget_H___
#define ___C3D_TextRenderTarget_H___

#include "Castor3D/Render/RenderTarget.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RenderTarget >
		: public TextWriterT< castor3d::RenderTarget >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::RenderTarget const & target
			, TextFile & file )override;
	};
}

#endif
