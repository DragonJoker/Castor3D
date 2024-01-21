#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

CU_ImplementSmartPtr( castor3d, PassMapComponent )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		void PassMapMaterialShader::fillMaterialType( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits )const
		{
			if ( !type.hasMember( m_mapMemberName ) )
			{
				type.declMember( m_mapMemberName, ast::type::Kind::eUInt );
				inits.emplace_back( makeExpr( 0_u ) );
			}
		}
	}

	//*********************************************************************************************

	void PassMapComponentPlugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0u, 0u );
	}

	bool PassMapComponentPlugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		bool result = true;

		if ( auto it = checkFlag( configuration.components, getTextureFlags() );
			it != configuration.components.end() )
		{
			result = doWriteTextureConfig( configuration, it->componentsMask, tabs, file );
		}

		return result;
	}

	//*********************************************************************************************

	PassMapComponent::PassMapComponent( Pass & pass
		, castor::String type
		, TextureFlags textureFlags
		, castor::StringArray deps )
		: PassComponent{ pass, std::move( type ), std::move( deps ) }
		, m_textureFlags{ makeTextureFlag( getId(), textureFlags ) }
	{
	}

	void PassMapComponent::fillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		if ( hasAny( configuration.components, getTextureFlags() ) )
		{
			doFillConfig( configuration, vis );
		}
	}

	void PassMapComponent::doFillBuffer( PassBuffer & buffer )const
	{
		if ( getPlugin().getTextureFlags() == 0u )
		{
			m_plugin.zeroBuffer( *getOwner(), *m_materialShader, buffer );
		}

		auto tit = std::find_if( getOwner()->begin()
			, getOwner()->end()
			, [this]( TextureUnitRPtr const & lookup )
			{
				return hasAny( lookup->getFlags(), getPlugin().getTextureFlags() );
			} );

		if ( tit != getOwner()->end() )
		{
			auto data = buffer.getData( getOwner()->getId() );

			if ( auto cit = std::find_if( ( *tit )->getConfiguration().components.begin()
				, ( *tit )->getConfiguration().components.end()
				, [this]( TextureFlagConfiguration const & lookup )
				{
					return lookup.flag == getPlugin().getTextureFlags();
				} );
				cit != ( *tit )->getConfiguration().components.end() )
			{
				data.write( m_materialShader->getMaterialChunk()
					, uint32_t( ( ( *tit )->getId() << 16u ) | cit->startIndex )
					, 0u );
			}
			else
			{
				m_plugin.zeroBuffer( *getOwner(), *m_materialShader, buffer );
			}
		}
		else
		{
			m_plugin.zeroBuffer( *getOwner(), *m_materialShader, buffer );
		}
	}

	//*********************************************************************************************
}
