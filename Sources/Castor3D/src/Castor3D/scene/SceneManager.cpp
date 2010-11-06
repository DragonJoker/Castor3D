
#include "PrecompiledHeader.h"

#include "scene/Module_Scene.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"



using namespace Castor3D;


SceneManager :: SceneManager()
{
}

SceneManager :: ~SceneManager()
{
	Clear();
}

ScenePtr SceneManager :: CreateElement( const String & p_elementName)
{
	SceneManager & l_pThis = GetSingleton();
	ScenePtr l_pReturn;

	if (l_pThis.m_objectMap.find( p_elementName) == l_pThis.m_objectMap.end())
	{
		l_pReturn.reset( new Scene( p_elementName));
		l_pThis.m_objectMap.insert( std::pair<String, ScenePtr>( p_elementName, l_pReturn));
	}
	else
	{
		l_pReturn = l_pThis.m_objectMap.find( p_elementName)->second;
	}

	return l_pReturn;
}

void SceneManager :: ClearScenes()
{
	SceneManager & l_pThis = GetSingleton();

	for (TypeMap::iterator l_it = l_pThis.m_objectMap.begin() ; l_it != l_pThis.m_objectMap.end() ; ++l_it)
	{
		l_it->second->ClearScene();
	}

//	map::cycle( m_objectMap, & Scene::ClearScene);
}