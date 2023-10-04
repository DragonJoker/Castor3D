#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

CU_ImplementSmartPtr( castor3d, SubsurfaceScatteringComponent )

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
		enum class Section
			: uint32_t
		{
			eSubsurfaceScattering = CU_MakeSectionName( 'S', 'S', 'S', 'G' ),
			eTransmittanceProfile = CU_MakeSectionName( 'T', 'R', 'P', 'R' ),
		};

		static CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				parsingContext.subsurfaceScattering = castor::makeUnique< SubsurfaceScattering >();
			}
		}
		CU_EndAttributePush( Section::eSubsurfaceScattering )

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
		CU_EndAttributePush( Section::eTransmittanceProfile )

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

	void SubsurfaceScatteringComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "sssProfileIndex" ) )
		{
			components.declMember( "sssProfileIndex", sdw::type::Kind::eUInt );
		}
	}

	void SubsurfaceScatteringComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
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
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "sssProfileIndex" ) )
		{
			res.getMember< sdw::UInt >( "sssProfileIndex" ) = max( res.getMember< sdw::UInt >( "sssProfileIndex" )
				, src.getMember< sdw::UInt >( "sssProfileIndex" ) );
		}
	}

	//*********************************************************************************************

	SubsurfaceScatteringComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
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

	void SubsurfaceScatteringComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "subsurface_scattering" )
			, sss::parserPassSubsurfaceScattering );

		castor::addParserT( parsers
			, sss::Section::eSubsurfaceScattering
			, cuT( "strength" )
			, sss::parserSubsurfaceScatteringStrength
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, sss::Section::eSubsurfaceScattering
			, cuT( "gaussian_width" )
			, sss::parserSubsurfaceScatteringGaussianWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, sss::Section::eSubsurfaceScattering
			, cuT( "transmittance_profile" )
			, sss::parserSubsurfaceScatteringTransmittanceProfile );
		castor::addParserT( parsers
			, sss::Section::eSubsurfaceScattering
			, cuT( "}" )
			, sss::parserSubsurfaceScatteringEnd );

		castor::addParserT( parsers
			, sss::Section::eTransmittanceProfile
			, cuT( "factor" )
			, sss::parserTransmittanceProfileFactor
			, { castor::makeParameter< castor::ParameterType::ePoint4F >() } );
	}

	void SubsurfaceScatteringComponent::Plugin::createSections( castor::StrUInt32Map & sections )const
	{
		sections.emplace( uint32_t( sss::Section::eSubsurfaceScattering )
			, "subsurface_scattering" );
		sections.emplace( uint32_t( sss::Section::eTransmittanceProfile )
			, "transmittance_profile" );
	}

	void SubsurfaceScatteringComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0u, 0u );
	}

	bool SubsurfaceScatteringComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	castor::String const SubsurfaceScatteringComponent::TypeName = C3D_MakePassLightingComponentName( "subsurf_scatter" );

	SubsurfaceScatteringComponent::SubsurfaceScatteringComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< SubsurfaceScatteringUPtr > >{ pass, TypeName }
	{
	}

	void SubsurfaceScatteringComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Subsurface Scattering" ) );

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
			*data.transmittanceProfileSize = subsurfaceScattering.getProfileSize();
			*data.gaussianWidth = subsurfaceScattering.getGaussianWidth();
			*data.subsurfaceScatteringStrength = subsurfaceScattering.getStrength();
			
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
			*data.transmittanceProfileSize = 0u;
			*data.gaussianWidth = 0.0f;
			*data.subsurfaceScatteringStrength = 0.0f;
		}
	}

	PassComponentUPtr SubsurfaceScatteringComponent::doClone( Pass & pass )const
	{
		auto & subsurfaceScattering = getSubsurfaceScattering();
		auto sss = castor::makeUnique< SubsurfaceScattering >();
		sss->setGaussianWidth( subsurfaceScattering.getGaussianWidth() );
		sss->setStrength( subsurfaceScattering.getStrength() );
		sss->setSubsurfaceRadius( subsurfaceScattering.getSubsurfaceRadius() );

		auto result = castor::makeUnique< SubsurfaceScatteringComponent >( pass );
		result->setSubsurfaceScattering( std::move( sss ) );
		return castor::ptrRefCast< PassComponent >( result );
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
