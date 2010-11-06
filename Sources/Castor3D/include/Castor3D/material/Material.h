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
#ifndef ___C3D_Material___
#define ___C3D_Material___

#include "../geometry/Module_Geometry.h"
#include <CastorUtils/ResourceLoader.h>

namespace Castor3D
{
	//! Material resource loader
	/*!
	Loads and saves materials from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API MaterialLoader : Castor::Resource::ResourceLoader <Material>
	{
	public:
		MaterialPtr LoadFromFile( const String & p_file);
		bool SaveToFile( Castor::Utils::File & p_pFile, MaterialPtr p_material);
	};
	//! Definition of a material
	/*!
	A material is composed of one or more passes.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Material : public Castor::Resource::Resource
	{
	protected:
		friend class MaterialManager;			//!< The MaterialManager is a friend so it is the only one who can create a material :P
		friend class MaterialLoader;			//!< The MaterialLoader is a friend so it is the only one who can create a material :P

		PassPtrArray m_passes;


	public:
		/**
		 * Constructor, not be used by user, use MaterialManager::CreateMaterial() instead
		 */
		Material( const String & p_name= C3DEmptyString, int p_iNbInitialPasses=1);
		/**
		 * Destructor
		 */
		virtual ~Material();
		/**
		 * Applies the material
		 */
		void Apply( eDRAW_TYPE p_displayMode);
		/**
		 * Applies the material
		 */
		void Apply2D( eDRAW_TYPE p_displayMode);
		/**
		 * Removes the material (to avoid it from interfering with other materials)
		 */
		void Remove();
		/**
		 * Sets my name, tells the MaterialManager my name has changed
		 *@param p_name : [in] The wanted name
		 *@return true if successful, false if not
		 */
		bool SetName( const String & p_name);
		/**
		 * Initialises the material and all it's passes
		 */
		void Initialise();
		/**
		 * Creates a Pass and returns it
		 */
		PassPtr CreatePass();
		/**
		 * Rerieves a pass and returns it
		 *@param p_index : [in] The index of the wanted pass
		 *@return The retrieved pass or NULL if not found
		 */
		PassPtr GetPass( unsigned int p_index);
		/**
		 * Destroys the pass at the given index
		 */
		void DestroyPass( unsigned int p_index);

	public:
		inline String				GetName				()const { return m_name; }
		inline unsigned int			GetNbPasses			()const { return m_passes.size(); }
		inline PassPtrArray			GetPasses			()const { return m_passes; }
	};
}

#endif

