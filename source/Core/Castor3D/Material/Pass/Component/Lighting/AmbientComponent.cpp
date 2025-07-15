#include "Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp"

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
	class TextWriter< AmbientComponent >
		: public TextWriterT< AmbientComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< AmbientComponent >{ tabs }
		{
		}

		bool operator()( AmbientComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "ambient_colour" ), object.getAmbientColour(), AmbientComponent::DefaultColour )
				&& writeOpt( file, cuT( "ambient_factor" ), object.getAmbientFactor(), AmbientComponent::DefaultFactor );
		}
	};

	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParserBlock( parserPassAmbient, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< AmbientComponent >( *blockContext );
				component.setAmbientFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassAmbientColour, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< AmbientComponent >( *blockContext );
				component.setAmbient( params[0]->get< RgbColour >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	AmbientComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void AmbientComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "ambientColour" ) )
		{
			type.declMember( "ambientColour", ast::type::Kind::eVec3F );
			type.declMember( "ambientFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ AmbientComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ AmbientComponent::DefaultFactor } ) );
		}
	}

	//*********************************************************************************************

	void AmbientComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "ambientColour" ) )
		{
			components.declMember( "ambientColour", ast::type::Kind::eVec3F );
			components.declMember( "ambientFactor", ast::type::Kind::eFloat );
		}
	}

	void AmbientComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "ambientColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "ambientColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "ambientFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ AmbientComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ AmbientComponent::DefaultFactor } ) );
		}
	}

	void AmbientComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "ambientColour" ) )
		{
			res.getMember< sdw::Vec3 >( "ambientColour" ) += src.getMember< sdw::Vec3 >( "ambientColour", true ) * passMultiplier;
			res.getMember< sdw::Float >( "ambientFactor" ) += src.getMember< sdw::Float >( "ambientFactor", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void AmbientComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient" )
			, emscmp::parserPassAmbient
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient_factor" )
			, emscmp::parserPassAmbient
			, { makeParameter< ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient_colour" )
			, emscmp::parserPassAmbientColour
			, { makeParameter< ParameterType::eRgbColour >() } );
	}

	void AmbientComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), AmbientComponent::DefaultColour, offset );
		data.write( materialShader.getMaterialChunk(), AmbientComponent::DefaultFactor, offset );
	}

	bool AmbientComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	String const AmbientComponent::TypeName = C3D_MakePassLightingComponentName( "ambient" );

	AmbientComponent::AmbientComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, AmbientComponent::DefaultColour, AmbientComponent::DefaultFactor }
	{
	}

	void AmbientComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Ambient" ) );
		vis.visit( cuT( "Colour" ), m_value.colour );
		vis.visit( cuT( "Factor" ), m_value.factor );
	}

	PassComponentUPtr AmbientComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< AmbientComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool AmbientComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< AmbientComponent >{ tabs }( *this, file );
	}

	void AmbientComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getAmbientColour(), offset );
		data.write( m_materialShader->getMaterialChunk(), getAmbientFactor(), offset );
	}

	//*********************************************************************************************
}
