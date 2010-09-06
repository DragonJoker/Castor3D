#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/MaterialManager.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "scene/SceneFileParser.h"
#include "Log.h"

using namespace Castor3D;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
	delete m_defaultMaterial;
	Clear();
}

void MaterialManager::Initialise()
{
	m_defaultMaterial = new Material( C3D_T( "DefaultMaterial"));
	m_defaultMaterial->Initialise();
	m_defaultMaterial->GetPass( 0)->SetDoubleFace( true);
}

void MaterialManager :: Update()
{
	TypeMap::iterator l_it = m_newMaterials.begin();

	while (l_it != m_newMaterials.end())
	{
		l_it->second->Initialise();
		++l_it;
	}

	m_newMaterials.clear();

	for (size_t i = 0 ; i < m_arrayToDelete.size() ; i++)
	{
		delete m_arrayToDelete[i];
	}

	m_arrayToDelete.clear();
}

void MaterialManager :: GetMaterialNames( StringArray & l_names)const
{
	l_names.clear();
	TypeMap::const_iterator l_it = m_objectMap.begin();

	while (l_it != m_objectMap.end())
	{
		l_names.push_back( l_it->first);
		l_it++;
	}
}

Material * MaterialManager :: CreateMaterial( const String & p_name, int p_iNbInitialPasses)
{
	Material * l_material = GetElementByName( p_name);

	if (l_material == NULL)
	{
		l_material = new Material( p_name, p_iNbInitialPasses);
		AddElement( l_material);
		Log::LogMessage( C3D_T( "Material %s created"), p_name.c_str());
		m_newMaterials.insert( MaterialPtrStrMap::value_type( p_name, l_material));
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't create Material %s, already exists"), p_name.c_str());
		l_material->Ref();
	}

	return l_material;
}

void MaterialManager :: SetToInitialise( Material * p_material)
{
	if (p_material != NULL)
	{
		MaterialPtrStrMap::iterator l_it = m_newMaterials.find( p_material->GetName());

		if (l_it == m_newMaterials.end())
		{
			m_newMaterials.insert( MaterialPtrStrMap::value_type( p_material->GetName(), p_material));
		}
	}
}

bool MaterialManager :: Write( const String & p_path)const
{
	bool l_bReturn = true;
	size_t l_slashIndex = p_path.find_last_of( C3D_T( "."));
	String l_path = p_path.substr( 0, l_slashIndex) + ".csmat";
	TypeMap::const_iterator l_it = m_objectMap.begin();
	MaterialLoader l_loader;

	FileIO * l_pFile = new FileIO( l_path, FileBase::eWrite);
	bool l_bFirst = true;

	while (l_bReturn && l_it != m_objectMap.end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			l_pFile->WriteLine( "\n");
		}

		l_bReturn = l_loader.SaveToFileIO( l_pFile, l_it->second);
		++l_it;
	}

	delete l_pFile;

	return l_bReturn;
}

bool MaterialManager :: Read( const String & p_path)
{
	bool l_bReturn = true;
	StringArray l_files;

	size_t l_slashIndex = p_path.find_last_of( C3D_T( "."));
	String l_path = p_path.substr( 0, l_slashIndex) + ".csmat";

	SceneFileParser l_parser;
	l_parser.ParseFile( l_path);
/*
	FileIO::ListDirectoryFiles( l_path, l_files);

	String l_fileName;
	String l_matName;
	String l_dotIndex;
	MaterialLoader l_loader;

	for (size_t i = 0 ; i < l_files.size() && l_bReturn ; i++)
	{
		if (l_files[i].find( C3D_T( ".csmat")) != String::npos)
		{
			l_bReturn = (l_loader.LoadFromFileIO( l_files[i]) != NULL);
		}
	}
*/
	return l_bReturn;
}

void MaterialManager :: DeleteAll()
{
	TypeMap::const_iterator l_it = m_objectMap.begin();

	while (l_it != m_objectMap.end())
	{
		m_arrayToDelete.push_back( l_it->second);
		l_it++;
	}

	m_objectMap.clear();
}