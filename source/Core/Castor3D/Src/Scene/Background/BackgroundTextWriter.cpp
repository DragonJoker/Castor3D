#include "Scene/Background/BackgroundTextWriter.hpp"

#include "Engine.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Colour.hpp"
#include "Scene/Background/Image.hpp"
#include "Scene/Background/Skybox.hpp"
#include "Texture/TextureLayout.hpp"

namespace castor3d
{
	BackgroundTextWriter::BackgroundTextWriter( castor::TextFile & file
		, castor::String const & tabs )
		: m_file{ file }
		, m_tabs{ tabs }
	{
	}

	void BackgroundTextWriter::visit( ColourBackground const & background )
	{
	}

	void BackgroundTextWriter::visit( SkyboxBackground const & background )
	{
		auto result = m_file.writeText( cuT( "\n" ) + m_tabs + cuT( "// Skybox\n" ) ) > 0;

		if ( result )
		{
			SkyboxBackground::TextWriter writer{ m_tabs };
			writer( background, m_file );
		}
	}

	void BackgroundTextWriter::visit( ImageBackground const & background )
	{
		castor::Path relative = Scene::TextWriter::copyFile( castor::Path{ background.getTexture().getDefaultImage().toString() }
			, m_file.getFilePath()
			, castor::Path{ cuT( "Textures" ) } );
		auto result = m_file.writeText( m_tabs + cuT( "background_image \"" ) + relative + cuT( "\"\n" ) ) > 0;
		castor::TextWriter< Scene >::checkError( result, "Background image" );
	}
}
