/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_MaterialImporter___
#define ___C3DAssimp_MaterialImporter___

#include <Castor3D/Material/Material.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h> // Output data structure
#pragma warning( pop )

namespace c3d_assimp
{
	class AssimpImporter;

	class MaterialImporter
	{
	public:
		explicit MaterialImporter( AssimpImporter & importer );

		void import( castor::Path const & fileName
			, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, aiScene const & aiScene
			, castor3d::Scene & scene );

		castor3d::MaterialResPtr getMaterial( uint32_t index )const
		{
			auto it = m_materials.find( index );
			CU_Require( it != m_materials.end() );
			return it->second;
		}

	private:
		castor3d::MaterialResPtr doProcessMaterial( castor3d::Scene & scene
			, aiScene const & aiScene
			, aiMaterial const & aiMaterial
			, uint32_t index );

	private:
		AssimpImporter & m_importer;
		castor::Path m_fileName;
		std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > m_textureRemaps;
		std::map< uint32_t, castor3d::MaterialResPtr > m_materials;
	};
}

#endif
