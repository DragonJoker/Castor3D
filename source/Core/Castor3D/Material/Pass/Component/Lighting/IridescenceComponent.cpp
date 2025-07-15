#include "Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< IridescenceComponent >
		: public TextWriterT< IridescenceComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< IridescenceComponent >{ tabs }
		{
		}

		bool operator()( IridescenceComponent const & object
			, StringStream & file )override
		{
			return write( file, cuT( "iridescence_factor" ), object.getFactor(), IridescenceComponent::DefaultFactor )
				&& writeOpt( file, cuT( "iridescence_ior" ), object.getIor(), IridescenceComponent::DefaultIor )
				&& writeOpt( file, cuT( "iridescence_min_thickness" ), object.getMinThickness(), IridescenceComponent::DefaultMinThickness )
				&& writeOpt( file, cuT( "iridescence_max_thickness" ), object.getMaxThickness(), IridescenceComponent::DefaultMaxThickness );
		}
	};

	//*********************************************************************************************

	namespace irid
	{
		static CU_ImplementAttributeParserBlock( parserPassIridescenceFactor, PassContext )
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
				auto & component = getPassComponent< IridescenceComponent >( *blockContext );
				component.setFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassIridescenceIor, PassContext )
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
				auto & component = getPassComponent< IridescenceComponent >( *blockContext );
				component.setIor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassIridescenceMinThickness, PassContext )
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
				auto & component = getPassComponent< IridescenceComponent >( *blockContext );
				component.setMinThickness( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassIridescenceMaxThickness, PassContext )
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
				auto & component = getPassComponent< IridescenceComponent >( *blockContext );
				component.setMaxThickness( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	IridescenceComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void IridescenceComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "iridescenceFactor" ) )
		{
			type.declMember( "iridescenceFactor", ast::type::Kind::eFloat );
			type.declMember( "iridescenceIor", ast::type::Kind::eFloat );
			type.declMember( "iridescenceMinThickness", ast::type::Kind::eFloat );
			type.declMember( "iridescenceMaxThickness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultFactor } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultIor } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultMinThickness } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultMaxThickness } ) );
		}
	}

	//*********************************************************************************************

	void IridescenceComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
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

		if ( !components.hasMember( "iridescenceFactor" ) )
		{
			components.declMember( "iridescenceFactor", sdw::type::Kind::eFloat );
			components.declMember( "iridescenceIor", sdw::type::Kind::eFloat );
			components.declMember( "iridescenceThickness", sdw::type::Kind::eFloat );
			components.declMember( "iridescenceMinThickness", sdw::type::Kind::eFloat );
			components.declMember( "iridescenceMaxThickness", sdw::type::Kind::eFloat );
			components.declMember( "iridescenceDielectricFresnel", sdw::type::Kind::eVec3F );
			components.declMember( "iridescenceMetallicFresnel", sdw::type::Kind::eVec3F );
		}
	}

	void IridescenceComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "iridescenceFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "iridescenceFactor" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "iridescenceIor" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "iridescenceMaxThickness" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "iridescenceMinThickness" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "iridescenceMaxThickness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultFactor } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultIor } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultMaxThickness } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultMinThickness } ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ IridescenceComponent::DefaultMaxThickness } ) );
		}

		inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
	}

	void IridescenceComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "iridescenceFactor" ) )
		{
			res.iridescenceFactor += src.iridescenceFactor * passMultiplier;
			res.iridescenceIor += src.iridescenceIor * passMultiplier;
			res.iridescenceThickness += src.iridescenceThickness * passMultiplier;
			res.getMember< sdw::Float >( "iridescenceMinThickness" ) += src.getMember< sdw::Float >( "iridescenceMinThickness", true ) * passMultiplier;
			res.getMember< sdw::Float >( "iridescenceMaxThickness" ) += src.getMember< sdw::Float >( "iridescenceMaxThickness", true ) * passMultiplier;
		}
	}

	void IridescenceComponent::ComponentsShader::finishComponent( shader::DerivSurfaceBase const & surface
		, shader::CameraData const & camera
		, shader::ModelData const & model
		, shader::Utils & utils
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "iridescenceFactor" ) )
		{
			return;
		}

		auto & writer = findWriterMandat( surface, camera.position(), components );

		sdwIF( writer, components.iridescenceThickness == 0.0_f )
		{
			components.iridescenceFactor = 0.0_f;
		}
		sdwFI

		sdwIF( writer, components.iridescenceFactor != 0.0_f )
		{
			auto incident = writer.declLocale( "c3d_iridescenceIncident"
				, normalize( surface.worldPosition.value().xyz() - camera.position() ) );
			auto NdotV = writer.declLocale( "NdotV"
				, clamp( dot( components.getRawNormal(), -incident ), 0.0_f, 1.0_f ) );
			components.getMember< sdw::Vec3 >( "iridescenceDielectricFresnel" ) = utils.evalIridescence( 1.0_f
				, components.iridescenceIor
				, NdotV
				, components.iridescenceThickness
				, components.dielectricF0 );
			components.getMember< sdw::Vec3 >( "iridescenceMetallicFresnel" ) = utils.evalIridescence( 1.0_f
				, components.iridescenceIor
				, NdotV
				, components.iridescenceThickness
				, components.baseColour );
		}
		sdwFI
	}

	//*********************************************************************************************

	void IridescenceComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "iridescence_factor" )
			, irid::parserPassIridescenceFactor
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "iridescence_ior" )
			, irid::parserPassIridescenceIor
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "iridescence_min_thickness" )
			, irid::parserPassIridescenceMinThickness
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "iridescence_max_thickness" )
			, irid::parserPassIridescenceMaxThickness
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void IridescenceComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), IridescenceComponent::DefaultFactor, offset );
		offset += data.write( materialShader.getMaterialChunk(), IridescenceComponent::DefaultIor, offset );
		offset += data.write( materialShader.getMaterialChunk(), IridescenceComponent::DefaultMinThickness, offset );
		data.write( materialShader.getMaterialChunk(), IridescenceComponent::DefaultMaxThickness, offset );
	}

	bool IridescenceComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const IridescenceComponent::TypeName = C3D_MakePassLightingComponentName( "iridescence" );

	IridescenceComponent::IridescenceComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, IridescenceComponent::DefaultFactor, IridescenceComponent::DefaultIor
			, IridescenceComponent::DefaultMinThickness, IridescenceComponent::DefaultMaxThickness }
	{
	}

	void IridescenceComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Iridescence" ) );
		vis.visit( cuT( "Factor" ), m_value.factor );
		vis.visit( cuT( "IOR" ), m_value.ior );
		vis.visit( cuT( "Min Thickness" ), m_value.minThickness );
		vis.visit( cuT( "Max Thickness" ), m_value.maxThickness );
	}

	PassComponentUPtr IridescenceComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< IridescenceComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool IridescenceComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< IridescenceComponent >{ tabs }( *this, file );
	}

	void IridescenceComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getFactor()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getIor()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getMinThickness()
			, offset );
		data.write( m_materialShader->getMaterialChunk()
			, getMaxThickness()
			, offset );
	}

	//*********************************************************************************************
}
