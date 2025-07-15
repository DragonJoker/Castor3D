#include "Castor3D/Material/Pass/Component/Map/IridescenceThicknessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< IridescenceThicknessMapComponent >
		: public TextWriterT< IridescenceThicknessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< IridescenceThicknessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "iridescence_thickness_mask" ), m_mask );
		}

		bool operator()( IridescenceThicknessMapComponent const & object
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
		static CU_ImplementAttributeParserBlock( parserUnitIridescenceThicknessMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceThicknessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( IridescenceThicknessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapIridescenceThickness, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceThicknessMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapIridescenceThicknessMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( IridescenceThicknessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void IridescenceThicknessMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		if ( !material.hasMember( "iridescenceThickness" )
			|| !components.hasMember( "iridescenceThickness" ) )
		{
			return;
		}

		auto iridescenceMinThickness = components.getMember< sdw::Float >( "iridescenceMinThickness" );
		auto iridescenceMaxThickness = components.getMember< sdw::Float >( "iridescenceMaxThickness" );
		components.iridescenceThickness = mix( iridescenceMinThickness
			, iridescenceMaxThickness
			, loadFloatComponent( cuT( "iridescenceThickness" )
				, passShaders, textureConfigs, textureAnims
				, material, components, sampleTexture ) );
	}

	//*********************************************************************************************

	void IridescenceThicknessMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "iridescence_thickness" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< IridescenceThicknessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x0000FF00u );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserUnitIridescenceThicknessMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserUnitIridescenceThicknessMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "iridescence_thickness" )
			, trscmp::parserTexRemapIridescenceThickness );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserTexRemapIridescenceThicknessMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool IridescenceThicknessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void IridescenceThicknessMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, IridescenceThicknessMapComponent >( pass ) );
	}

	bool IridescenceThicknessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< IridescenceThicknessMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const IridescenceThicknessMapComponent::TypeName = C3D_MakePassMapComponentName( "iridescence_thickness" );

	IridescenceThicknessMapComponent::IridescenceThicknessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, IridescenceThickness
			, { IridescenceComponent::TypeName } }
	{
	}

	PassComponentUPtr IridescenceThicknessMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, IridescenceThicknessMapComponent >( pass );
	}

	void IridescenceThicknessMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Iridescence Thickness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
