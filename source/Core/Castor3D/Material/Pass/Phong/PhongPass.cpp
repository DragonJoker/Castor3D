#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::PhongPass >
		: public TextWriterT< castor3d::PhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::PhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::PhongPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing PhongPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace phong
	{
		enum class Section
			: uint32_t
		{
			ePhongPass = CU_MakeSectionName( 'P', 'H', 'O', 'N' ),
		};

		static castor::String const Name{ cuT( "phong_pass" ) };

		CU_ImplementAttributeParser( parserPassDiffuse )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< PhongPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setDiffuse( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassSpecular )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< PhongPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setSpecular( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAmbient )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< PhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setAmbient( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassShininess )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< PhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setShininess( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const PhongPass::Type = cuT( "phong" );

	PhongPass::PhongPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	PhongPass::~PhongPass()
	{
	}

	PassSPtr PhongPass::create( Material & parent )
	{
		return std::make_shared< PhongPass >( parent );
	}

	castor::AttributeParsersBySection PhongPass::createParsers()
	{
		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( phong::Section::ePhongPass ), cuT( "diffuse" ), &phong::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( phong::Section::ePhongPass ), cuT( "specular" ), &phong::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( phong::Section::ePhongPass ), cuT( "ambient" ), &phong::parserPassAmbient, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( phong::Section::ePhongPass ), cuT( "shininess" ), &phong::parserPassShininess, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addCommonParsers( uint32_t( phong::Section::ePhongPass ), result );

		return result;
	}

	castor::StrUInt32Map PhongPass::createSections()
	{
		return
		{
			{ uint32_t( phong::Section::ePhongPass ), phong::Name },
		};
	}

	void PhongPass::accept( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getDiffuse().red();
		data.colourDiv->g = getDiffuse().green();
		data.colourDiv->b = getDiffuse().blue();
		data.colourDiv->a = getAmbient();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = getShininess().value();

		doFillData( data );
	}

	uint32_t PhongPass::getSectionID()const
	{
		return uint32_t( phong::Section::ePhongPass );
	}

	bool PhongPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< PhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	void PhongPass::doInitialise()
	{
	}

	void PhongPass::doCleanup()
	{
	}

	void PhongPass::doSetOpacity( float p_value )
	{
	}

	void PhongPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "DifOpa" ) );
		doJoinSpcShn( result );
	}

	void PhongPass::doJoinSpcShn( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eSpecular
			, offsetof( TextureConfiguration, specularMask )
			, 0x00FFFFFF
			, TextureFlag::eShininess
			, offsetof( TextureConfiguration, glossinessMask )
			, 0xFF000000
			, cuT( "SpcShn" )
			, result );
	}

	//*********************************************************************************************
}
