#include "Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< OpacityMapComponent >
		: public TextWriterT< OpacityMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< OpacityMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( OpacityMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "opacity_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "opacity_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace opacmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitOpacityMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( OpacityMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( OpacityMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapOpacity, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( OpacityMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapOpacityMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( OpacityMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OpacityMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "opacity" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void OpacityMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "opacity" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< OpacityMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0xFF000000 );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "opacity_mask" )
			, opacmp::parserUnitOpacityMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "opacity_mask" )
			, opacmp::parserUnitOpacityMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "opacity" )
			, opacmp::parserTexRemapOpacity );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "opacity_mask" )
			, opacmp::parserTexRemapOpacityMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool OpacityMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| hasAny( textures, getTextureFlags() );
	}

	void OpacityMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, OpacityMapComponent >( pass ) );
	}

	bool OpacityMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< OpacityMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const OpacityMapComponent::TypeName = C3D_MakePassMapComponentName( "opacity" );

	OpacityMapComponent::OpacityMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Opacity
			, { OpacityComponent::TypeName } }
	{
	}

	PassComponentUPtr OpacityMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, OpacityMapComponent >( pass );
	}

	void OpacityMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Opacity" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
