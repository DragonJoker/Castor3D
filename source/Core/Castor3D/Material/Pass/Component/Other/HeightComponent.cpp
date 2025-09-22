#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< HeightComponent >
		: public TextWriterT< HeightComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< HeightComponent >{ tabs }
		{
		}

		bool operator()( HeightComponent const & object
			, StringStream & file )override
		{
			if ( object.getParallaxOcclusion() != ParallaxOcclusionMode::eNone )
			{
				return write( file, cuT( "parallax_occlusion" ), getName( object.getParallaxOcclusion() ) );
			}

			return true;
		}
	};

	//*********************************************************************************************

	namespace hgtcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassParallaxOcclusion, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto value = uint32_t( ParallaxOcclusionMode::eNone );
				params[0]->get( value );
				auto & component = getPassComponent< HeightComponent >( *blockContext );
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

			if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				inits.emplace_back( shader::makeRawExpr( surface->getMember< shader::DerivVec3 >( "tangentSpaceFragPosition", shader::derivVec3( 0.0_f ) ) ) );
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "tangentSpaceFragPosition", vec3( 0.0_f ) ) ) );
			}
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

	void HeightComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "parallax_occlusion" )
			, hgtcmp::parserPassParallaxOcclusion
			, { makeParameter< ParameterType::eCheckedText, ParallaxOcclusionMode >() } );
	}

	bool HeightComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry );
	}

	//*********************************************************************************************

	String const HeightComponent::TypeName = C3D_MakePassOtherComponentName( "height" );

	HeightComponent::HeightComponent( Pass & pass )
		: BaseDataPassComponentT< AtomicGroupChangeTracked< ParallaxOcclusionMode > >{ pass, TypeName }
	{
	}

	void HeightComponent::accept( ConfigurationVisitorBase & vis )
	{
		static StringArray names{ cuT( "None" )
			, cuT( "One" )
			, cuT( "Repeat" ) };
		vis.visit( cuT( "Parallax Occlusion" ) );
		vis.visit( cuT( "Mode" )
			, m_value
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< ParallaxOcclusionMode >( [this]( ParallaxOcclusionMode, ParallaxOcclusionMode newV )
			{
				m_value = newV;
			} ) );
	}

	PassComponentUPtr HeightComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< HeightComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool HeightComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< HeightComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
