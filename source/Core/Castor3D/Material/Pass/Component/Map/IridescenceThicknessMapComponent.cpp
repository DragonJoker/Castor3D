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

CU_ImplementSmartPtr( castor3d, IridescenceThicknessMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::IridescenceThicknessMapComponent >
		: public TextWriterT< castor3d::IridescenceThicknessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::IridescenceThicknessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "iridescence_thickness_mask" ), m_mask );
		}

		bool operator()( castor3d::IridescenceThicknessMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
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
		std::string valueName = "iridescenceThickness";
		std::string mapName = "iridescenceThickness";
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
		auto iridescenceMinThickness = components.getMember< sdw::Float >( "iridescenceMinThickness" );
		auto iridescenceMaxThickness = components.getMember< sdw::Float >( "iridescenceMaxThickness" );
		auto iridescenceThickness = components.getMember< sdw::Float >( valueName );

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
		iridescenceThickness = mix( iridescenceMinThickness
			, iridescenceMaxThickness
			, shader::TextureConfigData::getFloat( sampled, mask ) );
	}

	//*********************************************************************************************

	void IridescenceThicknessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( "iridescence_thickness"
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< IridescenceThicknessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x0000FF00u );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserUnitIridescenceThicknessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserUnitIridescenceThicknessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "iridescence_thickness" )
			, trscmp::parserTexRemapIridescenceThickness );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "iridescence_thickness_mask" )
			, trscmp::parserTexRemapIridescenceThicknessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool IridescenceThicknessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void IridescenceThicknessMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, IridescenceThicknessMapComponent >( pass ) );
	}

	bool IridescenceThicknessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< IridescenceThicknessMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const IridescenceThicknessMapComponent::TypeName = C3D_MakePassMapComponentName( "iridescence_thickness" );

	IridescenceThicknessMapComponent::IridescenceThicknessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, IridescenceThickness
			, { IridescenceComponent::TypeName } }
	{
	}

	PassComponentUPtr IridescenceThicknessMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, IridescenceThicknessMapComponent >( pass );
	}

	void IridescenceThicknessMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Iridescence Thickness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
