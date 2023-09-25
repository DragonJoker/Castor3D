/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextVctConfig_H___
#define ___CSE_TextVctConfig_H___

#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::VctConfig >
		: public TextWriterT< castor3d::VctConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::VctConfig const & obj
			, StringStream & file )override;
	};
}

#endif
