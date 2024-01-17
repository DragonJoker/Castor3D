#include "Castor3D/Material/Pass/Component/Lighting/ThicknessComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, ThicknessComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::ThicknessComponent >
		: public TextWriterT< castor3d::ThicknessComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::ThicknessComponent >{ tabs }
		{
		}

		bool operator()( castor3d::ThicknessComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "thickness_factor" ), object.getThicknessFactor(), 0.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trsatt
	{
		static CU_ImplementAttributeParserBlock( parserPassThicknessFactor, PassContext )
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
				auto & component = getPassComponent< ThicknessComponent >( *blockContext );
				component.setThicknessFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ThicknessComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "thicknessFactor" ) )
		{
			components.declMember( "thicknessFactor", sdw::type::Kind::eFloat );
		}
	}

	void ThicknessComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "thicknessFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "thicknessFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void ThicknessComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "thicknessFactor" ) )
		{
			return;
		}

		res.getMember< sdw::Float >( "thicknessFactor", true ) += src.getMember< sdw::Float >( "thicknessFactor", true ) * passMultiplier;
	}

	//*********************************************************************************************

	ThicknessComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void ThicknessComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "thicknessFactor" ) )
		{
			type.declMember( "thicknessFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void ThicknessComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "thickness_factor" )
			, trsatt::parserPassThicknessFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void ThicknessComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0.0f, 0u );
	}

	bool ThicknessComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const ThicknessComponent::TypeName = C3D_MakePassLightingComponentName( "thickness" );

	ThicknessComponent::ThicknessComponent( Pass & pass )
		: BaseDataPassComponentT{ pass
			, TypeName
			, { AttenuationComponent::TypeName } }
	{
	}

	void ThicknessComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Thickness" ) );
		vis.visit( cuT( "Factor" ), m_value );
	}

	PassComponentUPtr ThicknessComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< ThicknessComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool ThicknessComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ThicknessComponent >{ tabs }( *this, file );
	}

	void ThicknessComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, getThicknessFactor()
			, 0u );
	}

	//*********************************************************************************************
}
