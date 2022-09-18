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

namespace toon
{
	//*********************************************************************************************

	template< typename TypeT >
	class TextWriter
		: public castor::TextWriterT< ToonPassT< TypeT > >
	{
	public:
		TextWriter( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder )
			: castor::TextWriterT< ToonPassT< TypeT > >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( ToonPassT< TypeT > const & pass
			, castor::StringStream & file )override
		{
			castor3d::log::info << this->tabs() << cuT( "Writing Toon data " ) << std::endl;
			return this->writeOpt( file, "smooth_band_width", pass.getSmoothBandWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_width" ), pass.getEdgeWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_depth_factor" ), pass.getDepthFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_normal_factor" ), pass.getNormalFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_object_factor" ), pass.getObjectFactor(), 1.0f )
				&& this->writeNamedSubOpt( file, cuT( "edge_colour" ), pass.getEdgeColour(), castor::RgbaColour::fromPredefined( castor::PredefinedRgbaColour::eOpaqueBlack ) );
		}

	private:
		castor::Path m_folder;
		castor::String m_subfolder;
	};

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
		return castor3d::PhongPass::writeText( tabs, folder, subfolder, file )
			&& TextWriter< castor3d::PhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	castor3d::PassSPtr ToonPhongPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonPhongPass >( material );
		result->setDiffuse( getDiffuse() );
		result->setSpecular( getSpecular() );
		result->setAmbient( getAmbient() );
		result->setShininess( getShininess().value() );
		cloneData( *result );
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
		return castor3d::BlinnPhongPass::writeText( tabs, folder, subfolder, file )
			&& TextWriter< castor3d::BlinnPhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	castor3d::PassSPtr ToonBlinnPhongPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonBlinnPhongPass >( material );
		result->setDiffuse( getDiffuse() );
		result->setSpecular( getSpecular() );
		result->setAmbient( getAmbient() );
		result->setShininess( getShininess().value() );
		cloneData( *result );
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
		return castor3d::PbrPass::writeText( tabs, folder, subfolder, file )
			&& TextWriter< castor3d::PbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	castor3d::PassSPtr ToonPbrPass::doClone( castor3d::Material & material )const
	{
		auto result = std::make_shared< ToonPbrPass >( material );
		result->setAlbedo( getAlbedo() );
		result->setRoughness( getRoughness() );
		result->setMetalness( getMetalness() );
		result->setSpecular( getSpecular() );
		result->m_specularSet = m_specularSet;
		result->m_metalnessSet = m_metalnessSet;
		cloneData( *result );
		return result;
	}

	//*********************************************************************************************
}
