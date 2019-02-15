#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

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
