#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"

namespace c3d::shader
{
	//*********************************************************************************************

	sdw::UInt getNodeId( sdw::ArrayStorageBufferT< shader::ObjectsIds > const & data
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID )
	{
		return data[pipelineID].getNodeId( drawID );
	}

	//*********************************************************************************************
}
