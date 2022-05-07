/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextVoxelSceneData_H___
#define ___CSE_TextVoxelSceneData_H___

#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::VoxelSceneData >
		: public TextWriterT< castor3d::VoxelSceneData >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::VoxelSceneData const & obj
			, StringStream & file )override;
	};
}

#endif
