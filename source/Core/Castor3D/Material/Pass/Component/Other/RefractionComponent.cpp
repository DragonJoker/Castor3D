#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace c3d
{
	template<>
	class TextWriter< RefractionComponent >
		: public TextWriterT< RefractionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< RefractionComponent >{ tabs }
		{
		}

		bool operator()( RefractionComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "refraction_ratio" ), object.getRefractionRatio(), RefractionComponent::Default );
		}
	};

	//*********************************************************************************************

	namespace refrcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassRefractionRatio, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< RefractionComponent >( *blockContext );
				component.setRefractionRatio( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassHasRefraction, PassContext )
		{
			CU_ParsingDeprecated();
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	RefractionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void RefractionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "ior" ) )
		{
			type.declMember( "ior", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ RefractionComponent::Default } ) );
		}
	}

	//*********************************************************************************************

	void RefractionComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "ior" ) )
		{
			components.declMember( "ior", sdw::type::Kind::eFloat );
		}
	}

	void RefractionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "ior" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "ior" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ RefractionComponent::Default } ) );
		}
	}

	void RefractionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "ior" ) )
		{
			res.getMember< sdw::Float >( "ior" ) += src.getMember< sdw::Float >( "ior" ) * passMultiplier;
		}
	}

	void RefractionComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, bool isFrontCulled )const
	{
		components.dielectricF0 = vec3( pow( ( components.ior - 1.0f ) / ( components.ior + 1.0f ), 2.0_f ) );

		if ( components.hasMember( "specularFactor" ) )
		{
			components.specularWeight = components.getMember< sdw::Float >( "specularFactor" );
		}

		if ( components.hasMember( "specular" ) )
		{
			components.dielectricF0 = min( components.dielectricF0 * components.getMember< sdw::Vec3 >( "specular" ), vec3( 1.0_f ) );
		}

		components.dielectricF90 = vec3( components.specularWeight );
	}

	//*********************************************************************************************

	void RefractionComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "refraction_ratio" )
			, refrcmp::parserPassRefractionRatio
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "has_refraction" )
			, refrcmp::parserPassHasRefraction
			, { makeParameter< ParameterType::eBool >() } );
	}

	void RefractionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), RefractionComponent::Default, 0u );
	}

	bool RefractionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const RefractionComponent::TypeName = C3D_MakePassOtherComponentName( "refraction" );

	RefractionComponent::RefractionComponent( Pass & pass )
		: BaseDataPassComponentT< AtomicGroupChangeTracked< float > >{ pass, TypeName, {}
			, RefractionComponent::Default }
	{
	}

	void RefractionComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Refraction" ) );
		vis.visit( cuT( "IoR" ), m_value );
	}

	PassComponentUPtr RefractionComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< RefractionComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool RefractionComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< RefractionComponent >{ tabs }( *this, file );
	}

	void RefractionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getRefractionRatio(), 0u );
	}

	//*********************************************************************************************
}
