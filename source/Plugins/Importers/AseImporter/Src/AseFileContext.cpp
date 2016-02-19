#include "AseFileContext.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Ase
{
	AseFileContext::AseFileContext( AseFileParser * p_pParser, AseImporter & p_importer, TextFile * p_pFile )
		: FileParserContext( p_pFile )
		, m_pParser( p_pParser )
		, m_importer( p_importer )
	{
		Initialise();
	}

	void AseFileContext::Initialise()
	{
		pScene = NULL;
		uiUInt16 = 0;
		uiUInt32 = 0;
		uiUInt64 = 0;
		eViewportType = eVIEWPORT_TYPE_COUNT;
		bBool1 = false;
		bBool2 = false;
		pSceneNode.reset();
		pGeometry.reset();
		pMesh.reset();
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
