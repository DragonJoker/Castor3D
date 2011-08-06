/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_MovableObject___
#define ___C3D_MovableObject___

#include "Prerequisites.hpp"

namespace Castor
{	namespace Resources
{
	//! MovableObject loader
	/*!
	Loads and saves movable objects from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::MovableObject>
	{
	public:
		/**
		 * Writes a movable object into a text file
		 *@param p_file : [in] the file to save the movable object in
		 *@param p_object : [in] the movable object to save
		 */
		static bool Write( const Castor3D::MovableObject & p_object, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! Movable object types enumerator
	/*!
	There are threee movable object types : camera, geometry and light.
	*/
	typedef enum
	{	eMOVABLE_TYPE_CAMERA
	,	eMOVABLE_TYPE_GEOMETRY
	,	eMOVABLE_TYPE_LIGHT
	,	eMOVABLE_TYPE_COUNT
	}	eMOVABLE_TYPE;
	//! Movable objects handler class
	/*!
	Movable objects description : name, center, orientation, parent node
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API MovableObject : public Serialisable
	{
	protected:
		eMOVABLE_TYPE m_eType;
		String m_strName;			//!< The name
		String m_strNodeName;		//!< The node name
		SceneNode * m_pSceneNode;	//!< The parent scene node, the center's position is relative to the parent node's position
		Scene * m_pScene;

	public :
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		MovableObject( eMOVABLE_TYPE p_eType);
		/**
		 * Constructor
		 *@param p_sn : [in] Parent node
		 *@param p_name : [in] The name
		 */
		MovableObject( Scene * p_pScene, SceneNode * p_sn, String const & p_name, eMOVABLE_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~MovableObject();
		//@}

		virtual void Render( ePRIMITIVE_TYPE p_displayMode)=0;
		virtual void EndRender()=0;
		/**
		 * Cleans the pointers the object has created and that are not necessary (currently none)
		 */
		void Cleanup();
		/**
		 * Detaches the movable object from it's parent
		 */
		void Detach();
		/**
		 * Attaches the movable object to a node
		 */
		void AttachTo( SceneNode * p_node);

		/**@name Accessors */
		//@{
		inline String const &	GetName		()const	{ return m_strName; }
		inline SceneNode *		GetParent	()const { return m_pSceneNode; }
		inline Scene *			GetScene	()const { return m_pScene; }
		inline const eMOVABLE_TYPE &	GetType		()const { return m_eType; }

		inline	void SetName	( String const & p_strName)	{ m_strName = p_strName; }
		//@}

		DECLARE_SERIALISE_MAP()
		DECLARE_POST_UNSERIALISE()
	};
}

#endif
