#include "Castor3D/Render/Transform/TransformPipeline.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

namespace castor3d
{
	//*********************************************************************************************

	TransformPipeline::TransformPipeline( uint32_t pindex )
		: index{ pindex }
	{
		constexpr auto maxSubmeshSize = sizeof( SubmeshComponentCombineID ) * 8u;
		constexpr auto maxMorphSize = castor::getBitSize( uint32_t( MorphFlag::eAllBase ) );
		uint32_t offset = 0u;
		combineID = SubmeshComponentCombineID( ( index >> offset ) & SubmeshComponentCombineID( 0xFFFF ) );
		offset += maxSubmeshSize;
		morphFlags = MorphFlags( ( index >> offset ) & uint32_t( MorphFlag::eAllBase ) );
		offset += maxMorphSize;
		meshletsBounds = ( ( index >> offset ) & 0x01 ) != 0u;
		offset += 1u;
		hasMorphingWeights = ( ( index >> offset ) & 0x01 ) != 0u;
	}

	uint32_t TransformPipeline::getIndex( SubmeshComponentCombineID combine
		, MorphFlags const & morphFlags
		, ProgramFlags const & programFlags
		, bool morphingWeights )
	{
		constexpr auto maxSubmeshSize = sizeof( SubmeshComponentCombineID ) * 8u;
		constexpr auto maxMorphSize = castor::getBitSize( uint32_t( MorphFlag::eAllBase ) );
		static_assert( maxSubmeshSize + maxMorphSize + 1u <= 32 );
		auto offset = 0u;
		uint32_t result{};
		result = uint32_t( combine ) << offset;
		offset += maxSubmeshSize;
		result |= uint32_t( morphFlags ) << offset;
		offset += maxMorphSize;
		result |= uint32_t( checkFlag( programFlags, ProgramFlag::eHasTask ) ? 1u : 0u ) << offset;
		offset += 1u;
		result |= uint32_t( bool( morphingWeights ) ? 1u : 0u ) << offset;
		return result;
	}

	std::string TransformPipeline::getName( Engine const & engine )const
	{
		return getName( engine, combineID, morphFlags, meshletsBounds, hasMorphingWeights );
	}

	std::string TransformPipeline::getName( Engine const & engine
		, SubmeshComponentCombineID combineID
		, MorphFlags const & morphFlags
		, bool meshletsBounds
		, bool hasMorphingWeights )
	{
		auto combine = engine.getSubmeshComponentsRegister().getSubmeshComponentCombine( combineID );
		std::string result = "VertexTransformPass";

		if ( morphFlags != MorphFlag::eNone )
		{
			result += "Morph";
		}

		if ( combine.hasSkinFlag )
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
		return getName( normals );
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
