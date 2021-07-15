#include "Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularGlossinessPbrPass >
		: public TextWriterT< castor3d::SpecularGlossinessPbrPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::SpecularGlossinessPbrPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::SpecularGlossinessPbrPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing SpecularGlossinessPbrPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "glossiness", pass.getGlossiness() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace pbrsg
	{
		enum class Section
			: uint32_t
		{
			eSpecularGlossinessPass = CU_MakeSectionName( 'S', 'P', 'G', 'L' ),
		};

		static castor::String const Name{ cuT( "pbrsg_pass" ) };

		CU_ImplementAttributeParser( parserPassDiffuse )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
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
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setSpecular( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassGlossiness )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setGlossiness( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const SpecularGlossinessPbrPass::Type = cuT( "specular_glossiness" );

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent )
		: Pass{ parent }
		, m_diffuse{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_glossiness{ m_dirty, 0.0f }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	PassSPtr SpecularGlossinessPbrPass::create( Material & parent )
	{
		return std::make_shared< SpecularGlossinessPbrPass >( parent );
	}

	castor::AttributeParsersBySection SpecularGlossinessPbrPass::createParsers()
	{
		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( pbrsg::Section::eSpecularGlossinessPass ), cuT( "diffuse" ), &pbrsg::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eSpecularGlossinessPass ), cuT( "albedo" ), &pbrsg::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eSpecularGlossinessPass ), cuT( "specular" ), &pbrsg::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eSpecularGlossinessPass ), cuT( "glossiness" ), &pbrsg::parserPassGlossiness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addCommonParsers( uint32_t( pbrsg::Section::eSpecularGlossinessPass ), result );

		return result;
	}

	castor::StrUInt32Map SpecularGlossinessPbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbrsg::Section::eSpecularGlossinessPass ), pbrsg::Name },
		};
	}

	void SpecularGlossinessPbrPass::accept( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getDiffuse().red();
		data.colourDiv->g = getDiffuse().green();
		data.colourDiv->b = getDiffuse().blue();
		data.colourDiv->a = 1.0f - getGlossiness();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = castor::point::length( castor::Point3f{ getSpecular().constPtr() } );

		doFillData( data );
	}

	uint32_t SpecularGlossinessPbrPass::getSectionID()const
	{
		return uint32_t( pbrsg::Section::eSpecularGlossinessPass );
	}

	bool SpecularGlossinessPbrPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularGlossinessPbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doAccept( PipelineVisitorBase & vis )
	{
		vis.visit( cuT( "Diffuse" )
			, m_diffuse );
		vis.visit( cuT( "Specular" )
			, m_specular );
		vis.visit( cuT( "Glossiness" )
			, m_glossiness );
	}

	void SpecularGlossinessPbrPass::doSetOpacity( float value )
	{
	}

	void SpecularGlossinessPbrPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "AlbOpa" ) );
		doJoinSpcGls( result );
	}

	void SpecularGlossinessPbrPass::doJoinSpcGls( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eSpecular
			, offsetof( TextureConfiguration, specularMask )
			, 0x00FFFFFF
			, TextureFlag::eGlossiness
			, offsetof( TextureConfiguration, glossinessMask )
			, 0xFF000000
			, cuT( "SpcGls" )
			, result );
	}
}
