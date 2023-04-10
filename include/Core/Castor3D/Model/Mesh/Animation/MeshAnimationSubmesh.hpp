/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationSubmesh_H___
#define ___C3D_MeshAnimationSubmesh_H___

#include "MeshAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"

namespace castor3d
{
	class MeshAnimationSubmesh
		: public castor::OwnedBy< MeshAnimation >
		, public std::enable_shared_from_this< MeshAnimationSubmesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\param[in]	submesh		The submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 *\param[in]	submesh		Le sous-maillage.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh const & rhs ) = delete;
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		Submesh & getSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		Submesh const & getSubmesh()const
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		MorphComponent const & getComponent()const
		{
			return *m_component;
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		MorphComponent & getComponent()
		{
			return *m_component;
		}

	private:
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;
		//!\~english	The bones component.
		//!\~french		Le composant des os.
		MorphComponentRPtr m_component;

		friend class MeshAnimationInstanceSubmesh;
	};
}

#endif
