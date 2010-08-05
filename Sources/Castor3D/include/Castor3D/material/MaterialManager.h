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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_MaterialManager___
#define ___C3D_MaterialManager___

#include "Material.h"
#include <CastorUtils/Manager.h>
#include <CastorUtils/AutoSingleton.h>

namespace Castor3D
{
	/*!
	Handles all the created materials, since they must be created by it
	It also applies them over each geometry they are to be applied
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API MaterialManager : public General::Templates::Manager <Material>, public General::Theory::AutoSingleton<MaterialManager>
	{
	private:

		Material * m_defaultMaterial;			//!< The default material
		MaterialPtrStrMap m_newMaterials;		//!< The newly created materials, a material is in this list until it is initialised
		MaterialPtrArray m_arrayToDelete;

	public:
		/**
		 * Constructor
		 */
		MaterialManager();
		/**
		 * Destructor
		 */
		~MaterialManager();
		/**
		 * Creates a material, with the given name, if it is not already used
		 *@param p_name : The wanted name of the material
		 *@return
		 */
		virtual Material * CreateMaterial( const String & p_name, int p_iNbInitialPasses=1);
		/**
		 * Initialises the default material
		 */
		void Initialise();
		/**
		 * Initialises all the newly created materials
		 */
		void Update();
		/**
		 * Puts all the materials names in the given array
		 *@param p_names : [in/out] The array of names to be filled
		 */
		void GetMaterialNames( StringArray & p_names)const;
		/**
		 * Writes the materials in separate files
		 *@return true if successful, false if not
		 */
		bool Write( const String & p_path)const;
		/**
		 * Reads materials from separate files in the same folder than the given file path
		 *@param p_file : [in] The file path
		 *@return true if successful, false if not
		 */
		bool Read( const String & p_path);
		/**
		 * Puts the given material in the newly created materials, to re-initialise it
		 *@param p_material : [in] The material we want to initialise again
		 */
		void SetToInitialise( Material * p_material);
		/**
		 * 
		 */
		void DeleteAll();

	public:
		inline Material * GetDefaultMaterial() {return m_defaultMaterial;}
	};
}

#endif

