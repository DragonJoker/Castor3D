#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

CU_ImplementSmartPtr( c3d, PassMapComponent )

namespace c3d
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

		//*********************************************************************************************

		sdw::Float PassMapComponentsShader::loadFloatComponent( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName ) )
			{
				return 1.0_f;
			}

			return doLoadFloatComponent( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		sdw::Vec2 PassMapComponentsShader::loadVec2Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName ) )
			{
				return vec2( 1.0_f );
			}

			return doLoadVec2Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		sdw::Vec3 PassMapComponentsShader::loadVec3Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName ) )
			{
				return vec3( 1.0_f );
			}

			return doLoadVec3Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		sdw::Vec4 PassMapComponentsShader::loadVec4Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = mbMapName + "MapAndMask";

			if ( !material.hasMember( textureName ) )
			{
				return vec4( 1.0_f );
			}

			return doLoadVec4Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		void PassMapComponentsShader::applyFloatComponent( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = toUtf8( mapName ) + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto value = components.getMember< sdw::Float >( mbValueName );
			value *= doLoadFloatComponent( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		void PassMapComponentsShader::applyVec2Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = toUtf8( mapName ) + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto value = components.getMember< sdw::Vec2 >( mbValueName );
			value *= doLoadVec2Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		void PassMapComponentsShader::applyVec3Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = toUtf8( mapName ) + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto value = components.getMember< sdw::Vec3 >( mbValueName );
			value *= doLoadVec3Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		void PassMapComponentsShader::applyVec4Component( String const & mapName
			, String const & valueName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto mbMapName = toUtf8( mapName );
			auto mbValueName = toUtf8( valueName );
			auto textureName = toUtf8( mapName ) + "MapAndMask";

			if ( !material.hasMember( textureName )
				|| !components.hasMember( mbValueName ) )
			{
				return;
			}

			auto value = components.getMember< sdw::Vec4 >( mbValueName );
			value *= doLoadVec4Component( mbMapName, mbValueName, textureName
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture );
		}

		sdw::Float PassMapComponentsShader::doLoadFloatComponent( MbString const & mbMapName
			, MbString const & mbValueName
			, MbString const & textureName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto & writer{ *material.getWriter() };
			auto mask = writer.declLocale( mbMapName + "Mask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			return shader::TextureConfigData::getFloat( doLoadVec4Component( mbMapName, mbValueName, textureName
					, passShaders, textureConfigs, textureAnims
					, material, components, sampleTexture )
				, mask );
		}

		sdw::Vec2 PassMapComponentsShader::doLoadVec2Component( MbString const & mbMapName
			, MbString const & mbValueName
			, MbString const & textureName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto & writer{ *material.getWriter() };
			auto mask = writer.declLocale( mbMapName + "Mask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			return shader::TextureConfigData::getVec2( doLoadVec4Component( mbMapName, mbValueName, textureName
					, passShaders, textureConfigs, textureAnims
					, material, components, sampleTexture )
				, mask );
		}

		sdw::Vec3 PassMapComponentsShader::doLoadVec3Component( MbString const & mbMapName
			, MbString const & mbValueName
			, MbString const & textureName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto & writer{ *material.getWriter() };
			auto mask = writer.declLocale( mbMapName + "Mask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			return shader::TextureConfigData::getVec3( doLoadVec4Component( mbMapName, mbValueName, textureName
					, passShaders, textureConfigs, textureAnims
					, material, components, sampleTexture )
				, mask );
		}

		sdw::Vec4 PassMapComponentsShader::doLoadVec4Component( MbString const & mbMapName
			, MbString const & mbValueName
			, MbString const & textureName
			, PassShaders const & passShaders
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, Material const & material
			, BlendComponents & components
			, SampleTexture const & sampleTexture )const
		{
			auto & writer{ *material.getWriter() };
			auto map = writer.declLocale( mbMapName + "Map"
				, material.getMember< sdw::UInt >( textureName ) >> 16u );
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
			return sampled;
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
		, String const & tabs
		, StringStream & file )const
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
		, String type
		, TextureFlags textureFlags
		, StringArray deps )
		: PassComponent{ pass, c3d::move( type ), c3d::move( deps ) }
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

	void PassMapComponent::createDefaultTexture( Pass & pass
		, String name
		, TextureConfiguration config
		, ImageCreateParams imageParams )
	{
		TextureSourceInfo sourceInfo{ std::move( name ), std::move( config ), std::move( imageParams ) };
		PassTextureConfig passConfig{ getEngine( pass )->getDefaultSampler() };
		sourceInfo.setSerialisable( false );
		pass.registerTexture( std::move( sourceInfo ), std::move( passConfig ) );
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
