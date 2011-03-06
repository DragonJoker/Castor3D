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

ScenePtr SceneManager :: CreateScene( const String & p_strName)
{
	ScenePtr l_pReturn;
	TypeMap::iterator l_it = m_objectMap.find( p_strName);

	if (l_it != m_objectMap.end())
	{
		l_pReturn = l_it->second;
	}
	else
	{
		l_pReturn.reset( new Scene( this, p_strName));
		m_objectMap[p_strName] = l_pReturn;
	}

	return l_pReturn;
}