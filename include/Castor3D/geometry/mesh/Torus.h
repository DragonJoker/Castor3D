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
#ifndef ___CU_Torus___
#define ___CU_Torus___

#include "Mesh.h"

namespace Castor3D
{
	//! The torus representation
	/*!
	A torus is an ellipse performing an horizontal ellipse.
	The original ellipse will be called internal and has its own radius and number of subsections
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Torus : public Mesh
	{
	private:
		friend class MeshManager;
		real m_internalRadius;				//!< The internal torus radius
		real m_externalRadius;				//!< The torus radius
		unsigned int m_internalNbFaces;		//!< The internal torus number of subsections
		unsigned int m_externalNbFaces;		//!< The torus number of subsections
		/**
		 * Constructor, only MeshManager can use it
		 *@param p_internalRadius : [in] The wanted torus internal radius
		 *@param p_externalRadius : [in] The wanted torus radius
		 *@param p_internalNbFaces : [in] The wanted torus internal subdivisions number
		 *@param p_externalNbFaces : [in] The wanted torus subdivisions number
		 *@param p_name : [in] The mesh name
		 */
		Torus( MeshManager * p_pManager, real p_internalRadius=0.0, real p_externalRadius=0.0,
			   unsigned int p_internalNbFaces=1, unsigned int p_externalNbFaces=1,
			   const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~Torus();
		/**
		 * Generates the torus points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the torus caracteristics then rebuild it
		 *@param p_internalRadius : [in] The wanted torus internal radius
		 *@param p_externalRadius : [in] The wanted torus radius
		 *@param p_internalNbFaces : [in] The wanted torus internal subdivisions number
		 *@param p_externalNbFaces : [in] The wanted torus subdivisions number
		 */
		void Modify( real p_internalRadius, real p_externalRadius,
					 unsigned int p_internalNbFaces, unsigned int p_externalNbFaces);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetInternalNbFaces	()const {return m_internalNbFaces;}
		inline unsigned int	GetExternalNbFaces	()const {return m_externalNbFaces;}
		inline real		GetInternalRadius	()const	{return m_internalRadius;}
		inline real		GetExternalRadius	()const	{return m_externalRadius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const Torus & c) {return o << "Torus(" << c.m_internalNbFaces << "," << c.m_externalNbFaces << "," << c.m_internalRadius << "," << c.m_externalRadius << ")";}
	};
}

#endif
