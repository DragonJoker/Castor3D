#include "Castor3D/Material/Pass/Component/Base/UntileComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::UntileComponent >
		: public TextWriterT< castor3d::UntileComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::UntileComponent >{ tabs }
		{
		}

		bool operator()( castor3d::UntileComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "untile" ), object.isUntiling(), false );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace unt
	{
		static CU_ImplementAttributeParser( parserPassUntile )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value{ false };
				params[0]->get( value );
				auto & component = getPassComponent< UntileComponent >( parsingContext );
				component.setUntiling( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	sdw::Vec4 UntileComponent::ComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 const & texCoords )const
	{
		return sampleUntiled( map
			, texCoords.xy()
			, dFdx( texCoords.xy() )
			, dFdy( texCoords.xy() ) );
	}

	sdw::Vec4 UntileComponent::ComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
		, shader::DerivTex const & texCoords )const
	{
		return sampleUntiled( map
			, texCoords.uv()
			, texCoords.dPdx()
			, texCoords.dPdy() );
	}

	sdw::RetVec4 UntileComponent::ComponentsShader::sampleUntiled( sdw::CombinedImage2DRgba32 const & pmap
		, sdw::Vec2 const & ptexCoords
		, sdw::Vec2 const & pddx
		, sdw::Vec2 const & pddy )const
	{
		if ( !m_hash4 )
		{
			auto & writer = findWriterMandat( pmap, ptexCoords, pddx, pddy );
			m_hash4 = writer.implementFunction< sdw::Vec4 >( "hash4"
				, [&]( sdw::Vec2 const & p )
				{
					writer.returnStmt( fract( sin( vec4( 1.0_f + dot( p, vec2( 37.0_f, 17.0_f ) )
						, 2.0_f + dot( p, vec2( 11.0_f, 47.0_f ) )
						, 3.0_f + dot( p, vec2( 41.0_f, 29.0_f ) )
						, 4.0_f + dot( p, vec2( 23.0_f, 31.0_f ) ) ) ) * 103.0_f ) );
				}
				, sdw::InVec2{ writer, "p" } );
		}

		if ( !m_sampleUntiled )
		{
			auto & writer = findWriterMandat( pmap, ptexCoords, pddx, pddy );
			m_sampleUntiled = writer.implementFunction< sdw::Vec4 >( "sampleUntiled"
				, [&]( sdw::CombinedImage2DRgba32 const & map
					, sdw::Vec2 const & texCoords
					, sdw::Vec2 const & ddx
					, sdw::Vec2 const & ddy )
				{
					auto iuv = writer.declLocale( "iuv", floor( texCoords ) );
					auto fuv = writer.declLocale( "fuv", fract( texCoords ) );

					// generate per-tile transform
					auto ofa = writer.declLocale( "ofa", m_hash4( iuv + vec2( 0.0_f, 0.0_f ) ) );
					auto ofb = writer.declLocale( "ofb", m_hash4( iuv + vec2( 1.0_f, 0.0_f ) ) );
					auto ofc = writer.declLocale( "ofc", m_hash4( iuv + vec2( 0.0_f, 1.0_f ) ) );
					auto ofd = writer.declLocale( "ofd", m_hash4( iuv + vec2( 1.0_f, 1.0_f ) ) );

					// transform per-tile uvs
					ofa.zw() = sign( ofa.zw() - 0.5_f );
					ofb.zw() = sign( ofb.zw() - 0.5_f );
					ofc.zw() = sign( ofc.zw() - 0.5_f );
					ofd.zw() = sign( ofd.zw() - 0.5_f );

					// uv's, and derivarives (for correct mipmapping)
					auto uva = writer.declLocale( "uva", texCoords * ofa.zw() + ofa.xy() );
					auto ddxa = writer.declLocale( "ddxa", ddx * ofa.zw() );
					auto ddya = writer.declLocale( "ddya", ddy * ofa.zw() );
					auto uvb = writer.declLocale( "uvb", texCoords * ofb.zw() + ofb.xy() );
					auto ddxb = writer.declLocale( "ddxb", ddx * ofb.zw() );
					auto ddyb = writer.declLocale( "ddyb", ddy * ofb.zw() );
					auto uvc = writer.declLocale( "uvc", texCoords * ofc.zw() + ofc.xy() );
					auto ddxc = writer.declLocale( "ddxc", ddx * ofc.zw() );
					auto ddyc = writer.declLocale( "ddyc", ddy * ofc.zw() );
					auto uvd = writer.declLocale( "uvd", texCoords * ofd.zw() + ofd.xy() );
					auto ddxd = writer.declLocale( "ddxd", ddx * ofd.zw() );
					auto ddyd = writer.declLocale( "ddyd", ddy * ofd.zw() );

					// fetch and blend
					auto b = writer.declLocale( "b", smoothStep( vec2( 0.25_f ), vec2( 0.75_f ), fuv ) );

					writer.returnStmt( mix( mix( map.grad( uva, ddxa, ddya ), map.grad( uvb, ddxb, ddyb ), vec4( b.x() ) )
						, mix( map.grad( uvc, ddxc, ddyc ), map.grad( uvd, ddxd, ddyd ), vec4( b.x() ) )
						, vec4( b.y() ) ) );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "map" }
				, sdw::InVec2{ writer, "texCoords" }
				, sdw::InVec2{ writer, "ddx" }
				, sdw::InVec2{ writer, "ddy" } );
		}

		return m_sampleUntiled( pmap, ptexCoords, pddx, pddy );
	}

	//*********************************************************************************************

	void UntileComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "untile" )
			, unt::parserPassUntile
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	castor::String const UntileComponent::TypeName = C3D_MakePassBaseComponentName( "untile" );

	UntileComponent::UntileComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void UntileComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Untile" ), m_value );
	}

	PassComponentUPtr UntileComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< UntileComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool UntileComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< UntileComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
