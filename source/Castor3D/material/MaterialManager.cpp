#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/shader/ShaderManager.h"

using namespace Castor3D;
using namespace Castor::Templates;

MaterialManager :: MaterialManager( SceneManager * p_pParent)
	:	m_pParent( p_pParent)
	,	m_pShaderManager( new ShaderManager)
{
}

MaterialManager :: ~MaterialManager()
{
//	m_defaultMaterial.reset();
//	delete m_defaultMaterial;
	delete m_pShaderManager;
	Clear();
}

void MaterialManager::Initialise()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_defaultMaterial.reset( new Material( this, CU_T( "DefaultMaterial")));
	m_defaultMaterial->Initialise();
	m_defaultMaterial->GetPass( 0)->SetDoubleFace( true);
}

void MaterialManager :: Clear()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Manager<String, Material, MaterialManager>::Clear();
	m_defaultMaterial.reset();
}

void MaterialManager :: Update()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	std::map<String, Material *>::iterator l_it = m_newMaterials.begin();

	while (l_it != m_newMaterials.end())
	{
		l_it->second->Initialise();
		++l_it;
	}

	m_newMaterials.clear();

	for (size_t i = 0 ; i < m_arrayToDelete.size() ; i++)
	{
		m_arrayToDelete[i].reset();
	}

	m_arrayToDelete.clear();
}

void MaterialManager :: GetMaterialNames( StringArray & l_names)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	l_names.clear();
	TypeMap::const_iterator l_it = m_objectMap.begin();

	while (l_it != m_objectMap.end())
	{
		l_names.push_back( l_it->first);
		l_it++;
	}
}

MaterialPtr MaterialManager :: CreateMaterial( const String & p_name, int p_iNbInitialPasses)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	MaterialPtr l_material;

	if (m_objectMap.find( p_name) == m_objectMap.end())
	{
		l_material.reset( new Material( this, p_name, p_iNbInitialPasses));
		AddElement( l_material);
		Logger::LogMessage( CU_T( "Material %s created"), p_name.char_str());
		m_newMaterials[p_name] = l_material.get();
	}
	else
	{
		l_material = m_objectMap.find( p_name)->second;
		Logger::LogMessage( CU_T( "Can't create Material %s, already exists"), p_name.char_str());
		l_material->Ref();
	}

	return l_material;
}

void MaterialManager :: SetToInitialise( MaterialPtr p_material)
{
	if ( ! p_material == NULL)
	{
		SetToInitialise( p_material.get());
	}
}

void MaterialManager :: SetToInitialise( Material * p_material)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

	if (p_material != NULL)
	{
		std::map<String, Material *>::iterator l_it = m_newMaterials.find( p_material->GetName());

		if (l_it == m_newMaterials.end())
		{
			m_newMaterials[p_material->GetName()] = p_material;
		}
	}
}

bool MaterialManager :: Write( const String & p_path)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;
	size_t l_slashIndex = p_path.find_last_of( CU_T( "."));
	String l_path = p_path.substr( 0, l_slashIndex) + ".cmtl";
	TypeMap::const_iterator l_it = m_objectMap.begin();
	MaterialLoader l_loader;

	File l_pFile( l_path, File::eWrite);
	bool l_bFirst = true;

	while (l_bReturn && l_it != m_objectMap.end())
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

	SceneFileParser l_parser( m_pParent);
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
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	TypeMap::const_iterator l_it = m_objectMap.begin();

	while (l_it != m_objectMap.end())
	{
		m_arrayToDelete.push_back( l_it->second);
		l_it++;
	}

	m_objectMap.clear();
}

MaterialPtr MaterialManager :: GetDefaultMaterial()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_defaultMaterial;
}

ImagePtr MaterialManager :: CreateImage( const String & p_strPath)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_pParent->GetImageManager()->CreateImage( p_strPath, p_strPath);
}