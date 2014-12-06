#include "AseFileContext.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Ase
{
	AseFileContext::AseFileContext( AseFileParser * p_pParser, TextFile * p_pFile )
		:	FileParserContext( p_pFile )
		,	pScene( NULL )
		,	pSceneNode()
		,	pGeometry()
		,	pMesh()
		,	pSubmesh()
		,	pLight()
		,	pCamera()
		,	pTextureUnit()
		,	eViewportType( eVIEWPORT_TYPE_COUNT )
		,	strName()
		,	strName2()
		,	uiUInt16( 0 )
		,	uiUInt32( 0 )
		,	uiUInt64( 0 )
		,	m_pParser( p_pParser )
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
