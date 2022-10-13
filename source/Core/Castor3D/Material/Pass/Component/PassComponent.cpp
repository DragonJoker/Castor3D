#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

CU_ImplementCUSmartPtr( castor3d, PassComponentPlugin )

namespace castor3d
{
	//************************************************************************************************

	castor::String const & getPassComponentType( PassComponent const & component )
	{
		return component.getType();
	}

	bool areFlagsEmpty( ComponentsTextures const & textures )
	{
		return textures.end() == std::find_if( textures.begin()
			, textures.end()
			, []( PassComponentTextureFlag const & lookup ) -> bool
			{
				return ( lookup != 0u );
			} );
	}

	ComponentsTextures makeComponentsTextures( TextureFlagsArray const & textures )
	{
		ComponentsTextures result{};
		uint32_t index{};

		for ( auto flag : castor::makeArrayView( textures.data(), std::min( textures.size(), result.size() ) ) )
		{
			result[index++] = flag;
		}

		return result;
	}

	ComponentsTextures::const_iterator checkFlags( ComponentsTextures const & flags
		, PassComponentTextureFlag flag )
	{
		auto [passIndex, textureFlag] = splitTextureFlag( flag );
		auto it = std::find_if( flags.begin()
			, flags.end()
			, [passIndex, textureFlag]( PassComponentTextureFlag const & lookup )
			{
				auto [lookupPassIndex, lookuptextureFlag] = splitTextureFlag( lookup );
				return lookupPassIndex == passIndex
					&& castor::checkFlag( lookuptextureFlag, textureFlag );
			} );
		return it;
	}

	//************************************************************************************************

	namespace shader
	{
		PassComponentID PassComponentsShader::getId()const
		{
			return m_plugin.getId();
		}
	}

	//************************************************************************************************

	PassComponent::PassComponent( Pass & pass
		, castor::String const & type )
		: castor::OwnedBy< Pass >{ pass }
		, m_type{ type }
		, m_id{ pass.getComponentId( m_type ) }
		, m_plugin{ pass.getComponentPlugin( m_id ) }
		, m_dirty{ pass.m_dirty }
		, m_materialShader{ pass.getMaterialShader( m_type ) }
	{
	}

	PassComponentUPtr PassComponent::clone( Pass & pass )const
	{
		return doClone( pass );
	}

	bool PassComponent::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return doWriteText( tabs, folder, subfolder, file );
	}

	void PassComponent::fillBuffer( PassBuffer & buffer )const
	{
		if ( m_materialShader )
		{
			doFillBuffer( buffer );
		}
	}

	void PassComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		getPlugin().fillTextureConfiguration( configuration, mask );
	}

	//************************************************************************************************
}
