/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextVoxelSceneData_H___
#define ___C3D_TextVoxelSceneData_H___

#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::VoxelSceneData >
		: public TextWriterT< castor3d::VoxelSceneData >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::VoxelSceneData const & obj
			, StringStream & file )override;
	};
}

#endif
