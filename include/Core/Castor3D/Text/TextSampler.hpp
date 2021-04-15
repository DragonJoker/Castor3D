/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSampler___
#define ___C3D_TextSampler___

#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Sampler >
		: public TextWriterT< castor3d::Sampler >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Sampler const & sampler
			, castor::StringStream & file )override;
	};
}

#endif
