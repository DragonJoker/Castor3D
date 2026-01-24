#include "Castor3D/Material/Pass/Component/Map/IridescenceMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< IridescenceMapComponent >
		: public TextWriterT< IridescenceMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< IridescenceMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "iridescence_mask" ), m_mask );
		}

		bool operator()( IridescenceMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitIridescenceMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( IridescenceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapIridescence, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapIridescenceMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void IridescenceMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "iridescenceFactor" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void IridescenceMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "iridescence" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< IridescenceMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000u );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "iridescence_mask" )
			, trscmp::parserUnitIridescenceMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "iridescence_mask" )
			, trscmp::parserUnitIridescenceMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "iridescence" )
			, trscmp::parserTexRemapIridescence );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "iridescence_mask" )
			, trscmp::parserTexRemapIridescenceMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool IridescenceMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void IridescenceMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, IridescenceMapComponent >( pass ) );
	}

	bool IridescenceMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< IridescenceMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const IridescenceMapComponent::TypeName = C3D_MakePassMapComponentName( "iridescence" );

	IridescenceMapComponent::IridescenceMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Iridescence
			, { IridescenceComponent::TypeName } }
	{
	}

	PassComponentUPtr IridescenceMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, IridescenceMapComponent >( pass );
	}

	void IridescenceMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Iridescence" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
