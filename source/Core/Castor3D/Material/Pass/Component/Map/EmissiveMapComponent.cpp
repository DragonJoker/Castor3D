#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::EmissiveMapComponent >
		: public TextWriterT< castor3d::EmissiveMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::EmissiveMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::EmissiveMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "emissive_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "emissive_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParser( parserUnitEmissiveMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< EmissiveMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapEmissive )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eEmissive, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapEmissiveMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				EmissiveMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EmissiveMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "emissive" )
			|| !checkFlag( texturesFlags, TextureFlag::eEmissive ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.emsEnbl() != 0.0_f )
		{
			components.getMember< sdw::Vec3 >( "emissive" ) *= config.getVec3( sampled, config.emsMask() );
		}
		FI;
	}

	//*********************************************************************************************

	castor::String const EmissiveMapComponent::TypeName = C3D_MakePassMapComponentName( "emissive" );

	EmissiveMapComponent::EmissiveMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< EmissiveComponent >() )
		{
			pass.createComponent< EmissiveComponent >();
		}
	}

	void EmissiveMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "emissive", ChannelFiller{ uint32_t( TextureFlag::eEmissive )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< EmissiveMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "emissive_mask" )
			, emscmp::parserUnitEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "emissive" )
			, emscmp::parserTexRemapEmissive );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "emissive_mask" )
			, emscmp::parserTexRemapEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	void EmissiveMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.emissiveMask[0] = maskValue;
	}

	bool EmissiveMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< EmissiveMapComponent >{ tabs, configuration.emissiveMask[0] }( file );
	}

	bool EmissiveMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( textures, TextureFlag::eEmissive );
	}

	bool EmissiveMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.emissiveMask[0] != 0u;
	}

	void EmissiveMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< EmissiveMapComponent >( pass ) );
	}

	void EmissiveMapComponent::updateComponent( TextureFlags const & texturesFlags
		, shader::BlendComponents const & components )
	{
		if ( !checkFlag( texturesFlags, TextureFlag::eEmissive ) )
		{
			components.getMember< sdw::Vec3 >( "emissive", true ) *= components.getMember< sdw::Vec3 >( "colour", true );
		}
	}

	void EmissiveMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		getOwner()->mergeImages( TextureFlag::eEmissive
			, offsetof( TextureConfiguration, emissiveMask )
			, 0x00FFFFFF
			, TextureFlag::eOcclusion
			, offsetof( TextureConfiguration, occlusionMask )
			, 0xFF000000
			, "EmsOcc"
			, result );
	}

	void EmissiveMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eColour;
		configuration.textureSpace |= TextureSpace::eAllowSRGB;
		configuration.emissiveMask[0] = mask;
	}

	void EmissiveMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Emissive" ), TextureFlag::eEmissive, configuration.emissiveMask, 3u );
	}

	PassComponentUPtr EmissiveMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< EmissiveMapComponent >( pass );
	}

	//*********************************************************************************************
}
