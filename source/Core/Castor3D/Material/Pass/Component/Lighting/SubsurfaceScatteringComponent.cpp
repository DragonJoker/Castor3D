#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SubsurfaceScatteringComponent >
		: public TextWriterT< castor3d::SubsurfaceScatteringComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::SubsurfaceScatteringComponent >{ tabs }
		{
		}

		bool operator()( castor3d::SubsurfaceScatteringComponent const & object
			, StringStream & file )override
		{
			auto result = true;

			if ( object.hasSubsurfaceScattering() )
			{
				auto & sss = object.getSubsurfaceScattering();

				if ( auto block{ beginBlock( file, "subsurface_scattering" ) } )
				{
					result = write( file, cuT( "strength" ), sss.getStrength() )
						&& write( file, cuT( "gaussian_width" ), sss.getGaussianWidth() );

					if ( auto profBlock{ beginBlock( file, cuT( "transmittance_profile" ) ) } )
					{
						for ( auto & factor : sss )
						{
							result = result && writeNamedSub( file, cuT( "factor" ), factor );
						}
					}
				}
			}

			return result;
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace sss
	{
		static CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext.subsurfaceScattering = std::make_unique< SubsurfaceScattering >();
			}
		}
		CU_EndAttributePush( CSCNSection::eSubsurfaceScattering )

		static CU_ImplementAttributeParser( parserSubsurfaceScatteringStrength )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.subsurfaceScattering )
			{
				CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.subsurfaceScattering->setStrength( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSubsurfaceScatteringGaussianWidth )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.subsurfaceScattering )
			{
				CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.subsurfaceScattering->setGaussianWidth( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSubsurfaceScatteringTransmittanceProfile )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.subsurfaceScattering )
			{
				CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eTransmittanceProfile )

		static CU_ImplementAttributeParser( parserSubsurfaceScatteringEnd )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.subsurfaceScattering )
			{
				CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
			}
			else
			{
				auto & component = getPassComponent< SubsurfaceScatteringComponent >( parsingContext );
				component.setSubsurfaceScattering( std::move( parsingContext.subsurfaceScattering ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParser( parserTransmittanceProfileFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.subsurfaceScattering )
			{
				CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point4f value;
				params[0]->get( value );
				parsingContext.subsurfaceScattering->addProfileFactor( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SubsurfaceScatteringComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "sssProfileIndex" ) )
		{
			components.declMember( "sssProfileIndex", sdw::type::Kind::eUInt );
		}
	}

	void SubsurfaceScatteringComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "sssProfileIndex" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt >( "sssProfileIndex" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void SubsurfaceScatteringComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents const & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "sssProfileIndex" ) )
		{
			res.getMember< sdw::UInt >( "sssProfileIndex" ) = max( src.getMember< sdw::UInt >( "sssProfileIndex" ), src.getMember< sdw::UInt >( "sssProfileIndex" ) );
		}
	}

	//*********************************************************************************************

	SubsurfaceScatteringComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ MemChunk{ 0u, 4u, 4u } }
	{
	}

	void SubsurfaceScatteringComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "sssProfileIndex" ) )
		{
			type.declMember( "sssProfileIndex", ast::type::Kind::eUInt );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	castor::String const SubsurfaceScatteringComponent::TypeName = C3D_MakePassComponentName( "subsurf_scatter" );

	SubsurfaceScatteringComponent::SubsurfaceScatteringComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< SubsurfaceScatteringUPtr > >{ pass, TypeName }
	{
	}

	void SubsurfaceScatteringComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "subsurface_scattering" )
			, sss::parserPassSubsurfaceScattering );

		Pass::addParserT( parsers
			, CSCNSection::eSubsurfaceScattering
			, cuT( "strength" )
			, sss::parserSubsurfaceScatteringStrength
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParserT( parsers
			, CSCNSection::eSubsurfaceScattering
			, cuT( "gaussian_width" )
			, sss::parserSubsurfaceScatteringGaussianWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParserT( parsers
			, CSCNSection::eSubsurfaceScattering
			, cuT( "transmittance_profile" )
			, sss::parserSubsurfaceScatteringTransmittanceProfile );
		Pass::addParserT( parsers
			, CSCNSection::eSubsurfaceScattering
			, cuT( "}" )
			, sss::parserSubsurfaceScatteringEnd );

		Pass::addParserT( parsers
			, CSCNSection::eTransmittanceProfile
			, cuT( "factor" )
			, sss::parserTransmittanceProfileFactor
			, { castor::makeParameter< castor::ParameterType::ePoint4F >() } );
	}

	void SubsurfaceScatteringComponent::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0u, 0u );
	}

	bool SubsurfaceScatteringComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	void SubsurfaceScatteringComponent::accept( PassVisitorBase & vis )
	{
		if ( hasSubsurfaceScattering() )
		{
			m_value.value()->accept( vis );
		}
	}

	void SubsurfaceScatteringComponent::update()
	{
		if ( m_sssDirty )
		{
			onProfileChanged( *this );
			m_sssDirty = false;
		}
	}

	void SubsurfaceScatteringComponent::setSubsurfaceScattering( SubsurfaceScatteringUPtr value )
	{
		( *m_value ) = std::move( value );
		m_sssConnection = ( *m_value )->onChanged.connect( [this]( SubsurfaceScattering const & sss )
			{
				m_sssDirty = true;
			} );
		m_sssDirty = true;
	}

	void SubsurfaceScatteringComponent::fillProfileBuffer( SssProfileBuffer & buffer )const
	{
		auto data = buffer.getData( getSssProfileId() );

		if ( hasSubsurfaceScattering() )
		{
			auto & subsurfaceScattering = getSubsurfaceScattering();
			data.sssInfo->x = float( subsurfaceScattering.getProfileSize() );
			data.sssInfo->y = subsurfaceScattering.getGaussianWidth();
			data.sssInfo->z = subsurfaceScattering.getStrength();
			data.sssInfo->w = 0.0f;

			auto i = 0u;
			auto & transmittanceProfile = *data.transmittanceProfile;

			for ( auto & factor : subsurfaceScattering )
			{
				transmittanceProfile[i].x = factor[0];
				transmittanceProfile[i].y = factor[1];
				transmittanceProfile[i].z = factor[2];
				transmittanceProfile[i].w = factor[3];
				++i;
			}
		}
		else
		{
			data.sssInfo->x = 0.0f;
			data.sssInfo->y = 0.0f;
			data.sssInfo->z = 0.0f;
			data.sssInfo->w = 0.0f;
		}
	}

	PassComponentUPtr SubsurfaceScatteringComponent::doClone( Pass & pass )const
	{
		auto & subsurfaceScattering = getSubsurfaceScattering();
		auto result = std::make_unique< SubsurfaceScatteringComponent >( pass );
		auto sss = std::make_unique< SubsurfaceScattering >();
		sss->setGaussianWidth( subsurfaceScattering.getGaussianWidth() );
		sss->setStrength( subsurfaceScattering.getStrength() );
		sss->setSubsurfaceRadius( subsurfaceScattering.getSubsurfaceRadius() );
		result->setSubsurfaceScattering( std::move( sss ) );
		return result;
	}

	bool SubsurfaceScatteringComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SubsurfaceScatteringComponent >{ tabs }( *this, file );
	}

	void SubsurfaceScatteringComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getSssProfileId(), 0u );
	}

	//*********************************************************************************************
}
