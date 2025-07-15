#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< EmissiveComponent >
		: public TextWriterT< EmissiveComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< EmissiveComponent >{ tabs }
		{
		}

		bool operator()( EmissiveComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "emissive_colour" ), object.getEmissiveColour(), EmissiveComponent::DefaultColour )
				&& writeOpt( file, cuT( "emissive_factor" ), object.getEmissiveFactor(), EmissiveComponent::DefaultFactor );
		}
	};

	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParserBlock( parserPassEmissive, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< EmissiveComponent >( *blockContext );
				component.setEmissiveFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassEmissiveColour, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< EmissiveComponent >( *blockContext );
				component.setEmissive( params[0]->get< RgbColour >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	EmissiveComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void EmissiveComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "emissiveColour" ) )
		{
			type.declMember( "emissiveColour", ast::type::Kind::eVec3F );
			type.declMember( "emissiveFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ EmissiveComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ EmissiveComponent::DefaultFactor } ) );
		}
	}

	//*********************************************************************************************

	void EmissiveComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "emissiveColour" ) )
		{
			components.declMember( "emissiveColour", ast::type::Kind::eVec3F );
			components.declMember( "emissiveFactor", ast::type::Kind::eFloat );
		}
	}

	void EmissiveComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "emissiveColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "emissiveColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "emissiveFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ EmissiveComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ EmissiveComponent::DefaultFactor } ) );
		}
	}

	void EmissiveComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "emissiveColour" ) )
		{
			res.emissiveColour += src.emissiveColour * passMultiplier;
			res.emissiveFactor += src.emissiveFactor * passMultiplier;
		}
	}

	//*********************************************************************************************

	void EmissiveComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive" )
			, emscmp::parserPassEmissive
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive_factor" )
			, emscmp::parserPassEmissive
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive_colour" )
			, emscmp::parserPassEmissiveColour
			, { makeParameter< ParameterType::eRgbColour >() } );
	}

	void EmissiveComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), EmissiveComponent::DefaultColour, offset );
		data.write( materialShader.getMaterialChunk(), EmissiveComponent::DefaultFactor, offset );
	}

	bool EmissiveComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	String const EmissiveComponent::TypeName = C3D_MakePassLightingComponentName( "emissive" );

	EmissiveComponent::EmissiveComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, EmissiveComponent::DefaultColour, EmissiveComponent::DefaultFactor }
	{
	}

	void EmissiveComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Emissive" ) );
		vis.visit( cuT( "Colour" ), m_value.colour );
		vis.visit( cuT( "Factor" ), m_value.factor );
	}

	PassComponentUPtr EmissiveComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< EmissiveComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool EmissiveComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< EmissiveComponent >{ tabs }( *this, file );
	}

	void EmissiveComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getEmissiveColour(), offset );
		data.write( m_materialShader->getMaterialChunk(), getEmissiveFactor(), offset );
	}

	//*********************************************************************************************
}
