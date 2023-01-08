#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::AlphaTestComponent >
		: public TextWriterT< castor3d::AlphaTestComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::AlphaTestComponent >{ tabs }
		{
		}

		bool operator()( castor3d::AlphaTestComponent const & object
			, StringStream & file )override
		{
			static std::map< VkCompareOp, String > alphaFuncs
			{
				{ VK_COMPARE_OP_ALWAYS, cuT( "always" ) },
				{ VK_COMPARE_OP_LESS, cuT( "less" ) },
				{ VK_COMPARE_OP_LESS_OR_EQUAL, cuT( "less_equal" ) },
				{ VK_COMPARE_OP_EQUAL, cuT( "equal" ) },
				{ VK_COMPARE_OP_NOT_EQUAL, cuT( "not_equal" ) },
				{ VK_COMPARE_OP_GREATER_OR_EQUAL, cuT( "greater_equal" ) },
				{ VK_COMPARE_OP_GREATER, cuT( "greater" ) },
				{ VK_COMPARE_OP_NEVER, cuT( "never" ) },
			};
			bool result = true;

			if ( object.hasAlphaTest() )
			{
				result = write( file, cuT( "alpha_func" ), alphaFuncs[object.getAlphaFunc()], object.getAlphaRefValue() );
			}

			if ( result && object.hasBlendAlphaTest() )
			{
				result = write( file, cuT( "blend_alpha_func" ), alphaFuncs[object.getBlendAlphaFunc()], object.getAlphaRefValue() );
			}

			return result;
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace alptst
	{
		static CU_ImplementAttributeParser( parserPassAlphaFunc )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				auto & component = getPassComponent< AlphaTestComponent >( parsingContext );
				component.setAlphaFunc( VkCompareOp( uiFunc ) );
				component.setAlphaRefValue( fFloat );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassBlendAlphaFunc )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				auto & component = getPassComponent< AlphaTestComponent >( parsingContext );
				component.setBlendAlphaFunc( VkCompareOp( uiFunc ) );
				component.setAlphaRefValue( fFloat );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AlphaTestComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
		{
			return;
		}

		if ( !components.hasMember( "alphaRef" ) )
		{
			components.declMember( "alphaRef", sdw::type::Kind::eFloat );
		}
	}

	void AlphaTestComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "alphaRef" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "alphaRef" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.95_f ) );
		}
	}

	void AlphaTestComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "alphaRef" ) )
		{
			res.getMember< sdw::Float >( "alphaRef" ) += src.getMember< sdw::Float >( "alphaRef" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	AlphaTestComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void AlphaTestComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "alphaRef" ) )
		{
			type.declMember( "alphaRef", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 0.95_f ) );
		}
	}

	//*********************************************************************************************

	void AlphaTestComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha_func" )
			, alptst::parserPassAlphaFunc
			, { castor::makeParameter< castor::ParameterType::eCheckedText, VkCompareOp >(), castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "blend_alpha_func" )
			, alptst::parserPassBlendAlphaFunc
			, { castor::makeParameter< castor::ParameterType::eCheckedText, VkCompareOp >(), castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void AlphaTestComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0.95f, 0u );
	}

	bool AlphaTestComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	castor::String const AlphaTestComponent::TypeName = C3D_MakePassOtherComponentName( "alpha_test" );

	AlphaTestComponent::AlphaTestComponent( Pass & pass )
		: BaseDataPassComponentT< AlphaTestData >{ pass, TypeName }
	{
	}

	void AlphaTestComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Alpha func" ), m_value.alphaFunc );
		vis.visit( cuT( "Blend alpha func" ), m_value.blendAlphaFunc );
		vis.visit( cuT( "Alpha ref. value" ), m_value.alphaRefValue );
	}

	PassComponentUPtr AlphaTestComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< AlphaTestComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool AlphaTestComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< AlphaTestComponent >{ tabs }( *this, file );
	}

	void AlphaTestComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getAlphaRefValue(), 0u );
	}

	//*********************************************************************************************
}
