/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLpvConfig_H___
#define ___CSE_TextLpvConfig_H___

#include <Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::LpvConfig >
		: public TextWriterT< castor3d::LpvConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::LpvConfig const & config
			, castor::StringStream & file )override;
	};
}

#endif
