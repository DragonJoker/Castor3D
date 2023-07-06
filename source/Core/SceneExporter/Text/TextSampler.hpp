/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSampler___
#define ___CSE_TextSampler___

#include <Castor3D/Material/Texture/Sampler.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Sampler >
		: public TextWriterT< castor3d::Sampler >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Sampler const & sampler
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< ashes::Sampler >
		: public TextWriterT< ashes::Sampler >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ashes::Sampler const & sampler
			, castor::StringStream & file )override;
	};
}

#endif
