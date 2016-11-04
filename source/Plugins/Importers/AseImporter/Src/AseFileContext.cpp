#include "AseFileContext.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Ase
{
	AseFileContext::AseFileContext( AseFileParser * p_pParser, AseImporter & p_importer, TextFile * p_pFile, Scene & p_scene, Mesh * p_mesh )
		: FileParserContext{ p_pFile }
		, m_pParser{ p_pParser }
		, m_importer{ p_importer }
		, scene{ p_scene }
		, mesh{ p_mesh }
	{
		Initialise();
	}

	void AseFileContext::Initialise()
	{
		uiUInt16 = 0;
		uiUInt32 = 0;
		uiUInt64 = 0;
		eViewportType = ViewportType::eCount;
		bBool1 = false;
		bBool2 = false;
		pSceneNode.reset();
		pGeometry.reset();
		pMesh = nullptr;
		pSubmesh.reset();
		pLight.reset();
		pCamera.reset();
		pPass.reset();
		pMaterial.reset();
		pTextureUnit.reset();
		strName.clear();
		strName2.clear();
		m_mapMaterials.clear();
		clear_container( m_arrayTexCoords );
		m_arrayTexCoords.clear();
	}
}
