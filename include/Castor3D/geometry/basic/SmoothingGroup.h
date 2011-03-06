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
#ifndef ___C3D_SmoothingGroup___
#define ___C3D_SmoothingGroup___

#include "../../Prerequisites.h"

namespace Castor3D
{
	//! Smoothing groups handler class
	/*!
	Smoothig groups are used to build vertex normals and also to sort vertexes in object sections
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API SmoothingGroup : public Textable, public Serialisable, public MemoryTraced<SmoothingGroup>
	{
	private:
		class FaceAndAngle
		{
		public:
			real m_rAngle;
			FacePtr m_face;
			size_t m_uiIndex;
			Point3r m_ptVertex1;
			Point3r m_ptVertex2;

			FaceAndAngle( FacePtr p_face)
				:	m_face( p_face),
					m_rAngle( 0),
					m_uiIndex( 0)
			{
			}
		};

		typedef shared_ptr<FaceAndAngle>			FaceAndAnglePtr;
		typedef Container<FaceAndAnglePtr>::Vector	FaceAndAnglePtrArray;

	private:
		size_t m_uiGroupID;		//!< The group ID
		FacePtrArray m_arrayFaces;	//!< The faces in the group
		Submesh * m_pSubmesh;

	public:
		/**
		 * Constructor
		 *@param p_uiID : [in] the group's id
		 */
		SmoothingGroup( Submesh * p_pSubmesh, size_t p_uiID=1);
		/**
		 * Copy Constructor
		 *@param p_toCopy : [in] the group to copy
		 */
		SmoothingGroup( const SmoothingGroup & p_toCopy);
		/**
		 * Destructor
		 */
		~SmoothingGroup();
		/**
		 * Adds a face to the smoothing group
		 *@param p_face : [in] The face to add
		 */
		void AddFace( FacePtr p_face);
		/**
		 * Clears this group's faces
		 */
		void ClearFaces();
		/**
		 * Computes the vertexes normals for a given number of vertexes
		 *@param p_uiNbVertex : [in] The number of vertex for which we generate normals
		 */
		void SetNormals( size_t p_uiNbVertex);

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file) { return false; }
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

		/**@name Operators */
		//@{
		inline FacePtr		operator []	( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_arrayFaces.size());return m_arrayFaces[p_uiIndex]; }
		SmoothingGroup &	operator =	( const SmoothingGroup & p_toCopy);
		//@}

		/**@name Accessors */
		//@{
		inline size_t				GetGroupID	()const						{ return m_uiGroupID; }
		inline size_t				GetNbFaces	()const						{ return m_arrayFaces.size(); }
		inline const FacePtrArray &	GetFaces	()const						{ return m_arrayFaces; }
		inline FacePtrArray &		GetFaces	()							{ return m_arrayFaces; }
		inline FacePtr				GetFace		( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_arrayFaces.size());return m_arrayFaces[p_uiIndex]; }
		inline void SetGroupID	( const size_t & val)		{ m_uiGroupID = val; }
		//@}
	};
}

#endif
