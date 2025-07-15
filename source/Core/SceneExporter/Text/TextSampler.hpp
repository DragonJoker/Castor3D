/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSampler___
#define ___CSE_TextSampler___

#include <Castor3D/Material/Texture/Sampler.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Sampler >
		: public TextWriterT< Sampler >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Sampler const & sampler
			, StringStream & file )override;
	};
}

#endif
