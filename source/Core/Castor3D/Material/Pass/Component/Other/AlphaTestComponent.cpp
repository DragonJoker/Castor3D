#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< AlphaTestComponent >
		: public TextWriterT< AlphaTestComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< AlphaTestComponent >{ tabs }
		{
		}

		bool operator()( AlphaTestComponent const & object
			, StringStream & file )override
		{
			static Map< ComparisonFunc, String > alphaFuncs
			{
				{ ComparisonFunc ::eAlways, cuT( "always" ) },
				{ ComparisonFunc ::eLess, cuT( "less" ) },
				{ ComparisonFunc ::eLessOrEqual, cuT( "less_equal" ) },
				{ ComparisonFunc ::eEqual, cuT( "equal" ) },
				{ ComparisonFunc ::eNotEqual, cuT( "not_equal" ) },
				{ ComparisonFunc ::eGreaterOrEqual, cuT( "greater_equal" ) },
				{ ComparisonFunc ::eGreater, cuT( "greater" ) },
				{ ComparisonFunc ::eNever, cuT( "never" ) },
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

	//*********************************************************************************************

	namespace alptst
	{
		static CU_ImplementAttributeParserBlock( parserPassAlphaFunc, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				auto & component = getPassComponent< AlphaTestComponent >( *blockContext );
				component.setAlphaFunc( ComparisonFunc( uiFunc ) );
				component.setAlphaRefValue( fFloat );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassBlendAlphaFunc, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t uiFunc;
				float fFloat;
				params[0]->get( uiFunc );
				params[1]->get( fFloat );
				auto & component = getPassComponent< AlphaTestComponent >( *blockContext );
				component.setBlendAlphaFunc( ComparisonFunc( uiFunc ) );
				component.setAlphaRefValue( fFloat );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AlphaTestComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eOpacity )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
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
		if ( res.hasMember( "alphaRef" ) )
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

	void AlphaTestComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha_func" )
			, alptst::parserPassAlphaFunc
			, { makeParameter< ParameterType::eCheckedText, ComparisonFunc >(), makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "blend_alpha_func" )
			, alptst::parserPassBlendAlphaFunc
			, { makeParameter< ParameterType::eCheckedText, ComparisonFunc >(), makeParameter< ParameterType::eFloat >() } );
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

	String const AlphaTestComponent::TypeName = C3D_MakePassOtherComponentName( "alpha_test" );

	AlphaTestComponent::AlphaTestComponent( Pass & pass )
		: BaseDataPassComponentT< AlphaTestData >{ pass, TypeName }
	{
	}

	void AlphaTestComponent::accept( ConfigurationVisitorBase & vis )
	{
		static StringArray names{ cuT( "Never" )
			, cuT( "Less" )
			, cuT( "Equal" )
			, cuT( "Less Or Equal" )
			, cuT( "Greater" )
			, cuT( "Not Equal" )
			, cuT( "Greater Or Equal" )
			, cuT( "Always" ) };
		vis.visit( cuT( "Alpha Test" ) );
		vis.visit( cuT( "Alpha func" )
			, m_value.alphaFunc
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< ComparisonFunc >( [this]( ComparisonFunc, ComparisonFunc newV )
			{
				m_value.alphaFunc = newV;
			} ) );
		vis.visit( cuT( "Blend alpha func" )
			, m_value.blendAlphaFunc
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< ComparisonFunc >( [this]( ComparisonFunc, ComparisonFunc newV )
			{
				m_value.blendAlphaFunc = newV;
			} ) );
		vis.visit( cuT( "Alpha ref. value" ), m_value.alphaRefValue );
	}

	PassComponentUPtr AlphaTestComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< AlphaTestComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool AlphaTestComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< AlphaTestComponent >{ tabs }( *this, file );
	}

	void AlphaTestComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getAlphaRefValue(), 0u );
	}

	//*********************************************************************************************
}
