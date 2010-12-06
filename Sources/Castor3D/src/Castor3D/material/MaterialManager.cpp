#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/MaterialManager.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "scene/SceneFileParser.h"

using namespace Castor3D;
using namespace Castor::Templates;

MaterialManager :: MaterialManager()
{
}

MaterialManager :: ~MaterialManager()
{
//	m_defaultMaterial.reset();
//	delete m_defaultMaterial;
	Clear();
}

void MaterialManager::Initialise()
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	l_pThis->m_defaultMaterial.reset( new Material( CU_T( "DefaultMaterial")));
	l_pThis->m_defaultMaterial->Initialise();
	l_pThis->m_defaultMaterial->GetPass( 0)->SetDoubleFace( true);
}

void MaterialManager :: Clear()
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	UniqueManager<String, Material, MaterialManager>::Clear();
	l_pThis->m_defaultMaterial.reset();
}

void MaterialManager :: Update()
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	std::map<String, Material *>::iterator l_it = l_pThis->m_newMaterials.begin();

	while (l_it != l_pThis->m_newMaterials.end())
	{
		l_it->second->Initialise();
		++l_it;
	}

	l_pThis->m_newMaterials.clear();

	for (size_t i = 0 ; i < l_pThis->m_arrayToDelete.size() ; i++)
	{
		l_pThis->m_arrayToDelete[i].reset();
//		delete m_arrayToDelete[i];
	}

	l_pThis->m_arrayToDelete.clear();
}

void MaterialManager :: GetMaterialNames( StringArray & l_names)
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	l_names.clear();
	TypeMap::const_iterator l_it = l_pThis->m_objectMap.begin();

	while (l_it != l_pThis->m_objectMap.end())
	{
		l_names.push_back( l_it->first);
		l_it++;
	}
}

MaterialPtr MaterialManager :: CreateMaterial( const String & p_name, int p_iNbInitialPasses)
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	MaterialPtr l_material;

	if (l_pThis->m_objectMap.find( p_name) == l_pThis->m_objectMap.end())
	{
		l_material.reset( new Material( p_name, p_iNbInitialPasses));
		l_pThis->AddElement( l_material);
		Logger::LogMessage( CU_T( "Material %s created"), p_name.char_str());
		l_pThis->m_newMaterials[p_name] = l_material.get();
	}
	else
	{
		l_material = l_pThis->m_objectMap.find( p_name)->second;
		Logger::LogMessage( CU_T( "Can't create Material %s, already exists"), p_name.char_str());
		l_material->Ref();
	}

	return l_material;
}

void MaterialManager :: SetToInitialise( MaterialPtr p_material)
{
	if ( ! p_material.null())
	{
		SetToInitialise( p_material.get());
	}
}

void MaterialManager :: SetToInitialise( Material * p_material)
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);

	if (p_material != NULL)
	{
		std::map<String, Material *>::iterator l_it = l_pThis->m_newMaterials.find( p_material->GetName());

		if (l_it == l_pThis->m_newMaterials.end())
		{
			l_pThis->m_newMaterials[p_material->GetName()] = p_material;
		}
	}
}

bool MaterialManager :: Write( const String & p_path)
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	bool l_bReturn = true;
	size_t l_slashIndex = p_path.find_last_of( CU_T( "."));
	String l_path = p_path.substr( 0, l_slashIndex) + ".cmtl";
	TypeMap::const_iterator l_it = l_pThis->m_objectMap.begin();
	MaterialLoader l_loader;

	File l_pFile( l_path, File::eWrite);
	bool l_bFirst = true;

	while (l_bReturn && l_it != l_pThis->m_objectMap.end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			l_pFile.WriteLine( "\n");
		}

		l_bReturn = l_loader.SaveToFile( l_pFile, l_it->second);
		++l_it;
	}

	return l_bReturn;
}

bool MaterialManager :: Read( const String & p_path)
{
	StringArray l_files;
	bool l_bReturn = true;
	size_t l_slashIndex = p_path.find_last_of( CU_T( "."));
	String l_path = p_path.substr( 0, l_slashIndex) + ".cmtl";

	SceneFileParser l_parser;
	l_parser.ParseFile( l_path);
/*
	File::ListDirectoryFiles( l_path, l_files);

	String l_fileName;
	String l_matName;
	String l_dotIndex;
	MaterialLoader l_loader;

	for (size_t i = 0 ; i < l_files.size() && l_bReturn ; i++)
	{
		if (l_files[i].find( CU_T( ".cmtl")) != String::npos)
		{
			l_bReturn = (l_loader.LoadFromFile( l_files[i]) != NULL);
		}
	}
*/
	return l_bReturn;
}

void MaterialManager :: DeleteAll()
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	TypeMap::const_iterator l_it = l_pThis->m_objectMap.begin();

	while (l_it != l_pThis->m_objectMap.end())
	{
		l_pThis->m_arrayToDelete.push_back( l_it->second);
		l_it++;
	}

	l_pThis->m_objectMap.clear();
}

MaterialPtr MaterialManager :: GetDefaultMaterial()
{
	MaterialManager * l_pThis = GetSingletonPtr();
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	return l_pThis->m_defaultMaterial;
}