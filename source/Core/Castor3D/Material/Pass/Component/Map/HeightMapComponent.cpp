#include "Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"
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

CU_ImplementSmartPtr( castor3d, HeightMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::HeightMapComponent >
		: public TextWriterT< castor3d::HeightMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::TextureConfiguration const & configuration )
			: TextWriterT< castor3d::HeightMapComponent >{ tabs }
			, m_configuration{ configuration }
		{
		}

		bool operator()( castor3d::HeightMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "height_mask" ), getComponentsMask( m_configuration, object.getTextureFlags() ) )
				&& writeOpt( file, cuT( "height_factor" ), m_configuration.heightFactor, 1.0f );
		}

		bool operator()( StringStream & file
			, uint32_t mask )const
		{
			return writeMask( file, cuT( "height_mask" ), mask )
				&& writeOpt( file, cuT( "height_factor" ), m_configuration.heightFactor, 1.0f );
		}

	private:
		castor3d::TextureConfiguration const & m_configuration;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace hgtcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitHeightMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( HeightMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( HeightMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUnitHeightFactor, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->configuration.heightFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapHeight, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( HeightMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapHeightMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( HeightMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static bool enableParallaxOcclusionMapping( PassComponentRegister const & passComponents
			, PassComponentCombine const & pass )
		{
			auto & plugin = passComponents.getPlugin< HeightComponent >();
			return ( hasAny( pass, makePassComponentFlag( plugin.getId(), HeightComponent::eParallaxOcclusionMappingRepeat ) )
					|| hasAny( pass, makePassComponentFlag( plugin.getId(), HeightComponent::eParallaxOcclusionMappingOne ) ) );
		}

		static bool enableParallaxOcclusionMappingOne( PassComponentRegister const & passComponents
			, PassComponentCombine const & pass )
		{
			auto & plugin = passComponents.getPlugin< HeightComponent >();
			return hasAny( pass, makePassComponentFlag( plugin.getId(), HeightComponent::eParallaxOcclusionMappingOne ) );
		}
	}

	//*********************************************************************************************

	void HeightMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		std::string valueName = "height";
		std::string mapName = "height";
		auto textureName = mapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( "tangentSpaceViewPosition" )
			|| !components.hasMember( "tangentSpaceFragPosition" ) )
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

		if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			auto texCoords = components.getMember< shader::DerivTex >( "texCoords" );
			doComputeTexcoord( passShaders.getPassCombine()
				, config
				, maps[map - 1u]
				, texCoords
				, mask
				, components );
		}
		else if ( passShaders.getPassCombine().baseId == 0u )
		{
			auto texCoords = components.getMember< sdw::Vec2 >( "texCoords" );
			doComputeTexcoord( passShaders.getPassCombine()
				, config
				, maps[map - 1u]
				, texCoords
				, mask
				, components );
		}
		else
		{
			auto texCoords = components.getMember< sdw::Vec3 >( "texCoords" );
			doComputeTexcoord( passShaders.getPassCombine()
				, config
				, maps[map - 1u]
				, texCoords
				, mask
				, components );
		}
	}

	void HeightMapComponent::ComponentsShader::doComputeTexcoord( PassComponentCombine const & pass
		, shader::TextureConfigData const & config
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec2 & texCoords
		, sdw::UInt const & mask
		, shader::BlendComponents & components )const
	{
		if ( hgtcmp::enableParallaxOcclusionMapping( getPlugin().getRegister(), pass ) )
		{
			auto & writer = findWriterMandat( config, mask, map, texCoords, components );
			auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
			auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
			parallaxMapping( texCoords
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, map
				, config
				, mask );

			if ( hgtcmp::enableParallaxOcclusionMappingOne( getPlugin().getRegister(), pass ) )
			{
				IF( writer, texCoords.x() > 1.0_f
					|| texCoords.y() > 1.0_f
					|| texCoords.x() < 0.0_f
					|| texCoords.y() < 0.0_f )
				{
					writer.demote();
				}
				FI
			}

			auto texCoords0 = components.getMember< sdw::Vec2 >( "texture0" );
			auto texCoords1 = components.getMember< sdw::Vec2 >( "texture1", true );
			auto texCoords2 = components.getMember< sdw::Vec2 >( "texture2", true );
			auto texCoords3 = components.getMember< sdw::Vec2 >( "texture3", true );
			shader::TextureConfigurations::setTexcoord( config
				, texCoords
				, texCoords0
				, texCoords1
				, texCoords2
				, texCoords3 );
		}
	}

	void HeightMapComponent::ComponentsShader::doComputeTexcoord( PassComponentCombine const & pass
		, shader::TextureConfigData const & config
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::UInt const & mask
		, shader::BlendComponents & components )const
	{
		if ( hgtcmp::enableParallaxOcclusionMapping( getPlugin().getRegister(), pass ) )
		{
			auto & writer = findWriterMandat( config, mask, map, texCoords, components );
			auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
			auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
			parallaxMapping( texCoords
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, map
				, config
				, mask );

			if ( hgtcmp::enableParallaxOcclusionMappingOne( getPlugin().getRegister(), pass ) )
			{
				IF( writer, config.getUv( texCoords ).x() > 1.0_f
					|| config.getUv( texCoords ).y() > 1.0_f
					|| config.getUv( texCoords ).x() < 0.0_f
					|| config.getUv( texCoords ).y() < 0.0_f )
				{
					writer.demote();
				}
				FI
			}

			auto texCoords0 = components.getMember< sdw::Vec3 >( "texture0" );
			auto texCoords1 = components.getMember< sdw::Vec3 >( "texture1", true );
			auto texCoords2 = components.getMember< sdw::Vec3 >( "texture2", true );
			auto texCoords3 = components.getMember< sdw::Vec3 >( "texture3", true );
			shader::TextureConfigurations::setTexcoord( config
				, texCoords
				, texCoords0
				, texCoords1
				, texCoords2
				, texCoords3 );
		}
	}

	void HeightMapComponent::ComponentsShader::doComputeTexcoord( PassComponentCombine const & pass
		, shader::TextureConfigData const & config
		, sdw::CombinedImage2DRgba32 const & map
		, shader::DerivTex & texCoords
		, sdw::UInt const & mask
		, shader::BlendComponents & components )const
	{
		if ( hgtcmp::enableParallaxOcclusionMapping( getPlugin().getRegister(), pass ) )
		{
			auto & writer = findWriterMandat( config, mask, map, texCoords, components );
			auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
			auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
			parallaxMapping( texCoords
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, map
				, config
				, mask );

			if ( hgtcmp::enableParallaxOcclusionMappingOne( getPlugin().getRegister(), pass ) )
			{
				IF( writer, config.getUv( texCoords ).x() > 1.0_f
					|| config.getUv( texCoords ).y() > 1.0_f
					|| config.getUv( texCoords ).x() < 0.0_f
					|| config.getUv( texCoords ).y() < 0.0_f )
				{
					writer.demote();
				}
				FI
			}

			auto texCoords0 = components.getMember< shader::DerivTex >( "texture0" );
			auto texCoords1 = components.getMember< shader::DerivTex >( "texture1", true );
			auto texCoords2 = components.getMember< shader::DerivTex >( "texture2", true );
			auto texCoords3 = components.getMember< shader::DerivTex >( "texture3", true );
			shader::TextureConfigurations::setTexcoord( config
				, texCoords
				, texCoords0
				, texCoords1
				, texCoords2
				, texCoords3 );
		}
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( sdw::Vec2 const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, shader::TextureConfigData const & textureConfig
		, sdw::UInt const & mask )const
	{
		return parallaxMapping( texCoords
			, dFdxCoarse( texCoords )
			, dFdyCoarse( texCoords )
			, viewDir
			, heightMap
			, textureConfig
			, mask );
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( sdw::Vec3 const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, shader::TextureConfigData const & textureConfig
		, sdw::UInt const & mask )const
	{
		return parallaxMapping( texCoords.xy()
			, dFdxCoarse( texCoords.xy() )
			, dFdyCoarse( texCoords.xy() )
			, viewDir
			, heightMap
			, textureConfig
			, mask );
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( shader::DerivTex const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, shader::TextureConfigData const & textureConfig
		, sdw::UInt const & mask )const
	{
		parallaxMapping( texCoords.uv()
			, texCoords.dPdx()
			, texCoords.dPdy()
			, viewDir
			, heightMap
			, textureConfig
			, mask );
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( sdw::Vec2 ptexCoords
		, sdw::Vec2 const & pdx
		, sdw::Vec2 const & pdy
		, sdw::Vec3 const & pviewDir
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, shader::TextureConfigData const & ptextureConfig
		, sdw::UInt const & pmask )const
	{
		if ( !m_parallaxMapping )
		{
			auto & writer = findWriterMandat( ptexCoords
				, pdx
				, pdy
				, pviewDir
				, pheightMap
				, ptextureConfig
				, pmask );
			m_parallaxMapping = writer.implementFunction< sdw::Vec2 >( "c3d_parallaxMapping",
				[&writer]( sdw::Vec2 const & texCoords
					, sdw::Vec2 const & dx
					, sdw::Vec2 const & dy
					, sdw::Vec3 const & viewDir
					, sdw::CombinedImage2DRgba32 const & heightMap
					, shader::TextureConfigData const & textureConfig
					, sdw::UInt const & mask )
				{
					// number of depth layers
					auto minLayers = writer.declLocale( "minLayers"
						, 32.0_f );
					auto maxLayers = writer.declLocale( "maxLayers"
						, 64.0_f );
					auto numLayers = writer.declLocale( "numLayers"
						, mix( maxLayers
							, minLayers
							, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), viewDir ) ) ) );
					// calculate the size of each layer
					auto layerDepth = writer.declLocale( "layerDepth"
						, 1.0_f / numLayers );
					// depth of current layer
					auto currentLayerDepth = writer.declLocale( "currentLayerDepth"
						, 0.0_f );
					// the amount to shift the texture coordinates per layer (from vector P)
					auto p = writer.declLocale( "p"
						, viewDir.xy() / viewDir.z() * textureConfig.hgtFact() );
					auto deltaTexCoords = writer.declLocale( "deltaTexCoords"
						, p / numLayers );

					auto currentTexCoords = writer.declLocale( "currentTexCoords"
						, texCoords );
					auto heightIndex = writer.declLocale( "heightIndex"
						, mask );
					auto sampled = writer.declLocale( "sampled"
						, heightMap.grad( currentTexCoords, dx, dy ) );
					auto currentDepthMapValue = writer.declLocale( "currentDepthMapValue"
						, sampled[heightIndex] );

					WHILE( writer, currentLayerDepth < currentDepthMapValue )
					{
						// shift texture coordinates along direction of P
						currentTexCoords -= deltaTexCoords;
						// get depthmap value at current texture coordinates
						sampled = heightMap.grad( currentTexCoords, dx, dy );
						currentDepthMapValue = sampled[heightIndex];
						// get depth of next layer
						currentLayerDepth += layerDepth;
					}
					ELIHW

					// get texture coordinates before collision (reverse operations)
					auto prevTexCoords = writer.declLocale( "prevTexCoords"
						, currentTexCoords + deltaTexCoords );

					// get depth after and before collision for linear interpolation
					auto afterDepth = writer.declLocale( "afterDepth"
						, currentDepthMapValue - currentLayerDepth );
					sampled = heightMap.grad( prevTexCoords, dx, dy );
					auto beforeDepth = writer.declLocale( "beforeDepth"
						, sampled[heightIndex] - currentLayerDepth + layerDepth );

					// interpolation of texture coordinates
					auto weight = writer.declLocale( "weight"
						, vec2( afterDepth / ( afterDepth - beforeDepth ) ) );
					auto finalTexCoords = writer.declLocale( "finalTexCoords"
						, sdw::fma( prevTexCoords
							, weight
							, currentTexCoords * ( vec2( 1.0_f ) - weight ) ) );

					writer.returnStmt( finalTexCoords );
				}
				, sdw::InVec2{ writer, "texCoords" }
				, sdw::InVec2{ writer, "dx" }
				, sdw::InVec2{ writer, "dy" }
				, sdw::InVec3{ writer, "viewDir" }
				, sdw::InCombinedImage2DRgba32{ writer, "heightMap" }
				, shader::InTextureConfigData{ writer, "textureConfig" }
				, sdw::InUInt{ writer, "mask" } );
		}

		ptexCoords = m_parallaxMapping( ptexCoords
			, pdx
			, pdy
			, pviewDir
			, pheightMap
			, ptextureConfig
			, pmask );
	}

	sdw::RetFloat HeightMapComponent::ComponentsShader::parallaxShadow( sdw::Vec3 const & plightDir
		, sdw::Vec2 const & pinitialTexCoord
		, sdw::Float const & pinitialHeight
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, shader::TextureConfigData const & ptextureConfig
		, sdw::UInt const & pmask )
	{
		if ( !m_parallaxShadow )
		{
			auto & writer = findWriterMandat( plightDir
				, pinitialTexCoord
				, pinitialHeight
				, pheightMap
				, ptextureConfig
				, pmask );
			m_parallaxShadow = writer.implementFunction< sdw::Float >( "c3d_parallaxSoftShadowMultiplier"
				, [&writer]( sdw::Vec3 const & lightDir
					, sdw::Vec2 const & initialTexCoord
					, sdw::Float const & initialHeight
					, sdw::CombinedImage2DRgba32 const & heightMap
					, shader::TextureConfigData const & textureConfig
					, sdw::UInt const & mask )
				{
					auto shadowMultiplier = writer.declLocale( "shadowMultiplier"
						, 1.0_f );
					auto minLayers = writer.declLocale( "minLayers"
						, 10.0_f );
					auto maxLayers = writer.declLocale( "maxLayers"
						, 20.0_f );

					// calculate lighting only for surface oriented to the light source
					IF( writer, dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) > 0.0_f )
					{
						// calculate initial parameters
						auto numSamplesUnderSurface = writer.declLocale( "numSamplesUnderSurface"
							, 0.0_f );
						shadowMultiplier = 0.0_f;
						auto numLayers = writer.declLocale( "numLayers"
							, mix( maxLayers
								, minLayers
								, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) ) ) );
						auto layerHeight = writer.declLocale( "layerHeight"
							, initialHeight / numLayers );
						auto texStep = writer.declLocale( "deltaTexCoords"
							, ( lightDir.xy() * textureConfig.hgtFact() ) / lightDir.z() / numLayers );

						// current parameters
						auto currentLayerHeight = writer.declLocale( "currentLayerHeight"
							, initialHeight - layerHeight );
						auto currentTextureCoords = writer.declLocale( "currentTextureCoords"
							, initialTexCoord + texStep );
						auto heightIndex = writer.declLocale( "heightIndex"
							, mask );
						auto sampled = writer.declLocale( "sampled"
							, heightMap.sample( currentTextureCoords ) );
						auto heightFromTexture = writer.declLocale( "heightFromTexture"
							, sampled[heightIndex] );
						auto stepIndex = writer.declLocale( "stepIndex"
							, 1_i );

						// while point is below depth 0.0 )
						WHILE( writer, currentLayerHeight > 0.0_f )
						{
							// if point is under the surface
							IF( writer, heightFromTexture < currentLayerHeight )
							{
								// calculate partial shadowing factor
								numSamplesUnderSurface += 1.0_f;
								auto newShadowMultiplier = writer.declLocale( "newShadowMultiplier"
									, ( currentLayerHeight - heightFromTexture )
									* ( 1.0_f - writer.cast< sdw::Float >( stepIndex ) / numLayers ) );
								shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
							}
							FI

							// offset to the next layer
							stepIndex += 1_i;
							currentLayerHeight -= layerHeight;
							currentTextureCoords += texStep;
							sampled = heightMap.sample( currentTextureCoords );
							heightFromTexture = sampled[heightIndex];
						}
						ELIHW

						// Shadowing factor should be 1 if there were no points under the surface
						IF( writer, numSamplesUnderSurface < 1.0_f )
						{
							shadowMultiplier = 1.0_f;
						}
						ELSE
						{
							shadowMultiplier = 1.0_f - shadowMultiplier;
						}
						FI
					}
					FI

					writer.returnStmt( shadowMultiplier );
				}
				, sdw::InVec3{ writer, "lightDir" }
				, sdw::InVec2{ writer, "initialTexCoord" }
				, sdw::InFloat{ writer, "initialHeight" }
				, sdw::InCombinedImage2DRgba32{ writer, "heightMap" }
				, shader::InTextureConfigData{ writer, "textureConfig" }
				, sdw::InUInt{ writer, "mask" } );
		}

		return m_parallaxShadow( plightDir
			, pinitialTexCoord
			, pinitialHeight
			, pheightMap
			, ptextureConfig
			, pmask );
	}

	//*********************************************************************************************

	void HeightMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		static UInt32StrMap const parallaxOcclusionModes{ getEnumMapT< ParallaxOcclusionMode >() };

		channelFillers.try_emplace( "height"
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< HeightMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "height_mask" )
			, hgtcmp::parserUnitHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "height_factor" )
			, hgtcmp::parserUnitHeightFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_mask" )
			, hgtcmp::parserUnitHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_factor" )
			, hgtcmp::parserUnitHeightFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "height" )
			, hgtcmp::parserTexRemapHeight );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "height_mask" )
			, hgtcmp::parserTexRemapHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool HeightMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry )
			|| hasAny( textures, getTextureFlags() );
	}

	void HeightMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, HeightMapComponent >( pass ) );
	}

	bool HeightMapComponent::Plugin::hasTexcoordModif( PassComponentRegister const & passComponents
		, PipelineFlags const * flags )const
	{
		return flags
			? flags->enableTangentSpace()
				&& hasAny( flags->textures, getTextureFlags() )
				&& hgtcmp::enableParallaxOcclusionMapping( passComponents, flags->pass )
			: false;
	}

	bool HeightMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< HeightMapComponent >{ tabs, configuration }( file, mask );
	}

	//*********************************************************************************************

	castor::String const HeightMapComponent::TypeName = C3D_MakePassMapComponentName( "height" );

	HeightMapComponent::HeightMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Height
			, { HeightComponent::TypeName } }
	{
	}

	PassComponentUPtr HeightMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, HeightMapComponent >( pass );
	}

	void HeightMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Height" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
		vis.visit( cuT( "Factor" ), configuration.heightFactor );
	}

	//*********************************************************************************************
}
