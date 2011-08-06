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
#ifndef ___C3D_Material___
#define ___C3D_Material___

#include "Prerequisites.hpp"

namespace Castor
{	namespace Resources
{
	//! Material loader
	/*!
	Loads and saves materials from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::Material>
	{
	public:
		/**
		 * Loads a material from a binary file
		 *@param p_material : [in/out] The material to load
		 *@param p_file : [in/out] The file where to load the material
		 */
		static bool Load( Castor3D::Material & p_material, Utils::File & p_file);
		/**
		 * Saves a material into a binary file
		 *@param p_material : [in] The material to save
		 *@param p_file : [in/out] The file where to save the material
		 */
		static bool Save( const Castor3D::Material & p_material, Utils::File & p_file);
		/**
		 * Writes a material into a text file
		 *@param p_material : [in] The material to write
		 *@param p_file : [in/out] The file where to write the material
		 */
		static bool Write( const Castor3D::Material & p_material, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! Definition of a material
	/*!
	A material is composed of one or more passes.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Material : public Castor::Resources::Resource<Material>, public MemoryTraced<Material>
	{
	protected:
		friend class Factory<Material>;			//!< The MaterialLoader is a friend so it is the only one who can create a material :P

		PassPtrArray m_passes;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor, not be used by user, use Loader<Material>::LoadFromFile() or Loader<Material>::CreateMaterial() instead
		 */
		Material( String const & p_strName = cuEmptyString, int p_iNbInitialPasses=1);
		/**
		 * Destructor
		 */
		virtual ~Material();
		//@}
		
		/**
		 * Applies the material
		 */
		void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Applies the material
		 */
		void Render2D();
		/**
		 * Removes the material (to avoid it from interfering with other materials)
		 */
		void EndRender();
		/**
		 * Initialises the material and all it's passes
		 */
		void Initialise();
		/**
		 * Flushes passes
		 */
		void Cleanup();
		/**
		 * Creates a Pass and returns it
		 */
		PassPtr CreatePass();
		/**
		 * Rerieves a pass and returns it
		 *@param p_index : [in] The index of the wanted pass
		 *@return The retrieved pass or nullptr if not found
		 */
		const PassPtr GetPass( unsigned int p_index)const;
		PassPtr GetPass( unsigned int p_index);
		/**
		 * Destroys the pass at the given index
		 */
		void DestroyPass( unsigned int p_index);

		/**@name Accessors */
		//@{
		inline unsigned int			GetNbPasses			()const { return m_passes.size(); }
		//@}
	};
}

#endif

