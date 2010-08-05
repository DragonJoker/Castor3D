
#include "PrecompiledHeader.h"

#include "scene/Module_Scene.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "Log.h"

using namespace Castor3D;


SceneManager :: SceneManager()
{
}

SceneManager :: ~SceneManager()
{
	Clear();
}

void SceneManager :: ClearScenes()
{
	map::cycle( m_objectMap, & Scene::ClearScene);
}