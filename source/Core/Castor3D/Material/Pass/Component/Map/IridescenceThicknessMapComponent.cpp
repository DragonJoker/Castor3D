#include "Castor3D/Material/Pass/Component/Map/IridescenceThicknessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

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

		bool operator()( StringStream & file )
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
		static CU_ImplementAttributeParser( parserUnitIridescenceThicknessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( IridescenceThicknessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapIridescenceThickness )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( IridescenceThicknessMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapIridescenceThicknessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( IridescenceThicknessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void IridescenceThicknessMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember(  "iridescenceFactor" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto iridescenceMinThickness = components.getMember< sdw::Float >( "iridescenceMinThickness" );
			auto iridescenceMaxThickness = components.getMember< sdw::Float >( "iridescenceMaxThickness" );
			auto iridescenceThickness = components.getMember< sdw::Float >( "iridescenceThickness" );
			iridescenceThickness = mix( iridescenceMinThickness
				, iridescenceMaxThickness
				, config.getFloat( sampled, imgCompConfig.z() ) );
		}
		FI;
	}

	//*********************************************************************************************

	void IridescenceThicknessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "iridescence_thickness", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< IridescenceThicknessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x0000FF00u );
			} } );

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
