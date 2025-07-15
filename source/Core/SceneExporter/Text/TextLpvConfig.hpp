/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLpvConfig_H___
#define ___CSE_TextLpvConfig_H___

#include <Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< LpvConfig >
		: public TextWriterT< LpvConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( LpvConfig const & config
			, StringStream & file )override;
	};
}

#endif
