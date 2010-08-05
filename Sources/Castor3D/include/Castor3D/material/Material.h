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
#ifndef ___C3D_Material___
#define ___C3D_Material___

#include "../geometry/Module_Geometry.h"
#include <CastorUtils/ResourceLoader.h>

namespace Castor3D
{
	/*!
	Material loader from file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API MaterialLoader : General::Resource::ResourceLoader <Material>
	{
	public:
		Material * LoadFromFile( const String & p_file);
		bool SaveToFile( const String & p_file, Material * p_material);
	};
	/*!
	Castor definition of a material : base colours (ambient, diffuse, specular, emissive), shininess, 
	base texturing colour, texture units and texture units indexes
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Material : public General::Resource::Resource
	{
	protected:
		friend class MaterialManager;			//!< The MaterialManager is a friend so it is the only one who can create a material :P
		friend class MaterialLoader;			//!< The MaterialLoader is a friend so it is the only one who can create a material :P

		PassPtrArray m_passes;


	public:
		/**
		 * Constructor, not be used by user, use MaterialManager::GetSingletonPtr()->CreateMaterial() instead
		 */
		Material( const String & p_name= C3DEmptyString, int p_iNbInitialPasses=1);
		/**
		 * Destructor
		 */
		virtual ~Material();
		/**
		 * Applies this material
		 */
		void Apply( Submesh * p_submesh, DrawType p_displayMode);
		/**
		 * Applies this material
		 */
		void Apply( DrawType p_displayMode);
		/**
		 * Removes this material (to avoid it from interfering with other materials)
		 */
		void Remove();
		/**
		 * Sets my name, tells the MaterialManager my name has changed
		 *@param p_name : [in] The wanted name
		 *@return true if successful, false if not
		 */
		bool SetName( const String & p_name);
		/**
		 * Initialises this material and all it's passes
		 */
		void Initialise();
		/**
		 * Creates a Pass and returns it
		 */
		Pass * CreatePass();
		/**
		 * Rerieves a pass and returns it
		 *@param p_index : [in] The index of the wanted pass
		 *@return The retrieved pass or NULL if not found
		 */
		Pass * GetPass( unsigned int p_index);
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

