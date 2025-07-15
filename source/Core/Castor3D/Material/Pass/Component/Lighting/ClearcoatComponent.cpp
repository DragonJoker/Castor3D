#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< ClearcoatComponent >
		: public TextWriterT< ClearcoatComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< ClearcoatComponent >{ tabs }
		{
		}

		bool operator()( ClearcoatComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "clearcoat_factor" ), object.getClearcoatFactor(), ClearcoatComponent::DefaultFactor )
				&& writeOpt( file, cuT( "clearcoat_roughness_factor" ), object.getRoughnessFactor(), ClearcoatComponent::DefaultRoughness );
		}
	};

	//*********************************************************************************************

	namespace coating
	{
		static CU_ImplementAttributeParserBlock( parserPassClearcoatFactor, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< ClearcoatComponent >( *blockContext );
				component.setClearcoatFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassRoughnessFactor, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< ClearcoatComponent >( *blockContext );
				component.setRoughnessFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	ClearcoatComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void ClearcoatComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "clearcoatFactor" ) )
		{
			type.declMember( "clearcoatFactor", ast::type::Kind::eFloat );
			type.declMember( "clearcoatRoughness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ ClearcoatComponent::DefaultFactor } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ ClearcoatComponent::DefaultRoughness } ) );
		}
	}

	//*********************************************************************************************

	void ClearcoatComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "clearcoatFactor" ) )
		{
			components.declMember( "clearcoatNormal", sdw::type::Kind::eVec3F );
			components.declMember( "clearcoatFactor", sdw::type::Kind::eFloat );
			components.declMember( "clearcoatRoughness", sdw::type::Kind::eFloat );
			components.declMember( "clearcoatF0", sdw::type::Kind::eVec3F );
			components.declMember( "clearcoatF90", sdw::type::Kind::eVec3F );
			components.declMember( "clearcoatFresnel", sdw::type::Kind::eVec3F );
		}
	}

	void ClearcoatComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "clearcoatFactor" ) )
		{
			return;
		}

		if ( clrCot )
		{
			inits.emplace_back( sdw::makeExpr( clrCot->xyz() ) );
			inits.emplace_back( sdw::makeExpr( clrCot->a() ) );
		}
		else
		{
			if ( surface )
			{
				if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
				{
					inits.emplace_back( shader::makeRawExpr( surface->getMember< shader::DerivVec3 >( "normal", shader::derivVec3( vec3( 0.0_f, 0.0_f, 1.0_f ) ) ) ) );
				}
				else
				{
					inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "normal", vec3( 0.0_f, 0.0_f, 1.0_f ) ) ) );
				}
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( vec3( 0.0_f, 0.0_f, 1.0_f ) ) );
			}

			if ( material )
			{
				inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "clearcoatFactor" ) ) );
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( sdw::Float{ ClearcoatComponent::DefaultFactor } ) );
			}
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "clearcoatRoughness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ ClearcoatComponent::DefaultRoughness } ) );
		}

		inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) ); // clearcoatF0
		inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) ); // clearcoatF90
		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) ); // clearcoatFresnel
	}

	void ClearcoatComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "clearcoatFactor" ) )
		{
			return;
		}

		res.clearcoatFactor += src.clearcoatFactor * passMultiplier;
		res.clearcoatRoughness += src.clearcoatRoughness * passMultiplier;
		res.clearcoatNormal += src.clearcoatNormal * passMultiplier;
	}

	void ClearcoatComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, bool isFrontCulled )const
	{
		if ( components.hasMember( "clearcoatFactor" ) )
		{
			components.clearcoatF0 = vec3( pow( ( components.ior - 1.0_f ) / ( components.ior + 1.0_f ), 2.0_f ) );
			components.clearcoatF90 = vec3( 1.0_f );
			components.clearcoatRoughness = clamp( components.clearcoatRoughness, 0.0_f, 1.0_f );
		}
	}

	void ClearcoatComponent::ComponentsShader::finishComponent( shader::DerivSurfaceBase const & surface
		, shader::CameraData const & camera
		, shader::ModelData const & model
		, shader::Utils & utils
		, shader::BlendComponents & components )const
	{
		if ( components.hasMember( "clearcoatFactor" ) )
		{
			components.getMember< sdw::Vec3 >( "clearcoatFresnel" ) = utils.conductorFresnel( max( dot( components.clearcoatNormal, normalize( camera.position() - surface.worldPosition.value().xyz() ) ), 0.0_f )
				, components.clearcoatF0
				, components.clearcoatF90 );
		}
	}

	//*********************************************************************************************

	void ClearcoatComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "clearcoat_factor" )
			, coating::parserPassClearcoatFactor
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "clearcoat_roughness_factor" )
			, coating::parserPassRoughnessFactor
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void ClearcoatComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), ClearcoatComponent::DefaultFactor, offset );
		data.write( materialShader.getMaterialChunk(), ClearcoatComponent::DefaultRoughness, offset );
	}

	bool ClearcoatComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const ClearcoatComponent::TypeName = C3D_MakePassLightingComponentName( "clearcoat" );

	ClearcoatComponent::ClearcoatComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, ClearcoatComponent::DefaultFactor, ClearcoatComponent::DefaultRoughness }
	{
	}

	void ClearcoatComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Clearcoat" ) );
		vis.visit( cuT( "Factor" ), m_value.factor );
		vis.visit( cuT( "Roughness Factor" ), m_value.roughness );
	}

	PassComponentUPtr ClearcoatComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< ClearcoatComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool ClearcoatComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< ClearcoatComponent >{ tabs }( *this, file );
	}

	void ClearcoatComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getClearcoatFactor()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getRoughnessFactor()
			, offset );
	}

	//*********************************************************************************************
}
