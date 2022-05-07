/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextRenderTarget_H___
#define ___CSE_TextRenderTarget_H___

#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RenderTarget >
		: public TextWriterT< castor3d::RenderTarget >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::RenderTarget const & target
			, castor::StringStream & file )override;
	};
}

#endif
