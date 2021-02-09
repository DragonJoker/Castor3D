/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextScene_H___
#define ___C3D_TextScene_H___

#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Scene >
		: public castor::TextWriterT< castor3d::Scene >
	{
	public:
		struct Options
		{
			castor::Path materialsFile;
			castor::Path meshesFile;
			castor::Path nodesFile;
			castor::Path objectsFile;
			castor::Path lightsFile;
		};

		C3D_API explicit TextWriter( String const & tabs
			, Options options = {} );
		C3D_API bool operator()( castor3d::Scene const & scene
			, TextFile & file )override;

	private:
		Options m_options;
	};
}

#endif
