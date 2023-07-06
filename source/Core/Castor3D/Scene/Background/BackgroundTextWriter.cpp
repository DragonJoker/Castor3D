#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"

namespace castor3d
{
	BackgroundTextWriter::BackgroundTextWriter( castor::TextFile & file
		, castor::String const & tabs )
	{
	}

	void BackgroundTextWriter::visit( ColourBackground & background )
	{
	}

	void BackgroundTextWriter::visit( SkyboxBackground & background )
	{
	}

	void BackgroundTextWriter::visit( ImageBackground & background )
	{
	}
}
