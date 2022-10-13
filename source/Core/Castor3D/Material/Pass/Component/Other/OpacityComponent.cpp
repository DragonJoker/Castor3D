#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OpacityComponent >
		: public TextWriterT< castor3d::OpacityComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::OpacityComponent >{ tabs }
		{
		}

		bool operator()( castor3d::OpacityComponent const & object
			, StringStream & file )override
		{
			bool result = true;

			if ( object.getOpacity() < 1 )
			{
				result = write( file, cuT( "alpha" ), object.getOpacity() )
					&& write( file, cuT( "bw_accumulation" ), object.getBWAccumulationOperator() );
			}

			return result;
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace opacmp
	{
		static CU_ImplementAttributeParser( parserPassAlpha )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< OpacityComponent >( parsingContext );
				component.setOpacity( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassMixedInterpolative )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );

				if ( value )
				{
					auto & alphaTest = getPassComponent< AlphaTestComponent >( parsingContext );
					alphaTest.setAlphaRefValue( 0.95f );
					alphaTest.setAlphaFunc( VK_COMPARE_OP_GREATER );
					alphaTest.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );

					auto & blend = getPassComponent< BlendComponent >( parsingContext );
					blend.setAlphaBlendMode( BlendMode::eInterpolative );

					getPassComponent< OpacityComponent >( parsingContext );
				}

			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassBWAccumulationOperator )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value = 0u;
				params[0]->get( value );
				auto & component = getPassComponent< OpacityComponent >( parsingContext );
				component.setBWAccumulationOperator( uint8_t( value ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OpacityComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
		{
			return;
		}

		if ( !components.hasMember( "opacity" ) )
		{
			components.declMember( "opacity", sdw::type::Kind::eFloat );
			components.declMember( "bwAccumulation", sdw::type::Kind::eUInt );
		}
	}

	void OpacityComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "opacity" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "opacity" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt >( "bwAccumulation" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void OpacityComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "opacity" ) )
		{
			res.getMember< sdw::Float >( "opacity" ) += src.getMember< sdw::Float >( "opacity" ) * passMultiplier;
			res.getMember< sdw::UInt >( "bwAccumulation" ) = max( res.getMember< sdw::UInt >( "bwAccumulation" )
				, src.getMember< sdw::UInt >( "bwAccumulation" ) );
		}
	}

	//*********************************************************************************************

	OpacityComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void OpacityComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "opacity" ) )
		{
			type.declMember( "opacity", ast::type::Kind::eFloat );
			type.declMember( "bwAccumulation", ast::type::Kind::eUInt );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void OpacityComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha" )
			, opacmp::parserPassAlpha
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "mixed_interpolation" )
			, opacmp::parserPassMixedInterpolative
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "bw_accumulation" )
			, opacmp::parserPassBWAccumulationOperator
			, { castor::makeParameter< castor::ParameterType::eUInt32 >( castor::makeRange( 0u, 8u ) ) } );
	}

	void OpacityComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), 1.0f, offset );
		data.write( materialShader.getMaterialChunk(), 0u, offset );
	}

	bool OpacityComponent::Plugin::isComponentNeeded( TextureFlagsArray const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	castor::String const OpacityComponent::TypeName = C3D_MakePassOtherComponentName( "opacity" );

	OpacityComponent::OpacityComponent( Pass & pass )
		: BaseDataPassComponentT< OpacityData >{ pass, TypeName }
	{
	}

	void OpacityComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Opacity" ), m_value.opacity );
		vis.visit( cuT( "Blended weighted accumulator" ), m_value.bwAccumulationOperator );
	}

	void OpacityComponent::setOpacity( float v )
	{
		m_value.opacity = v;
	}

	PassComponentUPtr OpacityComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< OpacityComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool OpacityComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< OpacityComponent >{ tabs }( *this, file );
	}

	void OpacityComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getOpacity(), offset );
		data.write( m_materialShader->getMaterialChunk(), getBWAccumulationOperator(), offset );
	}

	//*********************************************************************************************
}
