#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

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
