#include "Castor3D/Material/Pass/Component/Lighting/SheenComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< SheenComponent >
		: public TextWriterT< SheenComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SheenComponent >{ tabs }
		{
		}

		bool operator()( SheenComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "sheen_colour" ), object.getSheenColour(), SheenComponent::DefaultFactor )
				&& writeOpt( file, cuT( "sheen_roughness" ), object.getRoughnessFactor(), SheenComponent::DefaultRoughness );
		}
	};

	//*********************************************************************************************

	namespace sheen
	{
		static CU_ImplementAttributeParserBlock( parserPassSheenFactor, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< SheenComponent >( *blockContext );
				component.setSheenColour( params[0]->get< HdrRgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassRoughnessFactor, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< SheenComponent >( *blockContext );
				component.setRoughnessFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	SheenComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void SheenComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "sheenColour" ) )
		{
			type.declMember( "sheenColour", ast::type::Kind::eVec3F );
			type.declMember( "sheenRoughness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ SheenComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ SheenComponent::DefaultRoughness } ) );
		}
	}

	//*********************************************************************************************

	void SheenComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "sheenColour" ) )
		{
			components.declMember( "sheenColour", sdw::type::Kind::eVec3F );
			components.declMember( "sheenRoughness", sdw::type::Kind::eFloat );
		}
	}

	void SheenComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "sheenColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "sheenColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "sheenRoughness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ SheenComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ SheenComponent::DefaultRoughness } ) );
		}
	}

	void SheenComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "sheenColour" ) )
		{
			res.getMember< sdw::Vec3 >( "sheenColour" ) += src.getMember< sdw::Vec3 >( "sheenColour", true ) * passMultiplier;
			res.getMember< sdw::Float >( "sheenRoughness" ) += src.getMember< sdw::Float >( "sheenRoughness", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void SheenComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "sheen_colour" )
			, sheen::parserPassSheenFactor
			, { makeParameter< ParameterType::eHdrRgbColour >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "sheen_roughness" )
			, sheen::parserPassRoughnessFactor
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void SheenComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), SheenComponent::DefaultFactor, offset );
		data.write( materialShader.getMaterialChunk(), SheenComponent::DefaultRoughness, offset );
	}

	bool SheenComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const SheenComponent::TypeName = C3D_MakePassLightingComponentName( "sheen" );

	SheenComponent::SheenComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, SheenComponent::DefaultFactor, SheenComponent::DefaultRoughness }
	{
	}

	void SheenComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Sheen" ) );
		vis.visit( cuT( "Colour" ), m_value.colour );
		vis.visit( cuT( "Roughness" ), m_value.roughness );
	}

	PassComponentUPtr SheenComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< SheenComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool SheenComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< SheenComponent >{ tabs }( *this, file );
	}

	void SheenComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getSheenColour()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getRoughnessFactor()
			, offset );
	}

	//*********************************************************************************************
}
