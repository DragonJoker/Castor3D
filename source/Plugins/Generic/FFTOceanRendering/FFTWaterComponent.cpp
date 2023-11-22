#include "FFTOceanRendering/FFTWaterComponent.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslPassShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( ocean_fft, FFTWaterComponent )

namespace castor
{
	template<>
	class TextWriter< ocean_fft::FFTWaterComponent >
		: public TextWriterT< ocean_fft::FFTWaterComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< ocean_fft::FFTWaterComponent >{ tabs }
		{
		}

		bool operator()( ocean_fft::FFTWaterComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, "waterDensity", object.getWaterDensity(), ocean_fft::FFTWaterComponent::Default );
		}
	};
}

namespace ocean_fft
{
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	namespace parse
	{
		static CU_ImplementAttributeParser( parserPassWaterDensity )
		{
			auto & parsingContext = getSceneParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< FFTWaterComponent >( parsingContext );
				component.setWaterDensity( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	static bool isComponentAvailable( ComponentModeFlags componentsMask
		, c3d::Materials const & materials )
	{
		return ( checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				|| checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			&& ( checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				|| checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) );
	}

	//*********************************************************************************************

	void FFTWaterComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, c3d::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "waterDensity" ) )
		{
			components.declMember( "waterDensity", sdw::type::Kind::eFloat );
			components.declMember( "waterColourMod", sdw::type::Kind::eFloat );
			components.declMember( "gradientJacobianUV", sdw::type::Kind::eVec2F );
			components.declMember( "noiseGradientUV", sdw::type::Kind::eVec2F );
			components.declMember( "mdlPosition", sdw::type::Kind::eVec3F );
		}
	}

	void FFTWaterComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, c3d::Materials const & materials
		, c3d::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "waterDensity" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "waterDensity" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ FFTWaterComponent::Default } ) );
		}

		inits.push_back( sdw::makeExpr( 1.0_f ) );

		if ( surface && surface->hasMember( "gradientJacobianUV" ) )
		{
			inits.push_back( sdw::makeExpr( surface->getMember< sdw::Vec2 >( "gradientJacobianUV" ) ) );
			inits.push_back( sdw::makeExpr( surface->getMember< sdw::Vec2 >( "noiseGradientUV" ) ) );
			inits.push_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "mdlPosition" ) ) );
		}
		else
		{
			inits.push_back( sdw::makeExpr( vec2( 0.0_f ) ) );
			inits.push_back( sdw::makeExpr( vec2( 0.0_f ) ) );
			inits.push_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void FFTWaterComponent::ComponentsShader::blendComponents( c3d::Materials const & materials
		, sdw::Float const & passMultiplier
		, c3d::BlendComponents & res
		, c3d::BlendComponents const & src )const
	{
		if ( !res.hasMember( "waterDensity" ) )
		{
			return;
		}

		res.getMember< sdw::Float >( "waterDensity", true ) += src.getMember< sdw::Float >( "waterDensity", true ) * passMultiplier;
	}

	void FFTWaterComponent::ComponentsShader::updateComponent( TextureCombine const & combine
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::BlendComponents & components
		, bool isFrontCulled )const
	{
		if ( !components.hasMember( "gradientJacobianUV" ) )
		{
			return;
		}

		auto & writer = *components.getWriter();

		if ( writer.hasGlobalVariable( "c3d_gradientJacobianMap" )
			&& writer.hasGlobalVariable( "c3d_normalsMap" ) )
		{
			auto gradientJacobianUV = components.getMember< sdw::Vec2 >( "gradientJacobianUV" );
			auto gradientJacobianMap = writer.getVariable< sdw::CombinedImage2DRgba16 >( "c3d_gradientJacobianMap" );
			auto gradJacobian = writer.declLocale( "gradJacobian"
				, gradientJacobianMap.sample( gradientJacobianUV ).xyz() );
		
			auto noiseGradientUV = components.getMember< sdw::Vec2 >( "noiseGradientUV" );
			auto normalsMap = writer.getVariable< sdw::CombinedImage2DRg32 >( "c3d_normalsMap" );
			auto noiseGradient = writer.declLocale( "noiseGradient"
				, vec2( 0.3_f ) * normalsMap.sample( noiseGradientUV ) );

			// Add low frequency gradient from heightmap with gradient from high frequency noisemap.
			auto finalNormal = writer.declLocale( "finalNormal"
				, vec3( -gradJacobian.x(), 1.0, -gradJacobian.y() ) );
			finalNormal.xz() -= noiseGradient;

			if ( isFrontCulled )
			{
				finalNormal = -finalNormal;
			}

			components.normal = normalize( finalNormal );
		
			if ( components.hasMember( "waterColourMod" ) )
			{
				auto waterColourMod = components.getMember< sdw::Float >( "waterColourMod" );

				auto jacobian = writer.declLocale( "jacobian"
					, gradJacobian.z() );
				auto turbulence = writer.declLocale( "turbulence"
					, max( 2.0_f - jacobian + dot( abs( noiseGradient ), vec2( 1.2_f ) )
						, 0.0_f ) );
				// Make water brighter based on how turbulent the water is.
				// This is rather "arbitrary", but looks pretty good in practice.
				waterColourMod = 1.0_f + 3.0_f * smoothStep( 1.2_f, 1.8_f, turbulence );
			}
		}
	}

	//*********************************************************************************************

	FFTWaterComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void FFTWaterComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "waterDensity" ) )
		{
			type.declMember( "waterDensity", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ FFTWaterComponent::Default } ) );
		}
	}

	//*********************************************************************************************

	void FFTWaterComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "waterDensity" )
			, parse::parserPassWaterDensity
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void FFTWaterComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), FFTWaterComponent::Default, 0u );
	}

	bool FFTWaterComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const FFTWaterComponent::TypeName = C3D_PluginMakePassReflectionComponentName( "fft_ocean", "water" );

	FFTWaterComponent::FFTWaterComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< float > >{ pass, TypeName, {}, Default }
	{
	}

	void FFTWaterComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "FFT Water" ) );
		vis.visit( cuT( "Water Density" ), m_value );
	}

	PassComponentUPtr FFTWaterComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< FFTWaterComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool FFTWaterComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< FFTWaterComponent >{ tabs }( *this, file );
	}

	void FFTWaterComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getWaterDensity(), 0u );
	}

	//*********************************************************************************************
}
