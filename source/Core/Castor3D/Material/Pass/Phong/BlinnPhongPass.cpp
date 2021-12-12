#include "Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::BlinnPhongPass >
		: public TextWriterT< castor3d::BlinnPhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::BlinnPhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::BlinnPhongPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing BlinnPhongPass " ) << std::endl;
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

	namespace blinn_phong
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'B', 'L', 'P', 'N' ),
			eTextureUnit = CU_MakeSectionName( 'B', 'L', 'P', 'T' ),
		};

		static castor::String const PassSectionName{ cuT( "blinn_phong_pass" ) };
		static castor::String const TextureSectionName{ cuT( "blinn_phong_texture_unit" ) };
	}

	//*********************************************************************************************

	castor::String const BlinnPhongPass::Type = cuT( "blinn_phong" );
	castor::String const BlinnPhongPass::LightingModel = shader::BlinnPhongLightingModel::getName();

	BlinnPhongPass::BlinnPhongPass( Material & parent
		, PassFlags initialFlags )
		: BlinnPhongPass{ parent
			, parent.getEngine()->getPassFactory().getNameId( Type )
			, initialFlags }
	{
	}

	BlinnPhongPass::BlinnPhongPass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: PhongPass{ parent
			, typeID
			, initialFlags }
	{
	}

	PassSPtr BlinnPhongPass::create( Material & parent )
	{
		return std::make_shared< BlinnPhongPass >( parent );
	}

	castor::AttributeParsers BlinnPhongPass::createParsers()
	{
		return createParsers( uint32_t( blinn_phong::Section::ePass )
			, uint32_t( blinn_phong::Section::eTextureUnit ) );
	}

	castor::AttributeParsers BlinnPhongPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		return PhongPass::createParsers( mtlSectionID, texSectionID );
	}

	castor::StrUInt32Map BlinnPhongPass::createSections()
	{
		return
		{
			{ uint32_t( blinn_phong::Section::ePass ), blinn_phong::PassSectionName },
			{ uint32_t( blinn_phong::Section::eTextureUnit ), blinn_phong::TextureSectionName },
		};
	}

	uint32_t BlinnPhongPass::getPassSectionID()const
	{
		return uint32_t( blinn_phong::Section::ePass );
	}

	uint32_t BlinnPhongPass::getTextureSectionID()const
	{
		return uint32_t( blinn_phong::Section::eTextureUnit );
	}

	//*********************************************************************************************
}
