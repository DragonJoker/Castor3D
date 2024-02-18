#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, NormalMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::NormalMapComponent >
		: public TextWriterT< castor3d::NormalMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::TextureConfiguration const & configuration )
			: TextWriterT< castor3d::NormalMapComponent >{ tabs }
			, m_configuration{ configuration }
		{
		}

		bool operator()( castor3d::NormalMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "normal_mask" ), getComponentsMask( m_configuration, object.getTextureFlags() ) )
				&& writeOpt( file, cuT( "normal_factor" ), m_configuration.normalFactor, 1.0f )
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalDirectX )
				&& writeOpt( file, cuT( "normal_2channels" ), m_configuration.normal2Channels );
		}

		bool operator()( StringStream & file
			, uint32_t mask )const
		{
			return writeMask( file, cuT( "normal_mask" ), mask )
				&& writeOpt( file, cuT( "normal_factor" ), m_configuration.normalFactor, 1.0f )
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalDirectX )
				&& writeOpt( file, cuT( "normal_2channels" ), m_configuration.normal2Channels );
		}

	private:
		castor3d::TextureConfiguration const & m_configuration;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace nmlcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitNormalMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( NormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( NormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUnitNormalFactor, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->configuration.normalFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUnitNormalDirectX, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->configuration.normalDirectX );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUnitNormal2Channels, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->configuration.normal2Channels );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapNormal, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( NormalMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapNormalMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( NormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapNormalDirectX, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->textureRemapIt->second.normalDirectX );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapNormal2Channels, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->textureRemapIt->second.normal2Channels );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void NormalMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		castor::MbString valueName = "normal";
		castor::MbString mapName = "normal";
		auto textureName = mapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( valueName ) )
		{
			return;
		}

		auto & writer{ *material.getWriter() };
		auto map = writer.declLocale( mapName + "Map"
			, material.getMember< sdw::UInt >( textureName ) >> 16u );
		auto mask = writer.declLocale( mapName + "Mask"
			, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );

		auto config = writer.declLocale( valueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( valueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );
		auto sampled = writer.declLocale( valueName + "Sampled"
			, sampleTexture( map, config, components ) );
		computeMikktNormal( config.nmlGMul(), config.nml2Chan(), mask, components, sampled );
	}

	void NormalMapComponent::ComponentsShader::computeMikktNormal( sdw::Float const & nmlGMul
		, sdw::UInt const & nml2Chan
		, sdw::UInt const & mask
		, shader::BlendComponents const & components
		, sdw::Vec4 const & sampled )
	{
		auto & writer{ *sampled.getWriter() };

		if ( components.usesDerivativeValues() )
		{
			auto normal = components.getMember< shader::DerivVec3 >( "normal" );
			auto tangent = components.getMember< shader::DerivVec4 >( "tangent" );
			auto bitangent = components.getMember< shader::DerivVec3 >( "bitangent" );
			auto tbn00 = shader::Utils::getTBN( normal.value(), tangent.value().xyz(), bitangent.value() );
			auto tbn10 = shader::Utils::getTBN( normal.value() + normal.dPdx(), tangent.value().xyz() + tangent.dPdx().xyz(), bitangent.value() + bitangent.dPdx() );
			auto tbn01 = shader::Utils::getTBN( normal.value() + normal.dPdy(), tangent.value().xyz() + tangent.dPdy().xyz(), bitangent.value() + bitangent.dPdy() );

			sampled[mask + 1u] = nmlGMul * sampled[mask + 1u];
			auto reconstructedNormal = writer.declLocale( "c3d_reconstructedNormalMikkt"
				, writer.ternary( nml2Chan != 0_u
					, shader::Utils::reconstructNormal( sampled[mask], sampled[mask + 1u] )
					, shader::TextureConfigData::getVec3( sampled, mask ) ) );
			reconstructedNormal = fma( vec3( 2.0_f ), reconstructedNormal, -vec3( 1.0_f ) );

			auto res00 = writer.declLocale( "c3d_mikktDerivRes00"
				, normalize( tbn00 * reconstructedNormal ) );
			auto res10 = writer.declLocale( "c3d_mikktDerivRes10"
				, normalize( tbn10 * reconstructedNormal ) );
			auto res01 = writer.declLocale( "c3d_mikktDerivRes01"
				, normalize( tbn01 * reconstructedNormal ) );
			normal.value() = res00;
			normal.dPdx() = res10 - res00;
			normal.dPdy() = res01 - res00;
		}
		else
		{
			auto normal = components.getMember< sdw::Vec3 >( "normal" );
			auto tangent = components.getMember< sdw::Vec4 >( "tangent" );
			auto bitangent = components.getMember< sdw::Vec3 >( "bitangent" );
			auto tbn = shader::Utils::getTBN( normal, tangent.xyz(), bitangent );
			sampled[mask + 1u] = nmlGMul * sampled[mask + 1u];
			normal = normalize( tbn
				* fma( vec3( 2.0_f )
					, writer.ternary( nml2Chan != 0_u
						, shader::Utils::reconstructNormal( sampled[mask], sampled[mask + 1u] )
						, shader::TextureConfigData::getVec3( sampled, mask ) )
					, -vec3( 1.0_f ) ) );
		}
	}

	//*********************************************************************************************

	void NormalMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "normal" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< NormalMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "normal_mask" )
			, nmlcmp::parserUnitNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "normal_factor" )
			, nmlcmp::parserUnitNormalFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "normal_directx" )
			, nmlcmp::parserUnitNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "normal_2channels" )
			, nmlcmp::parserUnitNormal2Channels
			, { castor::makeParameter< castor::ParameterType::eBool >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_mask" )
			, nmlcmp::parserUnitNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_factor" )
			, nmlcmp::parserUnitNormalFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_directx" )
			, nmlcmp::parserUnitNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_2channels" )
			, nmlcmp::parserUnitNormal2Channels
			, { castor::makeParameter< castor::ParameterType::eBool >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "normal" )
			, nmlcmp::parserTexRemapNormal );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_mask" )
			, nmlcmp::parserTexRemapNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_directx" )
			, nmlcmp::parserTexRemapNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_2channels" )
			, nmlcmp::parserTexRemapNormal2Channels
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	bool NormalMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eNormals )
			|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( filter, ComponentModeFlag::eOcclusion )
			|| hasAny( textures, getTextureFlags() );
	}

	void NormalMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, NormalMapComponent >( pass ) );
	}

	bool NormalMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< NormalMapComponent >{ tabs, configuration }( file, mask );
	}

	//*********************************************************************************************

	castor::String const NormalMapComponent::TypeName = C3D_MakePassMapComponentName( "normal" );

	NormalMapComponent::NormalMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Normal
			, { NormalComponent::TypeName } }
	{
	}

	PassComponentUPtr NormalMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, NormalMapComponent >( pass );
	}

	void NormalMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Normal" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
		vis.visit( cuT( "Factor" ), configuration.normalFactor );
		vis.visit( cuT( "DirectX" ), configuration.normalDirectX );
		vis.visit( cuT( "2 Channels" ), configuration.normal2Channels );
	}

	//*********************************************************************************************
}
