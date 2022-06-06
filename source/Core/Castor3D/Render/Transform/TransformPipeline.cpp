#include "Castor3D/Render/Transform/TransformPipeline.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

namespace castor3d
{
	//*********************************************************************************************

	TransformPipeline::TransformPipeline( uint32_t pindex )
		: index{ pindex }
	{
		constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
		constexpr auto maxMorphSize = castor::getBitSize( uint32_t( MorphFlag::eAllBase ) );
		uint32_t offset = 0u;
		submeshFlags = SubmeshFlags( ( index >> offset ) & uint32_t( SubmeshFlag::eAllBase ) );
		offset += maxSubmeshSize;
		morphFlags = MorphFlags( ( index >> offset ) & uint32_t( MorphFlag::eAllBase ) );
		offset += maxMorphSize;
		meshletsBounds = ( index >> offset ) != 0u;
	}

	std::string TransformPipeline::getName()const
	{
		return getName( submeshFlags, morphFlags, meshletsBounds );
	}

	std::string TransformPipeline::getName( SubmeshFlags const & submeshFlags
		, MorphFlags const & morphFlags
		, bool meshletsBounds )
	{
		std::string result = "VertexTransformPass";

		if ( morphFlags != MorphFlag::eNone )
		{
			result += "Morph";
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eSkin ) )
		{
			result += "Skin";
		}

		if ( meshletsBounds )
		{
			result += "Meshlet";
		}

		return result;
	}

	//*********************************************************************************************

	std::string BoundsTransformPipeline::getName()
	{
		return  "MeshletBoundsTransformPass";
	}

	//*********************************************************************************************
}
