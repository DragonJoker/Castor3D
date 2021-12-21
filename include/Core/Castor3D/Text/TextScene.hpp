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
		: public TextWriterT< castor3d::Scene >
	{
	public:
		struct Options
		{
			Options( Path prootFolder = {}
				, Path pmaterialsFile = {}
				, Path pmeshesFile = {}
				, Path pnodesFile = {}
				, Path pobjectsFile = {}
				, Path plightsFile = {}
				, String psubfolder = {}
				, float pscale = 1.0f )
				: rootFolder{ std::move( prootFolder ) }
				, materialsFile{ std::move( pmaterialsFile ) }
				, meshesFile{ std::move( pmeshesFile ) }
				, nodesFile{ std::move( pnodesFile ) }
				, objectsFile{ std::move( pobjectsFile ) }
				, lightsFile{ std::move( plightsFile ) }
				, subfolder{ std::move( psubfolder ) }
				, scale{ std::move( pscale ) }
			{
			}

			Path rootFolder;
			Path materialsFile;
			Path meshesFile;
			Path nodesFile;
			Path objectsFile;
			Path lightsFile;
			String subfolder;
			float scale;
		};

		C3D_API explicit TextWriter( String const & tabs
			, Options options = {} );
		C3D_API bool operator()( castor3d::Scene const & scene
			, castor::StringStream & file )override;

	private:
		Options m_options;
	};
}

#endif
