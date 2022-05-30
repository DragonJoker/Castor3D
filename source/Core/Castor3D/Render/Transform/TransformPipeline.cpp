#include "Castor3D/Render/Transform/TransformPipeline.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

namespace castor3d
{
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
		programFlags = ProgramFlags( ( index >> offset ) & uint32_t( ProgramFlag::eAllBase ) );
	}

	std::string TransformPipeline::getName()const
	{
		return getName( submeshFlags, morphFlags, programFlags );
	}

	std::string TransformPipeline::getName( SubmeshFlags const & submeshFlags
		, MorphFlags const & morphFlags
		, ProgramFlags const & programFlags )
	{
		std::string result = "VertexTransformPass";

		if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
		{
			result += "Morph";
		}

		return result;
	}
}
