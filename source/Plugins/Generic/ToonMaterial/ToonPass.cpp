#include "ToonPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/PassBuffer/PassBuffer.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	//*********************************************************************************************

	template<>
	class TextWriter< toon::ToonPhongPass >
		: public TextWriterT< toon::ToonPhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< toon::ToonPhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( toon::ToonPhongPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, "edge_width", pass.getEdgeWidth() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};

	template<>
	class TextWriter< toon::ToonBlinnPhongPass >
		: public TextWriterT< toon::ToonBlinnPhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< toon::ToonBlinnPhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( toon::ToonBlinnPhongPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonBlinnPhongPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, "edge_width", pass.getEdgeWidth() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};

	template<>
	class TextWriter< toon::ToonMetallicRoughnessPass >
		: public TextWriterT< toon::ToonMetallicRoughnessPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< toon::ToonMetallicRoughnessPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( toon::ToonMetallicRoughnessPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonMetallicRoughnessPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metallic", pass.getMetallic() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, "edge_width", pass.getEdgeWidth() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};

	template<>
	class TextWriter< toon::ToonSpecularGlossinessPass >
		: public TextWriterT< toon::ToonSpecularGlossinessPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< toon::ToonSpecularGlossinessPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( toon::ToonSpecularGlossinessPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonSpecularGlossinessPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "glossiness", pass.getGlossiness() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, "edge_width", pass.getEdgeWidth() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace toon
{
	//*********************************************************************************************

	namespace phong
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'P', 'H' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'P', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassSmoothBandWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setSmoothBandWidth( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setEdgeWidth( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const ToonPhongPass::Type = cuT( "toon_phong" );
	castor::String const ToonPhongPass::Name = cuT( "Toon Materials" );

	ToonPhongPass::ToonPhongPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		:ToonPhongPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	ToonPhongPass::ToonPhongPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: castor3d::PhongPass{ parent, typeID, initialFlags }
		, m_smoothBandWidth{ m_dirty, 1.0f }
		, m_edgeWidth{ m_dirty, { 1.0f, castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) } }
		, m_stepsCount{ m_dirty, { 2u, castor::makeRange( MinStepsCount, MaxStepsCount ) } }
	{
	}

	ToonPhongPass::~ToonPhongPass()
	{
	}

	castor3d::PassSPtr ToonPhongPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonPhongPass >( parent );
	}

	castor::AttributeParsersBySection ToonPhongPass::createParsers()
	{
		return createParsers( uint32_t( phong::Section::ePass )
			, uint32_t( phong::Section::eTextureUnit ) );
	}

	castor::AttributeParsersBySection ToonPhongPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		auto result = PhongPass::createParsers( mtlSectionID, texSectionID );
		Pass::addParser( result, mtlSectionID, cuT( "smooth_band_width" ), phong::parserPassSmoothBandWidth, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_width" ), phong::parserPassEdgeWidth, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) ) } );
		return result;
	}

	castor::StrUInt32Map ToonPhongPass::createSections()
	{
		return
		{
			{ uint32_t( phong::Section::ePass ), phong::PassSectionName },
			{ uint32_t( phong::Section::eTextureUnit ), phong::TextureSectionName },
		};
	}

	void ToonPhongPass::accept( castor3d::PassBuffer & buffer )const
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
		data.specific->r = getSmoothBandWidth();
		data.specific->g = getEdgeWidth();
		data.specific->b = getStepsCount();

		doFillData( data );
	}

	uint32_t ToonPhongPass::getPassSectionID()const
	{
		return uint32_t( phong::Section::ePass );
	}

	uint32_t ToonPhongPass::getTextureSectionID()const
	{
		return uint32_t( phong::Section::eTextureUnit );
	}

	bool ToonPhongPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ToonPhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	void ToonPhongPass::doAccept( castor3d::PassVisitorBase & vis )
	{
		castor3d::PhongPass::doAccept( vis );
		vis.visit( cuT( "Smooth band width" )
			, m_smoothBandWidth );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		//vis.visit( cuT( "Steps count" )
		//	, m_stepsCount );
	}

	void ToonPhongPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		castor3d::PhongPass::doAccept( configuration, vis );
	}

	//*********************************************************************************************

	namespace blinn_phong
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'B', 'P' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'B', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_blinn_phong_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_blinn_phong_texture_unit" ) };
	}

	//*********************************************************************************************

	castor::String const ToonBlinnPhongPass::Type = cuT( "toon_blinn_phong" );

	ToonBlinnPhongPass::ToonBlinnPhongPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		: ToonBlinnPhongPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	ToonBlinnPhongPass::ToonBlinnPhongPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: ToonPhongPass{ parent, typeID, initialFlags }
	{
	}

	ToonBlinnPhongPass::~ToonBlinnPhongPass()
	{
	}

	castor3d::PassSPtr ToonBlinnPhongPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonBlinnPhongPass >( parent );
	}

	castor::AttributeParsersBySection ToonBlinnPhongPass::createParsers()
	{
		return createParsers( uint32_t( blinn_phong::Section::ePass )
			, uint32_t( blinn_phong::Section::eTextureUnit ) );
	}

	castor::AttributeParsersBySection ToonBlinnPhongPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		return ToonPhongPass::createParsers( mtlSectionID, texSectionID );
	}

	castor::StrUInt32Map ToonBlinnPhongPass::createSections()
	{
		return
		{
			{ uint32_t( blinn_phong::Section::ePass ), blinn_phong::PassSectionName },
			{ uint32_t( blinn_phong::Section::eTextureUnit ), blinn_phong::TextureSectionName },
		};
	}

	uint32_t ToonBlinnPhongPass::getPassSectionID()const
	{
		return uint32_t( blinn_phong::Section::ePass );
	}

	uint32_t ToonBlinnPhongPass::getTextureSectionID()const
	{
		return uint32_t( blinn_phong::Section::eTextureUnit );
	}

	//*********************************************************************************************

	namespace pbrmr
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'M', 'R' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'M', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_metallic_roughness_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_metallic_roughness_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassSmoothBandWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonMetallicRoughnessPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setSmoothBandWidth( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonMetallicRoughnessPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setEdgeWidth( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const ToonMetallicRoughnessPass::Type = cuT( "toon_metallic_roughness" );

	ToonMetallicRoughnessPass::ToonMetallicRoughnessPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		: ToonMetallicRoughnessPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}
	
	ToonMetallicRoughnessPass::ToonMetallicRoughnessPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: castor3d::MetallicRoughnessPbrPass{ parent, typeID, initialFlags }
		, m_smoothBandWidth{ m_dirty, 1.0f }
		, m_edgeWidth{ m_dirty, { 1.0f, castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) } }
		, m_stepsCount{ m_dirty, { 2u, castor::makeRange( MinStepsCount, MaxStepsCount ) } }
	{
	}

	ToonMetallicRoughnessPass::~ToonMetallicRoughnessPass()
	{
	}

	castor3d::PassSPtr ToonMetallicRoughnessPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonMetallicRoughnessPass >( parent );
	}

	castor::AttributeParsersBySection ToonMetallicRoughnessPass::createParsers()
	{
		return createParsers( uint32_t( pbrmr::Section::ePass )
			, uint32_t( pbrmr::Section::eTextureUnit ) );
	}

	castor::AttributeParsersBySection ToonMetallicRoughnessPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		auto result = castor3d::MetallicRoughnessPbrPass::createParsers( mtlSectionID, texSectionID );
		Pass::addParser( result, mtlSectionID, cuT( "smooth_band_width" ), pbrmr::parserPassSmoothBandWidth, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_width" ), pbrmr::parserPassEdgeWidth, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) ) } );
		return result;
	}

	castor::StrUInt32Map ToonMetallicRoughnessPass::createSections()
	{
		return
		{
			{ uint32_t( pbrmr::Section::ePass ), pbrmr::PassSectionName },
			{ uint32_t( pbrmr::Section::eTextureUnit ), pbrmr::TextureSectionName },
		};
	}

	void ToonMetallicRoughnessPass::accept( castor3d::PassBuffer & buffer )const
	{
		auto f0 = castor::RgbColour{ 0.04f, 0.04f, 0.04f } *( 1.0f - getMetallic() ) + getAlbedo() * getMetallic();
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getAlbedo().red();
		data.colourDiv->g = getAlbedo().green();
		data.colourDiv->b = getAlbedo().blue();
		data.colourDiv->a = getRoughness();
		data.specDiv->r = f0.red();
		data.specDiv->g = f0.green();
		data.specDiv->b = f0.blue();
		data.specDiv->a = getMetallic();
		data.specific->r = getSmoothBandWidth();
		data.specific->g = getEdgeWidth();
		data.specific->b = getStepsCount();

		doFillData( data );
	}

	uint32_t ToonMetallicRoughnessPass::getPassSectionID()const
	{
		return uint32_t( pbrmr::Section::ePass );
	}

	uint32_t ToonMetallicRoughnessPass::getTextureSectionID()const
	{
		return uint32_t( pbrmr::Section::eTextureUnit );
	}

	bool ToonMetallicRoughnessPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ToonMetallicRoughnessPass >{ tabs, folder, subfolder }( *this, file );
	}

	void ToonMetallicRoughnessPass::doAccept( castor3d::PassVisitorBase & vis )
	{
		castor3d::MetallicRoughnessPbrPass::doAccept( vis );
		vis.visit( cuT( "Smooth band width" )
			, m_smoothBandWidth );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		//vis.visit( cuT( "Steps count" )
		//	, m_stepsCount );
	}

	void ToonMetallicRoughnessPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		castor3d::MetallicRoughnessPbrPass::doAccept( configuration, vis );
	}

	//*********************************************************************************************

	namespace pbrsg
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'S', 'G' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'S', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_specular_glossiness_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_specular_glossiness_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassSmoothBandWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonSpecularGlossinessPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setSmoothBandWidth( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonSpecularGlossinessPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				toonPass.setEdgeWidth( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const ToonSpecularGlossinessPass::Type = cuT( "toon_specular_glossiness" );

	ToonSpecularGlossinessPass::ToonSpecularGlossinessPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		: ToonSpecularGlossinessPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	ToonSpecularGlossinessPass::ToonSpecularGlossinessPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: castor3d::SpecularGlossinessPbrPass{ parent, typeID, initialFlags }
		, m_smoothBandWidth{ m_dirty, 1.0f }
		, m_edgeWidth{ m_dirty, { 1.0f, castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) } }
		, m_stepsCount{ m_dirty, { 2u, castor::makeRange( MinStepsCount, MaxStepsCount ) } }
	{
	}

	ToonSpecularGlossinessPass::~ToonSpecularGlossinessPass()
	{
	}

	castor3d::PassSPtr ToonSpecularGlossinessPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonSpecularGlossinessPass >( parent );
	}

	castor::AttributeParsersBySection ToonSpecularGlossinessPass::createParsers()
	{
		return createParsers( uint32_t( pbrsg::Section::ePass )
			, uint32_t( pbrsg::Section::eTextureUnit ) );
	}

	castor::AttributeParsersBySection ToonSpecularGlossinessPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		auto result = castor3d::SpecularGlossinessPbrPass::createParsers( mtlSectionID, texSectionID );
		Pass::addParser( result, mtlSectionID, cuT( "smooth_band_width" ), pbrsg::parserPassSmoothBandWidth, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "edge_width" ), pbrsg::parserPassEdgeWidth, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) ) } );
		return result;
	}

	castor::StrUInt32Map ToonSpecularGlossinessPass::createSections()
	{
		return
		{
			{ uint32_t( pbrsg::Section::ePass ), pbrsg::PassSectionName },
			{ uint32_t( pbrsg::Section::eTextureUnit ), pbrsg::TextureSectionName },
		};
	}

	void ToonSpecularGlossinessPass::accept( castor3d::PassBuffer & buffer )const
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
		data.specific->r = getSmoothBandWidth();
		data.specific->g = getEdgeWidth();
		data.specific->b = getStepsCount();

		doFillData( data );
	}

	uint32_t ToonSpecularGlossinessPass::getPassSectionID()const
	{
		return uint32_t( pbrsg::Section::ePass );
	}

	uint32_t ToonSpecularGlossinessPass::getTextureSectionID()const
	{
		return uint32_t( pbrsg::Section::eTextureUnit );
	}

	bool ToonSpecularGlossinessPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ToonSpecularGlossinessPass >{ tabs, folder, subfolder }( *this, file );
	}

	void ToonSpecularGlossinessPass::doAccept( castor3d::PassVisitorBase & vis )
	{
		castor3d::SpecularGlossinessPbrPass::doAccept( vis );
		vis.visit( cuT( "Smooth band width" )
			, m_smoothBandWidth );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		//vis.visit( cuT( "Steps count" )
		//	, m_stepsCount );
	}

	void ToonSpecularGlossinessPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		castor3d::SpecularGlossinessPbrPass::doAccept( configuration, vis );
	}

	//*********************************************************************************************
}
