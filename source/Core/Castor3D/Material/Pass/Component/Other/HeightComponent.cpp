#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, HeightComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::HeightComponent >
		: public TextWriterT< castor3d::HeightComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::HeightComponent >{ tabs }
		{
		}

		bool operator()( castor3d::HeightComponent const & object
			, StringStream & file )override
		{
			if ( object.getParallaxOcclusion() != castor3d::ParallaxOcclusionMode::eNone )
			{
				return write( file, cuT( "parallax_occlusion" ), getName( object.getParallaxOcclusion() ) );
			}

			return true;
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace hgtcmp
	{
		static CU_ImplementAttributeParser( parserPassParallaxOcclusion )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto value = uint32_t( ParallaxOcclusionMode::eNone );
				params[0]->get( value );
				auto & component = getPassComponent< HeightComponent >( parsingContext );
				component.setParallaxOcclusion( ParallaxOcclusionMode( value ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void HeightComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eGeometry )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eGeometry ) )
		{
			return;
		}

		if ( !components.hasMember( "tangentSpaceViewPosition" ) )
		{
			components.declMember( "tangentSpaceViewPosition", sdw::type::Kind::eVec3F );
			components.declMember( "tangentSpaceFragPosition", sdw::type::Kind::eVec3F );
		}
	}

	void HeightComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "tangentSpaceViewPosition" ) )
		{
			return;
		}

		if ( surface )
		{
			inits.emplace_back( sdw::makeExpr( surface->getMember( "tangentSpaceViewPosition", vec3( 0.0_f ) ) ) );
			inits.emplace_back( sdw::makeExpr( surface->getMember( "tangentSpaceFragPosition", vec3( 0.0_f ) ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void HeightComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "tangentSpaceViewPosition" ) )
		{
			res.getMember< sdw::Vec3 >( "tangentSpaceViewPosition" ) += src.getMember< sdw::Vec3 >( "tangentSpaceViewPosition" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "tangentSpaceFragPosition" ) += src.getMember< sdw::Vec3 >( "tangentSpaceFragPosition" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void HeightComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "parallax_occlusion" )
			, hgtcmp::parserPassParallaxOcclusion
			, { castor::makeParameter< castor::ParameterType::eCheckedText, ParallaxOcclusionMode >() } );
	}

	bool HeightComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry );
	}

	//*********************************************************************************************

	castor::String const HeightComponent::TypeName = C3D_MakePassOtherComponentName( "height" );

	HeightComponent::HeightComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< ParallaxOcclusionMode > >{ pass, TypeName }
	{
	}

	void HeightComponent::accept( ConfigurationVisitorBase & vis )
	{
		static castor::StringArray names{ cuT( "None" )
			, cuT( "One" )
			, cuT( "Repeat" ) };
		vis.visit( cuT( "Parallax Occlusion" ) );
		vis.visit( cuT( "Mode" )
			, m_value
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< ParallaxOcclusionMode >( [this]( ParallaxOcclusionMode oldV, ParallaxOcclusionMode newV )
			{
				m_value = newV;
			} ) );
	}

	PassComponentUPtr HeightComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< HeightComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool HeightComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< HeightComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
