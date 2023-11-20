#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( castor3d, PassComponent )
CU_ImplementSmartPtr( castor3d, PassComponentPlugin )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		PassComponentID PassComponentsShader::getId()const
		{
			return m_plugin.getId();
		}

		sdw::Vec4 PassComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 const & texCoords
			, shader::BlendComponents const & components )const
		{
			return map.sample( texCoords.xy() );
		}

		sdw::Vec4 PassComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
			, shader::DerivTex const & texCoords
			, shader::BlendComponents const & components )const
		{
			return map.grad( texCoords.uv()
				, texCoords.dPdx()
				, texCoords.dPdy() );
		}

		void PassComponentsShader::applyFloatComponent( std::string const & name
			, PassComponentTextureFlag flag
			, shader::TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, shader::BlendComponents & components )const
		{
			if ( !components.hasMember( name ) )
			{
				return;
			}

			auto & writer{ *sampled.getWriter() };

			IF( writer, imgCompConfig.x() == sdw::UInt{ flag } )
			{
				components.getMember< sdw::Float >( name ) *= config.getFloat( sampled, imgCompConfig.z() );
			}
			FI;
		}

		void PassComponentsShader::applyVec3Component( std::string const & name
			, PassComponentTextureFlag flag
			, shader::TextureConfigData const & config
			, sdw::U32Vec3 const & imgCompConfig
			, sdw::Vec4 const & sampled
			, shader::BlendComponents & components )const
		{
			if ( !components.hasMember( name ) )
			{
				return;
			}

			auto & writer{ *sampled.getWriter() };

			IF( writer, imgCompConfig.x() == sdw::UInt{ flag } )
			{
				components.getMember< sdw::Vec3 >( name ) *= config.getVec3( sampled, imgCompConfig.z() );
			}
			FI;
		}
	}

	//*********************************************************************************************

	PassComponent::PassComponent( Pass & pass
		, castor::String type
		, castor::StringArray deps )
		: castor::OwnedBy< Pass >{ pass }
		, m_type{ std::move( type ) }
		, m_dependencies{ std::move( deps ) }
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
