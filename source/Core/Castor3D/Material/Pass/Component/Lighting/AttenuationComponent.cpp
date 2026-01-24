#include "Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	template<>
	class TextWriter< AttenuationComponent >
		: public TextWriterT< AttenuationComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< AttenuationComponent >{ tabs }
		{
		}

		bool operator()( AttenuationComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "attenuation_colour" ), object.getAttenuationColour(), AttenuationComponent::DefaultColour )
				&& writeOpt( file, cuT( "attenuation_distance" ), object.getAttenuationDistance(), AttenuationComponent::DefaultDistance );
		}
	};

	//*********************************************************************************************

	namespace trsatt
	{
		static CU_ImplementAttributeParserBlock( parserPassAttenuationColour, PassContext )
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
				auto & component = getPassComponent< AttenuationComponent >( *blockContext );
				component.setAttenuationColour( params[0]->get< RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassAttenuationDistance, PassContext )
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
				auto & component = getPassComponent< AttenuationComponent >( *blockContext );
				component.setAttenuationDistance( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	AttenuationComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void AttenuationComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "attenuationColour" ) )
		{
			type.declMember( "attenuationColour", ast::type::Kind::eVec3F );
			type.declMember( "attenuationDistance", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ AttenuationComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void AttenuationComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
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

		if ( !components.hasMember( "attenuationColour" ) )
		{
			components.declMember( "attenuationColour", sdw::type::Kind::eVec3F );
			components.declMember( "attenuationDistance", sdw::type::Kind::eFloat );
		}
	}

	void AttenuationComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "attenuationColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "attenuationColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "attenuationDistance" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ AttenuationComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void AttenuationComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "attenuationColour" ) )
		{
			res.attenuationColour += src.attenuationColour * passMultiplier;
			res.attenuationDistance += src.attenuationDistance * passMultiplier;
		}
	}

	//*********************************************************************************************

	void AttenuationComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "attenuation_colour" )
			, trsatt::parserPassAttenuationColour
			, { makeParameter< ParameterType::eRgbColour >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "attenuation_distance" )
			, trsatt::parserPassAttenuationDistance
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void AttenuationComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), AttenuationComponent::DefaultColour, offset );
		data.write( materialShader.getMaterialChunk(), AttenuationComponent::DefaultDistance, offset );
	}

	bool AttenuationComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const AttenuationComponent::TypeName = C3D_MakePassLightingComponentName( "attenuation" );

	AttenuationComponent::AttenuationComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, { TransmissionComponent::TypeName }
			, AttenuationComponent::DefaultColour, AttenuationComponent::DefaultDistance }
	{
	}

	void AttenuationComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Attenuation" ) );
		vis.visit( cuT( "Colour" ), m_value.colour );
		vis.visit( cuT( "Distance" ), m_value.distance );
	}

	PassComponentUPtr AttenuationComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< AttenuationComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool AttenuationComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< AttenuationComponent >{ tabs }( *this, file );
	}

	void AttenuationComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getAttenuationColour()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getAttenuationDistance()
			, offset );
	}

	//*********************************************************************************************
}
