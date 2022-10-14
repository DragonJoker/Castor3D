#include "Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

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
			, uint32_t mask )
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
		static CU_ImplementAttributeParser( parserUnitHeightMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( HeightMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitHeightFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				getPassComponent< HeightMapComponent >( parsingContext );
				params[0]->get( parsingContext.textureConfiguration.heightFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapHeight )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( HeightMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapHeightMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( HeightMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static bool enableParallaxOcclusionMapping( PipelineFlags const & flags
			, PassComponentTextureFlag const & flag )
		{
			return flags.enableTangentSpace()
				&& checkFlags( flags.textures, flag ) != flags.textures.flags.end()
				&& ( checkFlag( flags.m_passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( flags.m_passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) );
		}

		static bool enableParallaxOcclusionMappingOne( PipelineFlags const & flags
			, PassComponentTextureFlag const & flag )
		{
			return flags.enableTangentSpace()
				&& checkFlags( flags.textures, flag ) != flags.textures.flags.end()
				&& checkFlag( flags.m_passFlags, PassFlag::eParallaxOcclusionMappingOne );
		}
	}

	//*********************************************************************************************

	void HeightMapComponent::ComponentsShader::computeTexcoord( PipelineFlags const & flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, shader::BlendComponents & components )const
	{
		if ( hgtcmp::enableParallaxOcclusionMapping( flags, getTextureFlags() )
			&& components.hasMember( "tangentSpaceViewPosition" )
			&& components.hasMember( "tangentSpaceFragPosition" ) )
		{
			auto & writer = findWriterMandat( config, imgCompConfig, map, texCoords, texCoord, components );

			IF( writer, imgCompConfig.y() )
			{
				auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
				auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
				parallaxMapping( texCoord
					, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
					, map
					, config
					, imgCompConfig );
				config.setUv( texCoords, texCoord );

				if ( hgtcmp::enableParallaxOcclusionMappingOne( flags, getTextureFlags() ) )
				{
					IF( writer, config.getUv( texCoords ).x() > 1.0_f
						|| config.getUv( texCoords ).y() > 1.0_f
						|| config.getUv( texCoords ).x() < 0.0_f
						|| config.getUv( texCoords ).y() < 0.0_f )
					{
						writer.demote();
					}
					FI;
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
			FI;
		}
	}

	void HeightMapComponent::ComponentsShader::computeTexcoord( PipelineFlags const & flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, shader::DerivTex & texCoords
		, shader::DerivTex & texCoord
		, shader::BlendComponents & components )const
	{
		if ( hgtcmp::enableParallaxOcclusionMapping( flags, getTextureFlags() )
			&& components.hasMember( "tangentSpaceViewPosition" )
			&& components.hasMember( "tangentSpaceFragPosition" ) )
		{
			auto & writer = findWriterMandat( config, imgCompConfig, map, texCoords, texCoord, components );

			IF( writer, imgCompConfig.y() )
			{
				auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
				auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
				parallaxMapping( texCoord
					, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
					, map
					, config
					, imgCompConfig );
				config.setUv( texCoords, texCoord );

				if ( hgtcmp::enableParallaxOcclusionMappingOne( flags, getTextureFlags() ) )
				{
					IF( writer, config.getUv( texCoords ).x() > 1.0_f
						|| config.getUv( texCoords ).y() > 1.0_f
						|| config.getUv( texCoords ).x() < 0.0_f
						|| config.getUv( texCoords ).y() < 0.0_f )
					{
						writer.demote();
					}
					FI;
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
			FI;
		}
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( sdw::Vec2 & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, shader::TextureConfigData const & textureConfig
		, sdw::U32Vec3 const & imgCompConfig )const
	{
		return parallaxMapping( texCoords
			, dFdxCoarse( texCoords )
			, dFdyCoarse( texCoords )
			, viewDir
			, heightMap
			, textureConfig
			, imgCompConfig );
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( shader::DerivTex & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, shader::TextureConfigData const & textureConfig
		, sdw::U32Vec3 const & imgCompConfig )const
	{
		parallaxMapping( texCoords.uv()
			, texCoords.dPdx()
			, texCoords.dPdy()
			, viewDir
			, heightMap
			, textureConfig
			, imgCompConfig );
	}

	void HeightMapComponent::ComponentsShader::parallaxMapping( sdw::Vec2 ptexCoords
		, sdw::Vec2 const & pdx
		, sdw::Vec2 const & pdy
		, sdw::Vec3 const & pviewDir
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, shader::TextureConfigData const & ptextureConfig
		, sdw::U32Vec3 const & pimgCompConfig )const
	{
		if ( !m_parallaxMapping )
		{
			auto & writer = findWriterMandat( ptexCoords
				, pdx
				, pdy
				, pviewDir
				, pheightMap
				, ptextureConfig
				, pimgCompConfig );
			m_parallaxMapping = writer.implementFunction< sdw::Vec2 >( "c3d_parallaxMapping",
				[&]( sdw::Vec2 const & texCoords
					, sdw::Vec2 const & dx
					, sdw::Vec2 const & dy
					, sdw::Vec3 const & viewDir
					, sdw::CombinedImage2DRgba32 const & heightMap
					, shader::TextureConfigData const & textureConfig
					, sdw::U32Vec3 const & imgCompConfig )
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
						, imgCompConfig.z() );
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
					ELIHW;

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
				, sdw::InU32Vec3{ writer, "imgCompConfig" } );
		}

		ptexCoords = m_parallaxMapping( ptexCoords
			, pdx
			, pdy
			, pviewDir
			, pheightMap
			, ptextureConfig
			, pimgCompConfig );
	}

	sdw::RetFloat HeightMapComponent::ComponentsShader::parallaxShadow( sdw::Vec3 const & plightDir
		, sdw::Vec2 const & pinitialTexCoord
		, sdw::Float const & pinitialHeight
		, sdw::CombinedImage2DRgba32 const & pheightMap
		, shader::TextureConfigData const & ptextureConfig
		, sdw::U32Vec3 const & pimgCompConfig )
	{
		if ( !m_parallaxShadow )
		{
			auto & writer = findWriterMandat( plightDir
				, pinitialTexCoord
				, pinitialHeight
				, pheightMap
				, ptextureConfig
				, pimgCompConfig );
			m_parallaxShadow = writer.implementFunction< sdw::Float >( "c3d_parallaxSoftShadowMultiplier"
				, [&]( sdw::Vec3 const & lightDir
					, sdw::Vec2 const & initialTexCoord
					, sdw::Float const & initialHeight
					, sdw::CombinedImage2DRgba32 const & heightMap
					, shader::TextureConfigData const & textureConfig
					, sdw::U32Vec3 const & imgCompConfig )
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
							, writer.cast< sdw::UInt >( imgCompConfig.z() ) );
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
							FI;

							// offset to the next layer
							stepIndex += 1_i;
							currentLayerHeight -= layerHeight;
							currentTextureCoords += texStep;
							sampled = heightMap.sample( currentTextureCoords );
							heightFromTexture = sampled[heightIndex];
						}
						ELIHW;

						// Shadowing factor should be 1 if there were no points under the surface
						IF( writer, numSamplesUnderSurface < 1.0_f )
						{
							shadowMultiplier = 1.0_f;
						}
						ELSE
						{
							shadowMultiplier = 1.0_f - shadowMultiplier;
						}
						FI;
					}
					FI;

					writer.returnStmt( shadowMultiplier );
				}
				, sdw::InVec3{ writer, "lightDir" }
				, sdw::InVec2{ writer, "initialTexCoord" }
				, sdw::InFloat{ writer, "initialHeight" }
				, sdw::InCombinedImage2DRgba32{ writer, "heightMap" }
				, shader::InTextureConfigData{ writer, "textureConfig" }
				, sdw::InU32Vec3{ writer, "imgCompConfig" } );
		}

		return m_parallaxShadow( plightDir
			, pinitialTexCoord
			, pinitialHeight
			, pheightMap
			, ptextureConfig
			, pimgCompConfig );
	}

	//*********************************************************************************************

	void HeightMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		static UInt32StrMap const parallaxOcclusionModes{ getEnumMapT< ParallaxOcclusionMode >() };

		channelFillers.emplace( "height", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< HeightMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_mask" )
			, hgtcmp::parserUnitHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_factor" )
			, hgtcmp::parserUnitHeightFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "height" )
			, hgtcmp::parserTexRemapHeight );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "height_mask" )
			, hgtcmp::parserTexRemapHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool HeightMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< HeightMapComponent >{ tabs, configuration }( file, it->componentsMask );
	}

	bool HeightMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry )
			|| textures.flags.end() != checkFlags( textures, getTextureFlags() );
	}

	void HeightMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< HeightMapComponent >( pass ) );
	}

	bool HeightMapComponent::Plugin::hasTexcoordModif( PipelineFlags const * flags )const
	{
		return flags
			? hgtcmp::enableParallaxOcclusionMapping( *flags, getTextureFlags() )
			: false;
	}

	//*********************************************************************************************

	castor::String const HeightMapComponent::TypeName = C3D_MakePassMapComponentName( "height" );

	HeightMapComponent::HeightMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< HeightComponent >() )
		{
			pass.createComponent< HeightComponent >();
		}
	}

	void HeightMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Height" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
		vis.visit( cuT( "Height factor" ), configuration.heightFactor );
	}

	PassComponentUPtr HeightMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< HeightMapComponent >( pass );
	}

	//*********************************************************************************************
}
