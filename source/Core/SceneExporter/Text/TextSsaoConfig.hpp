/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSsaoConfig_H___
#define ___CSE_TextSsaoConfig_H___

#include <Castor3D/Render/Ssao/SsaoConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SsaoConfig >
		: public TextWriterT< castor3d::SsaoConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::SsaoConfig const & config
			, StringStream & file )override;
	};
}

#endif
