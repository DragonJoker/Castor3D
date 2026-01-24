#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	template<>
	class TextWriter< OpacityComponent >
		: public TextWriterT< OpacityComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< OpacityComponent >{ tabs }
		{
		}

		bool operator()( OpacityComponent const & object
			, StringStream & file )override
		{
			bool result = true;

			if ( object.getOpacity() < 1 )
			{
				result = writeOpt( file, cuT( "opacity" ), object.getOpacity(), OpacityComponent::DefaultOpacity )
					&& writeOpt( file, cuT( "bw_accumulation" ), object.getBWAccumulationOperator(), OpacityComponent::DefaultBwAccumulationOperator );
			}

			return result;
		}
	};

	//*********************************************************************************************

	namespace opacmp
	{
		static CU_ImplementAttributeParserBlock( parserPassAlpha, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< OpacityComponent >( *blockContext );
				component.setOpacity( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassMixedInterpolative, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );

				if ( value )
				{
					auto & alphaTest = getPassComponent< AlphaTestComponent >( *blockContext );
					alphaTest.setAlphaRefValue( 0.95f );
					alphaTest.setAlphaFunc( ComparisonFunc::eGreater );
					alphaTest.setBlendAlphaFunc( ComparisonFunc::eLessOrEqual );

					auto & blend = getPassComponent< BlendComponent >( *blockContext );
					blend.setAlphaBlendMode( BlendMode::eInterpolative );

					getPassComponent< OpacityComponent >( *blockContext );
				}

			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassBWAccumulationOperator, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value = 0u;
				params[0]->get( value );
				auto & component = getPassComponent< OpacityComponent >( *blockContext );
				component.setBWAccumulationOperator( uint8_t( value ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OpacityComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eOpacity )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
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
		, sdw::Vec4 const * clrCot
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
			inits.emplace_back( sdw::makeExpr( sdw::Float{ OpacityComponent::DefaultOpacity } ) );
			inits.emplace_back( sdw::makeExpr( sdw::UInt{ OpacityComponent::DefaultBwAccumulationOperator } ) );
		}
	}

	void OpacityComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "opacity" ) )
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
			inits.emplace_back( sdw::makeExpr( sdw::Float{ OpacityComponent::DefaultOpacity } ) );
			inits.emplace_back( sdw::makeExpr( sdw::UInt{ OpacityComponent::DefaultBwAccumulationOperator } ) );
		}
	}

	//*********************************************************************************************

	void OpacityComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha" )
			, opacmp::parserPassAlpha
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "opacity" )
			, opacmp::parserPassAlpha
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "mixed_interpolation" )
			, opacmp::parserPassMixedInterpolative
			, { makeParameter< ParameterType::eBool >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "bw_accumulation" )
			, opacmp::parserPassBWAccumulationOperator
			, { makeParameter< ParameterType::eUInt32 >( makeRange( MinBwAccumulationOperator, MaxBwAccumulationOperator ) ) } );
	}

	void OpacityComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), OpacityComponent::DefaultOpacity, offset );
		data.write( materialShader.getMaterialChunk(), OpacityComponent::DefaultBwAccumulationOperator, offset );
	}

	bool OpacityComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	String const OpacityComponent::TypeName = C3D_MakePassOtherComponentName( "opacity" );

	OpacityComponent::OpacityComponent( Pass & pass )
		: BaseDataPassComponentT< OpacityData >{ pass, TypeName, {}
			, OpacityComponent::DefaultOpacity
			, makeRangedValue( OpacityComponent::DefaultBwAccumulationOperator
				, OpacityComponent::MinBwAccumulationOperator
				, OpacityComponent::MaxBwAccumulationOperator ) }
	{
	}

	void OpacityComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Opacity" ) );
		vis.visit( cuT( "Factor" ), m_value.opacity );
		vis.visit( cuT( "Blended weighted accumulator" ), m_value.bwAccumulationOperator );
	}

	void OpacityComponent::setOpacity( float v )
	{
		m_value.opacity = v;
	}

	PassComponentUPtr OpacityComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< OpacityComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool OpacityComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< OpacityComponent >{ tabs }( *this, file );
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
