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

		void PassComponentsShader::applyFloatComponent( castor::String const & mapName
			, castor::String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = castor::toUtf8( mapName );
			auto mbValueName = castor::toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto & writer{ *material.getWriter() };
			auto map = writer.declLocale( mbMapName + "Map"
				, material.getMember< sdw::UInt >( textureName ) >> 16u );
			auto mask = writer.declLocale( mbMapName + "Mask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			auto value = components.getMember< sdw::Float >( mbValueName );

			auto config = writer.declLocale( mbValueName + "Config"
				, textureConfigs.getTextureConfiguration( map ) );
			auto anim = writer.declLocale( mbValueName + "Anim"
				, textureAnims.getTextureAnimation( map ) );
			passShaders.computeTexcoords( textureConfigs
				, config
				, anim
				, components );
			auto sampled = writer.declLocale( mbValueName + "Sampled"
				, sampleTexture( map, config, components ) );
			value *= shader::TextureConfigData::getFloat( sampled, mask );
		}

		void PassComponentsShader::applyVec3Component( castor::String const & mapName
			, castor::String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = castor::toUtf8( mapName );
			auto mbValueName = castor::toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto & writer{ *material.getWriter() };
			auto map = writer.declLocale( mbMapName + "Map"
				, material.getMember< sdw::UInt >( textureName ) >> 16u );
			auto mask = writer.declLocale( mbMapName + "Mask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			auto value = components.getMember< sdw::Vec3 >( mbValueName );

			auto config = writer.declLocale( mbValueName + "Config"
				, textureConfigs.getTextureConfiguration( map ) );
			auto anim = writer.declLocale( mbValueName + "Anim"
				, textureAnims.getTextureAnimation( map ) );
			passShaders.computeTexcoords( textureConfigs
				, config
				, anim
				, components );
			auto sampled = writer.declLocale( mbValueName + "Sampled"
				, sampleTexture( map, config, components ) );
			value *= shader::TextureConfigData::getVec3( sampled, mask );
		}
	}

	//*********************************************************************************************

	PassComponent::PassComponent( Pass & pass
		, castor::String type
		, castor::StringArray deps )
		: castor::OwnedBy< Pass >{ pass }
		, m_type{ castor::move( type ) }
		, m_dependencies{ castor::move( deps ) }
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
		, uint32_t mask )const
	{
		getPlugin().fillTextureConfiguration( configuration, mask );
	}

	//************************************************************************************************
}
