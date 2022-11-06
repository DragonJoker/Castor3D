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
		meshletsBounds = ( ( index >> offset ) & 0x01 ) != 0u;
		offset += 1u;
		hasMorphingWeights = ( ( index >> offset ) & 0x01 ) != 0u;
	}

	uint32_t TransformPipeline::getIndex( SubmeshFlags const & submeshFlags
		, MorphFlags const & morphFlags
		, ProgramFlags const & programFlags
		, bool morphingWeights )
	{
		constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
		constexpr auto maxMorphSize = castor::getBitSize( uint32_t( MorphFlag::eAllBase ) );
		static_assert( maxSubmeshSize + maxMorphSize + 1u <= 32 );
		auto offset = 0u;
		uint32_t result{};
		result = uint32_t( submeshFlags ) << offset;
		offset += maxSubmeshSize;
		result |= uint32_t( morphFlags ) << offset;
		offset += maxMorphSize;
		result |= uint32_t( checkFlag( programFlags, ProgramFlag::eHasTask ) ? 1u : 0u ) << offset;
		offset += 1u;
		result |= uint32_t( bool( morphingWeights ) ? 1u : 0u ) << offset;
		return result;
	}

	std::string TransformPipeline::getName()const
	{
		return getName( submeshFlags, morphFlags, meshletsBounds, hasMorphingWeights );
	}

	std::string TransformPipeline::getName( SubmeshFlags const & submeshFlags
		, MorphFlags const & morphFlags
		, bool meshletsBounds
		, bool hasMorphingWeights )
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

		if ( meshletsBounds )
		{
			result += "MorphWeights";
		}

		return result;
	}

	//*********************************************************************************************

	BoundsTransformPipeline::BoundsTransformPipeline( bool pnormals )
		: normals{ pnormals }
	{
	}

	std::string BoundsTransformPipeline::getName()const
	{
		return  getName( normals );
	}

	std::string BoundsTransformPipeline::getName( bool normals )
	{
		std::string result =  "MeshletBoundsTransformPass";

		if ( normals )
		{
			result += "Nml";
		}

		return result;

	}

	//*********************************************************************************************
}
