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

CU_ImplementSmartPtr( castor3d, SheenComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::SheenComponent >
		: public TextWriterT< castor3d::SheenComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::SheenComponent >{ tabs }
		{
		}

		bool operator()( castor3d::SheenComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "sheen_colour" ), object.getSheenFactor() )
				&& writeOpt( file, cuT( "sheen_roughness" ), object.getRoughnessFactor(), 0.0f );
		}
	};
}

namespace castor3d
{
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
				component.setSheenFactor( params[0]->get< castor::HdrRgbColour >() );
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

		if ( !components.hasMember( "sheenFactor" ) )
		{
			components.declMember( "sheenFactor", sdw::type::Kind::eVec3F );
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
		if ( !components.hasMember( "sheenFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "sheenFactor" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "sheenRoughness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void SheenComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "sheenFactor" ) )
		{
			return;
		}

		res.getMember< sdw::Vec3 >( "sheenFactor", true ) += src.getMember< sdw::Vec3 >( "sheenFactor", true ) * passMultiplier;
		res.getMember< sdw::Float >( "sheenRoughness", true ) += src.getMember< sdw::Float >( "sheenRoughness", true ) * passMultiplier;
	}

	//*********************************************************************************************

	SheenComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void SheenComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "sheenFactor" ) )
		{
			type.declMember( "sheenFactor", ast::type::Kind::eVec3F );
			type.declMember( "sheenRoughness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void SheenComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "sheen_colour" )
			, sheen::parserPassSheenFactor
			, { castor::makeParameter< castor::ParameterType::eHdrRgbColour >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "sheen_roughness" )
			, sheen::parserPassRoughnessFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void SheenComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		data.write( materialShader.getMaterialChunk(), 0.0f, offset );
	}

	bool SheenComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const SheenComponent::TypeName = C3D_MakePassLightingComponentName( "sheen" );

	SheenComponent::SheenComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName }
	{
	}

	void SheenComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Sheen" ) );
		vis.visit( cuT( "Colour" ), m_value.factor );
		vis.visit( cuT( "Roughness" ), m_value.roughness );
	}

	PassComponentUPtr SheenComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< SheenComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool SheenComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SheenComponent >{ tabs }( *this, file );
	}

	void SheenComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getSheenFactor()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getRoughnessFactor()
			, offset );
	}

	//*********************************************************************************************
}
