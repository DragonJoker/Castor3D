/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSsaoConfig_H___
#define ___C3D_TextSsaoConfig_H___

#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SsaoConfig >
		: public TextWriterT< castor3d::SsaoConfig >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::SsaoConfig const & config
			, TextFile & file )override;
	};
}

#endif
