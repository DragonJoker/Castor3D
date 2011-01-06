/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Movable objects handler class
	/*!
	Movable objects description : name, center, orientation, parent node
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API MovableObject
	{
	public:
		typedef enum
		{
			eCamera,
			eGeometry,
			eLight
		}
		eTYPE;

	protected:
		String m_strName;			//!< The name
		SceneNode * m_pSceneNode;	//!< The parent scene node, the center's position is relative to the parent node's position
		eTYPE m_eType;

	public :
		/**
		 * Constructor
		 *@param p_sn : [in] Parent node
		 *@param p_name : [in] The name
		 */
		MovableObject( SceneNode * p_sn, const String & p_name);
		/**
		 * Destructor
		 */
		virtual ~MovableObject();
		virtual void Render( eDRAW_TYPE p_displayMode)=0;
		virtual void EndRender()=0;
		/**
		 * Cleans the pointers the object has created and that are not necessary (currently none)
		 */
		void Cleanup();
		/**
		 * Writes the object in a file
		 *@param p_pFile : [in] the file to write in
		 *@return true if successful, false if not
		 */
		bool Write( Castor::Utils::File & p_pFile)const;
		/**
		 * Detaches the movable object from it's parent
		 */
		void Detach();

	public:
		/**@name Accessors */
		//@{
		inline String			GetName		()const	{ return m_strName; }
		inline SceneNode	*	GetParent	()const { return m_pSceneNode; }
		inline const eTYPE &	GetType		()const { return m_eType; }

		inline void SetName	( const String & p_strName)	{ m_strName = p_strName; }

		virtual void AttachTo( SceneNode * p_node) { m_pSceneNode = p_node; }
		//@}
	};
}

#endif
