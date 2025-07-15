/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextRenderTarget_H___
#define ___CSE_TextRenderTarget_H___

#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< RenderTarget >
		: public TextWriterT< RenderTarget >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( RenderTarget const & target
			, StringStream & file )override;
	};
}

#endif
