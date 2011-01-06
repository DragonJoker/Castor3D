#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/geometry/mesh/MeshManager.h"
#include "Castor3D/material/MaterialManager.h"

using namespace Castor3D;


SceneManager :: SceneManager()
{
	m_pMeshManager = new MeshManager( this);
	m_pMaterialManager = new MaterialManager( this);
	m_pImageManager = new ImageManager();
	m_pFontManager = new FontManager();
}

SceneManager :: ~SceneManager()
{
	Clear();
	delete m_pMeshManager;
	delete m_pMaterialManager;
	delete m_pImageManager;
	delete m_pFontManager;
}

void SceneManager :: ClearScenes()
{
	for (TypeMap::iterator l_it = m_objectMap.begin() ; l_it != m_objectMap.end() ; ++l_it)
	{
		l_it->second->ClearScene();
	}
}