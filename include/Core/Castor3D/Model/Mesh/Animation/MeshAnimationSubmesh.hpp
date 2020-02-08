/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationSubmesh_H___
#define ___C3D_MeshAnimationSubmesh_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class MorphComponent;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Class which represents the mesh animation submeshes.
	\~french
	\brief		Classe de représentation de sous-maillages d'animations de maillage.
	*/
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
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~MeshAnimationSubmesh();
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh & getSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh const & getSubmesh()const
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		inline MorphComponent const & getComponent()const
		{
			return *m_component;
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		inline MorphComponent & getComponent()
		{
			return *m_component;
		}

	protected:
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		castor::Milliseconds m_length{ 0 };
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;
		//!\~english	The bones component.
		//!\~french		Le composant des os.
		std::shared_ptr< MorphComponent > m_component;

		friend class MeshAnimationInstanceSubmesh;
	};
}

#endif
