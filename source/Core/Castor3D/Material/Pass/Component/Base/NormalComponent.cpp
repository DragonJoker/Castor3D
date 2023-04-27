#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, NormalComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::NormalComponent >
		: public TextWriterT< castor3d::NormalComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::NormalComponent >{ tabs }
		{
		}

		bool operator()( castor3d::NormalComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "invert_normals" ), object.areNormalsInverted() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace normal
	{
		static CU_ImplementAttributeParser( parserPassInvertNormals )
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
				auto & component = getPassComponent< NormalComponent >( parsingContext );
				component.setNormalsInverted( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void NormalComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eNormals ) )
		{
			return;
		}

		if ( !components.hasMember( "normal" ) )
		{
			components.declMember( "normal", sdw::type::Kind::eVec3F );
			components.declMember( "tangent", sdw::type::Kind::eVec3F );
			components.declMember( "bitangent", sdw::type::Kind::eVec3F );
		}
	}

	void NormalComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eNormals ) )
		{
			return;
		}

		if ( surface )
		{
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "normal", vec3( 0.0_f ) ) ) );
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "tangent", vec3( 0.0_f ) ) ) );
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "bitangent", vec3( 0.0_f ) ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void NormalComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "normal" ) )
		{
			res.getMember< sdw::Vec3 >( "normal" ) += src.getMember< sdw::Vec3 >( "normal" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "tangent" ) += src.getMember< sdw::Vec3 >( "tangent" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "bitangent" ) += src.getMember< sdw::Vec3 >( "bitangent" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void NormalComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "invert_normals" )
			, normal::parserPassInvertNormals
			, { castor::makeDefaultedParameter< castor::ParameterType::eBool >( true ) } );
	}

	//*********************************************************************************************

	castor::String const NormalComponent::TypeName = C3D_MakePassBaseComponentName( "normal" );

	NormalComponent::NormalComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void NormalComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Invert Normals" ), m_value );
	}

	PassComponentUPtr NormalComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< NormalComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool NormalComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< NormalComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
