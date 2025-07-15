/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextVctConfig_H___
#define ___CSE_TextVctConfig_H___

#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< VctConfig >
		: public TextWriterT< VctConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( VctConfig const & obj
			, StringStream & file )override;
	};
}

#endif
