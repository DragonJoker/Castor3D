#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/MovableObject.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Animation.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

MovableObject::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< MovableObject, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool MovableObject::TextLoader :: operator ()( MovableObject const & p_object, TextFile & p_file)
{
	bool l_bReturn = p_file.WriteText( cuT( "\tparent " ) + p_object.GetParent()->GetName() + cuT( "\n" ) ) > 0;

	return l_bReturn;
}

//*************************************************************************************************

MovableObject :: MovableObject( Scene * p_pScene, eMOVABLE_TYPE p_eType)
	:	m_pSceneNode	( NULL		)
	,	m_eType			( p_eType	)
	,	m_pScene		( p_pScene	)
{
}

MovableObject :: MovableObject( Scene * p_pScene, SceneNode * p_sn, String const & p_name, eMOVABLE_TYPE p_eType)
	:	m_strName		( p_name	)
	,	m_pSceneNode	( p_sn		)
	,	m_eType			( p_eType	)
	,	m_pScene		( p_pScene	)
{
	if( m_pSceneNode )
	{
		m_pSceneNode->AttachObject( this );
	}
}

MovableObject :: MovableObject( MovableObject const & p_object )
	:	m_strName		( p_object.m_strName		)
	,	m_strNodeName	( p_object.m_strNodeName	)
	,	m_pSceneNode	( p_object.m_pSceneNode		)
	,	m_eType			( p_object.m_eType			)
	,	m_pScene		( p_object.m_pScene			)
{
}

MovableObject :: MovableObject( MovableObject && p_object )
	:	m_strName		( std::move( p_object.m_strName		) )
	,	m_strNodeName	( std::move( p_object.m_strNodeName	) )
	,	m_pSceneNode	( std::move( p_object.m_pSceneNode	) )
	,	m_eType			( std::move( p_object.m_eType		) )
	,	m_pScene		( std::move( p_object.m_pScene		) )
{
	p_object.m_strName.clear();
	p_object.m_strNodeName.clear();
	p_object.m_pSceneNode = NULL;
	p_object.m_pScene = NULL;
}

MovableObject & MovableObject :: operator =( MovableObject const & p_object )
{
	m_strName		= p_object.m_strName		;
	m_strNodeName	= p_object.m_strNodeName	;
	m_pSceneNode	= p_object.m_pSceneNode		;
	m_eType			= p_object.m_eType			;
	m_pScene		= p_object.m_pScene			;

	return *this;
}

MovableObject & MovableObject :: operator =( MovableObject && p_object )
{
	if( this != &p_object )
	{
		m_strName		= std::move( p_object.m_strName		);
		m_strNodeName	= std::move( p_object.m_strNodeName	);
		m_pSceneNode	= std::move( p_object.m_pSceneNode	);
		m_eType			= std::move( p_object.m_eType		);
		m_pScene		= std::move( p_object.m_pScene		);

		p_object.m_strName.clear();
		p_object.m_strNodeName.clear();
		p_object.m_pSceneNode = NULL;
		p_object.m_pScene = NULL;
	}

	return *this;
}

MovableObject :: ~MovableObject()
{
	Detach();
}

void MovableObject :: Cleanup()
{
}

void MovableObject :: Detach()
{
	if (m_pSceneNode)
	{
		m_pSceneNode->DetachObject( this);
		m_pSceneNode = nullptr;
	}
}

void MovableObject :: AttachTo( SceneNode * p_node)
{
	m_pSceneNode = p_node;

	if (m_pSceneNode != NULL)
	{
		m_strNodeName = m_pSceneNode->GetName();
	}
	else
	{
		m_strNodeName = cuEmptyString;
	}
}

//*************************************************************************************************
