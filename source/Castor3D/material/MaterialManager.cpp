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
	Manager<Material>::Clear();
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
	if (p_material != NULL)
	{
		SetToInitialise( p_material.get());
	}
}

void MaterialManager :: SetToInitialise( Material * p_material)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();

	if (p_material != NULL)
	{
		String l_strName = p_material->GetName();
		std::map<String, Material *>::iterator l_it = m_newMaterials.find( l_strName);

		if (l_it == m_newMaterials.end())
		{
			m_newMaterials.insert(std::map<String, Material *>::value_type( l_strName, p_material));
		}
	}
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


bool MaterialManager :: Write( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = true;
	TypeMap::const_iterator l_it = m_objectMap.begin();
	MaterialLoader l_loader;

	bool l_bFirst = true;

	while (l_bReturn && l_it != m_objectMap.end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteLine( "\n");
		}

		l_bReturn = l_loader.SaveToFile( p_file, l_it->second);
		++l_it;
	}

	return l_bReturn;
}

bool MaterialManager :: Read( File & p_file)
{
	SceneFileParser l_parser( m_pParent);
	return l_parser.ParseFile( p_file);
}

bool MaterialManager :: Save( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	bool l_bReturn = p_file.Write( m_objectMap.size()) == sizeof( size_t);
	TypeMap::const_iterator l_it = m_objectMap.begin();

	while (l_bReturn && l_it != m_objectMap.end())
	{
		l_bReturn = l_it->second->Save( p_file);
		++l_it;
	}

	return l_bReturn;
}

bool MaterialManager :: Load( File & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_uiSize;
	bool l_bReturn = p_file.Write( l_uiSize) == sizeof( size_t);

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		MaterialPtr l_pMaterial( new Material( this, C3DEmptyString, 0));
		l_bReturn = l_pMaterial->Save( p_file);

		if (l_bReturn)
		{
			m_objectMap.insert( TypeMap::value_type( l_pMaterial->GetName(), l_pMaterial));
		}
	}

	return l_bReturn;
}