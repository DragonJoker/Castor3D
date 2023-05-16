#include "Castor3D/Material/Pass/Component/Base/FractalMappingComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, FractalMappingComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::FractalMappingComponent >
		: public TextWriterT< castor3d::FractalMappingComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::FractalMappingComponent >{ tabs }
		{
		}

		bool operator()( castor3d::FractalMappingComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "fractal" ), object.isFractal(), false );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace fractal
	{
		static CU_ImplementAttributeParser( parserPassFractal )
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
				auto & component = getPassComponent< FractalMappingComponent >( parsingContext );
				component.setFractal( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void FractalMappingComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !surface
			|| !surface->hasMember( "viewPosition" ) )
		{
			return;
		}

		if ( !components.hasMember( "viewDepth" ) )
		{
			components.declMember( "viewDepth", sdw::type::Kind::eFloat );
		}
	}

	void FractalMappingComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "viewDepth" ) )
		{
			return;
		}

		if ( surface )
		{
			inits.emplace_back( sdw::makeExpr( abs( surface->getMember< sdw::Vec3 >( "viewPosition", vec3( 0.0_f ) ).z() ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	sdw::Vec4 FractalMappingComponent::ComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 const & texCoords
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "viewDepth" ) )
		{
			return shader::PassComponentsShader::sampleMap( map
				, texCoords
				, components );
		}

		return sampleFractal( map
			, texCoords.xy()
			, dFdx( texCoords.xy() )
			, dFdy( texCoords.xy() )
			, components.getMember< sdw::Float >( "viewDepth" ) );
	}

	sdw::Vec4 FractalMappingComponent::ComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
		, shader::DerivTex const & texCoords
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "viewDepth" ) )
		{
			return shader::PassComponentsShader::sampleMap( map
				, texCoords
				, components );
		}

		return sampleFractal( map
			, texCoords.uv()
			, texCoords.dPdx()
			, texCoords.dPdy()
			, components.getMember< sdw::Float >( "viewDepth" ) );
	}

	sdw::RetVec4 FractalMappingComponent::ComponentsShader::sampleFractal( sdw::CombinedImage2DRgba32 const & pmap
		, sdw::Vec2 const & ptexCoords
		, sdw::Vec2 const & pddx
		, sdw::Vec2 const & pddy
		, sdw::Float const & pdepth )const
	{
		if ( !m_sampleFractal )
		{
			auto & writer = findWriterMandat( pmap, ptexCoords, pddx, pddy );
			m_sampleFractal = writer.implementFunction< sdw::Vec4 >( "c3d_sampleFractal"
				, [&]( sdw::CombinedImage2DRgba32 const & map
					, sdw::Vec2 const & uv
					, sdw::Vec2 const & ddx
					, sdw::Vec2 const & ddy
					, sdw::Float depth )
				{
					//Find the pixel level of detail
					auto LOD = writer.declLocale( "LOD"
						, sdw::log( depth ) );
					//Round LOD down
					auto LOD_floor = writer.declLocale( "LOD_floor"
						, sdw::floor( LOD ) );
					//Compute the fract part for interpolating
					auto LOD_fract = writer.declLocale( "LOD_fract"
						, LOD - LOD_floor );

					//Compute scaled uvs
					auto uv1 = writer.declLocale( "uv1"
						, uv / sdw::exp( LOD_floor - 1.0_f ) );
					auto uv2 = writer.declLocale( "uv2"
						, uv / sdw::exp( LOD_floor + 0.0_f ) );
					auto uv3 = writer.declLocale( "uv3"
						, uv / sdw::exp( LOD_floor + 1.0_f ) );

					//Compute continous derivitives
					auto dx = writer.declLocale( "dx"
						, ddx / depth * sdw::exp( 1.0_f ) );
					auto dy = writer.declLocale( "dy"
						, ddy / depth * sdw::exp( 1.0_f ) );

					//Sample at 3 scales
					auto tex0 = writer.declLocale( "tex0"
						, map.grad( uv1, dx, dy ) );
					auto tex1 = writer.declLocale( "tex1"
						, map.grad( uv2, dx, dy ) );
					auto tex2 = writer.declLocale( "tex2"
						, map.grad( uv3, dx, dy ) );

					//Blend samples together
					writer.returnStmt( ( tex1 + mix( tex0, tex2, vec4( LOD_fract ) ) ) * vec4( 0.5_f ) );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "map" }
				, sdw::InVec2{ writer, "uv" }
				, sdw::InVec2{ writer, "ddx" }
				, sdw::InVec2{ writer, "ddy" }
				, sdw::InFloat{ writer, "depth" } );
		}

		return m_sampleFractal( pmap
			, ptexCoords
			, pddx
			, pddy
			, pdepth );
	}

	//*********************************************************************************************

	void FractalMappingComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "fractal" )
			, fractal::parserPassFractal
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	castor::String const FractalMappingComponent::TypeName = C3D_MakePassBaseComponentName( "fractal" );

	FractalMappingComponent::FractalMappingComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void FractalMappingComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Fractal mapping" ) );
		vis.visit( cuT( "Fractal" ), m_value );
	}

	PassComponentUPtr FractalMappingComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< FractalMappingComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool FractalMappingComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< FractalMappingComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
