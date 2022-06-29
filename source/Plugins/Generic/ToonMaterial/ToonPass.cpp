#include "ToonPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>

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
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, cuT( "edge_width" ), pass.getEdgeWidth() )
				&& write( file, cuT( "edge_depth_factor" ), pass.getDepthFactor() )
				&& write( file, cuT( "edge_normal_factor" ), pass.getNormalFactor() )
				&& write( file, cuT( "edge_object_factor" ), pass.getObjectFactor() )
				&& writeNamedSub( file, cuT( "edge_colour" ), pass.getEdgeColour() );
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
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonBlinnPhongPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, cuT( "edge_width" ), pass.getEdgeWidth() )
				&& write( file, cuT( "edge_depth_factor" ), pass.getDepthFactor() )
				&& write( file, cuT( "edge_normal_factor" ), pass.getNormalFactor() )
				&& write( file, cuT( "edge_object_factor" ), pass.getObjectFactor() )
				&& writeNamedSub( file, cuT( "edge_colour" ), pass.getEdgeColour() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};

	template<>
	class TextWriter< toon::ToonPbrPass >
		: public TextWriterT< toon::ToonPbrPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< toon::ToonPbrPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( toon::ToonPbrPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing ToonPbrPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metalness", pass.getMetalness() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "smooth_band_width", pass.getSmoothBandWidth() )
				&& write( file, cuT( "edge_width" ), pass.getEdgeWidth() )
				&& write( file, cuT( "edge_depth_factor" ), pass.getDepthFactor() )
				&& write( file, cuT( "edge_normal_factor" ), pass.getNormalFactor() )
				&& write( file, cuT( "edge_object_factor" ), pass.getObjectFactor() )
				&& writeNamedSub( file, cuT( "edge_colour" ), pass.getEdgeColour() );
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
			eTextureRemap = CU_MakeSectionName( 'T', 'P', 'T', 'R' ),
			eTextureRemapChannel = CU_MakeSectionName( 'T', 'P', 'T', 'C' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_texture_unit" ) };
		static castor::String const RemapSectionName{ cuT( "toon_texture_remap" ) };
		static castor::String const RemapChannelSectionName{ cuT( "toon_texture_remap_channel" ) };
	}

	//*********************************************************************************************

	namespace blinn_phong
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'B', 'P' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'B', 'T', 'U' ),
			eTextureRemap = CU_MakeSectionName( 'T', 'B', 'T', 'R' ),
			eTextureRemapChannel = CU_MakeSectionName( 'T', 'B', 'T', 'C' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_blinn_phong_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_blinn_phong_texture_unit" ) };
		static castor::String const RemapSectionName{ cuT( "toon_blinn_phong_texture_remap" ) };
		static castor::String const RemapChannelSectionName{ cuT( "toon_blinn_phong_texture_remap_channel" ) };
	}

	//*********************************************************************************************

	namespace pbr
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'T', 'N', 'P', 'B' ),
			eTextureUnit = CU_MakeSectionName( 'T', 'R', 'T', 'U' ),
			eTextureRemap = CU_MakeSectionName( 'T', 'R', 'T', 'R' ),
			eTextureRemapChannel = CU_MakeSectionName( 'T', 'R', 'T', 'C' ),
		};

		static castor::String const PassSectionName{ cuT( "toon_pbr_pass" ) };
		static castor::String const TextureSectionName{ cuT( "toon_pbr_texture_unit" ) };
		static castor::String const RemapSectionName{ cuT( "toon_pbr_texture_remap" ) };
		static castor::String const RemapChannelSectionName{ cuT( "toon_pbr_texture_remap_channel" ) };
	}

	//*********************************************************************************************

	castor::String const ToonPhongPass::Type = cuT( "toon_phong" );

	ToonPhongPass::ToonPhongPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		: ToonPhongPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	ToonPhongPass::ToonPhongPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: ToonPass{ parent, typeID, initialFlags }
	{
	}

	castor3d::PassSPtr ToonPhongPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonPhongPass >( parent );
	}

	castor::AttributeParsers ToonPhongPass::createParsers()
	{
		return ToonPass::createParsers( uint32_t( phong::Section::ePass )
			, uint32_t( phong::Section::eTextureUnit )
			, uint32_t( phong::Section::eTextureRemap )
			, cuT( "toon_phong_texture_remap_config" )
			, uint32_t( phong::Section::eTextureRemapChannel ) );
	}

	castor::StrUInt32Map ToonPhongPass::createSections()
	{
		return
		{
			{ uint32_t( phong::Section::ePass ), phong::PassSectionName },
			{ uint32_t( phong::Section::eTextureUnit ), phong::TextureSectionName },
			{ uint32_t( phong::Section::eTextureRemap ), phong::RemapSectionName },
			{ uint32_t( phong::Section::eTextureRemapChannel ), phong::RemapChannelSectionName },
		};
	}

	void ToonPhongPass::fillBuffer( castor3d::PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = powf( getDiffuse().red(), 2.2f );
		data.colourDiv->g = powf( getDiffuse().green(), 2.2f );
		data.colourDiv->b = powf( getDiffuse().blue(), 2.2f );
		data.colourDiv->a = getAmbient();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = getShininess().value();

		ToonPass::fillData( data );
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

	castor3d::PassSPtr ToonPhongPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonPhongPass >( material );
		result->setDiffuse( getDiffuse() );
		result->setSpecular( getSpecular() );
		result->setAmbient( getAmbient() );
		result->setShininess( getShininess().value() );
		result->setSmoothBandWidth( getSmoothBandWidth() );
		return result;
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
		: ToonPass{ parent, typeID, initialFlags }
	{
	}

	castor3d::PassSPtr ToonBlinnPhongPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonBlinnPhongPass >( parent );
	}

	castor::AttributeParsers ToonBlinnPhongPass::createParsers()
	{
		return ToonPass::createParsers( uint32_t( blinn_phong::Section::ePass )
			, uint32_t( blinn_phong::Section::eTextureUnit )
			, uint32_t( blinn_phong::Section::eTextureRemap )
			, cuT( "toon_blinn_phong_texture_remap_config" )
			, uint32_t( blinn_phong::Section::eTextureRemapChannel ) );
	}

	castor::StrUInt32Map ToonBlinnPhongPass::createSections()
	{
		return
		{
			{ uint32_t( blinn_phong::Section::ePass ), blinn_phong::PassSectionName },
			{ uint32_t( blinn_phong::Section::eTextureUnit ), blinn_phong::TextureSectionName },
			{ uint32_t( blinn_phong::Section::eTextureRemap ), blinn_phong::RemapSectionName },
			{ uint32_t( blinn_phong::Section::eTextureRemapChannel ), blinn_phong::RemapChannelSectionName },
		};
	}

	void ToonBlinnPhongPass::fillBuffer( castor3d::PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = powf( getDiffuse().red(), 2.2f );
		data.colourDiv->g = powf( getDiffuse().green(), 2.2f );
		data.colourDiv->b = powf( getDiffuse().blue(), 2.2f );
		data.colourDiv->a = getAmbient();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = getShininess().value();

		ToonPass::fillData( data );
		doFillData( data );
	}

	uint32_t ToonBlinnPhongPass::getPassSectionID()const
	{
		return uint32_t( blinn_phong::Section::ePass );
	}

	uint32_t ToonBlinnPhongPass::getTextureSectionID()const
	{
		return uint32_t( blinn_phong::Section::eTextureUnit );
	}

	bool ToonBlinnPhongPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ToonBlinnPhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	castor3d::PassSPtr ToonBlinnPhongPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonPhongPass >( material );
		result->setDiffuse( getDiffuse() );
		result->setSpecular( getSpecular() );
		result->setAmbient( getAmbient() );
		result->setShininess( getShininess().value() );
		result->setSmoothBandWidth( getSmoothBandWidth() );
		return result;
	}

	//*********************************************************************************************

	castor::String const ToonPbrPass::Type = cuT( "toon_pbr" );

	ToonPbrPass::ToonPbrPass( castor3d::Material & parent
		, castor3d::PassFlags initialFlags )
		: ToonPbrPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}
	
	ToonPbrPass::ToonPbrPass( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: ToonPass{ parent, typeID, initialFlags }
	{
	}

	castor3d::PassSPtr ToonPbrPass::create( castor3d::Material & parent )
	{
		return std::make_shared< ToonPbrPass >( parent );
	}

	castor::AttributeParsers ToonPbrPass::createParsers()
	{
		return ToonPass::createParsers( uint32_t( pbr::Section::ePass )
			, uint32_t( pbr::Section::eTextureUnit )
			, uint32_t( pbr::Section::eTextureRemap )
			, cuT( "toon_pbr_texture_remap_config" )
			, uint32_t( pbr::Section::eTextureRemapChannel ) );
	}

	castor::StrUInt32Map ToonPbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbr::Section::ePass ), pbr::PassSectionName },
			{ uint32_t( pbr::Section::eTextureUnit ), pbr::TextureSectionName },
			{ uint32_t( pbr::Section::eTextureRemap ), pbr::RemapSectionName },
			{ uint32_t( pbr::Section::eTextureRemapChannel ), pbr::RemapChannelSectionName },
		};
	}

	void ToonPbrPass::fillBuffer( castor3d::PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getAlbedo().red();
		data.colourDiv->g = getAlbedo().green();
		data.colourDiv->b = getAlbedo().blue();
		data.colourDiv->a = getRoughness();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = getMetalness();

		ToonPass::fillData( data );
		doFillData( data );
	}

	uint32_t ToonPbrPass::getPassSectionID()const
	{
		return uint32_t( pbr::Section::ePass );
	}

	uint32_t ToonPbrPass::getTextureSectionID()const
	{
		return uint32_t( pbr::Section::eTextureUnit );
	}

	bool ToonPbrPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ToonPbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	castor3d::PassSPtr ToonPbrPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonPbrPass >( material );
		result->setAlbedo( getAlbedo() );
		result->setRoughness( getRoughness() );

		if ( m_specularSet )
		{
			result->setSpecular( getSpecular() );
		}

		if ( m_metalnessSet )
		{
			result->setMetalness( getMetalness() );
		}

		return result;
	}

	//*********************************************************************************************
}
