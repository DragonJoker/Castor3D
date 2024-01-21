#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"

namespace castor3d
{
	BackgroundTextWriter::BackgroundTextWriter( castor::TextFile &
		, castor::String const & )
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
