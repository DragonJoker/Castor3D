#include "Castor3D/Shader/Ubos/ModelIndexUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const ModelIndex::BufferName = cuT( "ModelIndex" );
	castor::String const ModelIndex::DataName = cuT( "c3d_modelIndex" );

	ModelIndex::ModelIndex( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_textures0{ getMember< sdw::UVec4 >( "textures0" ) }
		, m_textures1{ getMember< sdw::UVec4 >( "textures1" ) }
		, m_textures{ getMember< sdw::Int >( "textures" ) }
		, m_materialId{ getMember< sdw::Int >( "materialId" ) }
		, m_nodeId{ getMember< sdw::Int >( "nodeId" ) }
		, m_skinningId{ getMember< sdw::Int >( "skinningId" ) }
	{
	}

	ast::type::BaseStructPtr ModelIndex::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_" + BufferName );

		if ( result->empty() )
		{
			result->declMember( "textures0", ast::type::Kind::eVec4U );
			result->declMember( "textures1", ast::type::Kind::eVec4U );
			result->declMember( "textures", ast::type::Kind::eInt );
			result->declMember( "materialId", ast::type::Kind::eInt );
			result->declMember( "nodeId", ast::type::Kind::eInt );
			result->declMember( "skinningId", ast::type::Kind::eInt );
		}

		return result;
	}

	sdw::UVec4 ModelIndex::getTextures0( ProgramFlags programFlags
		, sdw::UVec4 const & instanceData )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return instanceData;
		}

		return getTextures0();
	}

	sdw::UVec4 ModelIndex::getTextures0()const
	{
		return m_textures0;
	}

	sdw::UVec4 ModelIndex::getTextures1( ProgramFlags programFlags
		, sdw::UVec4 const & instanceData )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return instanceData;
		}

		return getTextures1();
	}

	sdw::UVec4 ModelIndex::getTextures1()const
	{
		return m_textures1;
	}

	sdw::Int ModelIndex::getTextures( ProgramFlags programFlags
		, sdw::Int const & instanceData )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return instanceData;
		}

		return getTextures();
	}

	sdw::Int ModelIndex::getTextures()const
	{
		return m_textures;
	}

	sdw::UInt ModelIndex::getMaterialId( ProgramFlags programFlags
		, sdw::Int const & instanceMaterial )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return getWriter()->cast< sdw::UInt >( instanceMaterial );
		}

		return getMaterialId();
	}

	sdw::UInt ModelIndex::getMaterialId()const
	{
		return getWriter()->cast< sdw::UInt >( m_materialId );
	}

	sdw::Int ModelIndex::getNodeId( ProgramFlags programFlags
		, sdw::Int const & instanceNodeId )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return instanceNodeId;
		}

		return getNodeId();
	}

	sdw::Int ModelIndex::getNodeId()const
	{
		return m_nodeId;
	}

	sdw::Int ModelIndex::getSkinningId( ProgramFlags programFlags
		, sdw::Int const & instanceSkinningId )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return instanceSkinningId;
		}

		return getNodeId();
	}

	sdw::Int ModelIndex::getSkinningId()const
	{
		return m_skinningId;
	}

	sdw::UInt ModelIndex::getTexture( sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
		, uint32_t index )
	{
		return ( index < 4u
			? textures0[index]
			: ( index < 8u
				? textures1[index - 4u]
				: 0_u ) );
	}
}
