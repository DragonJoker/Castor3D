/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSsaoConfig_H___
#define ___CSE_TextSsaoConfig_H___

#include <Castor3D/Render/Ssao/SsaoConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SsaoConfig >
		: public TextWriterT< SsaoConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SsaoConfig const & config
			, StringStream & file )override;
	};
}

#endif
