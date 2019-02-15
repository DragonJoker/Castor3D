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
		SkyboxBackground::TextWriter writer{ m_tabs };
		writer( background, m_file );
	}

	void BackgroundTextWriter::visit( ImageBackground const & background )
	{
		ImageBackground::TextWriter writer{ m_tabs };
		writer( background, m_file );
	}
}
