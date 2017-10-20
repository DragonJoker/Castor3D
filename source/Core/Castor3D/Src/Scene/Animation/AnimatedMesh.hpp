/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_MESH_H___
#define ___C3D_ANIMATED_MESH_H___

#include "AnimatedObject.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Represents the animated submeshes
	\~french
	\brief		Représente les sous-maillages animés
	*/
	class AnimatedMesh
		: public AnimatedObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The object name.
		 *\param[in]	p_mesh	The mesh.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom de l'objet.
		 *\param[in]	p_mesh	Le maillage.
		 */
		C3D_API AnimatedMesh( castor::String const & p_name, Mesh & p_mesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedMesh();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API AnimatedMesh( AnimatedMesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API AnimatedMesh & operator=( AnimatedMesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimatedMesh( AnimatedMesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API AnimatedMesh & operator=( AnimatedMesh const & p_rhs ) = delete;
		/**
		 *\copydoc		castor3d::AnimatedObject::Update
		 */
		C3D_API void update( castor::Milliseconds const & p_tslf )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::isPlayingAnimation
		 */
		C3D_API bool isPlayingAnimation()const override
		{
			return m_playingAnimation != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Mesh const & getMesh()const
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The skeleton
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		Le squelette
		 */
		inline Mesh & getMesh()
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The currently playing animation for this object.
		 *\~french
		 *\return		L'animation en cours de lecture sur cet objet.
		 */
		inline MeshAnimationInstance const & getPlayingAnimation()const
		{
			return *m_playingAnimation;
		}

	private:
		/**
		 *\copydoc		castor3d::AnimatedObject::doAddAnimation
		 */
		void doAddAnimation( castor::String const & p_name )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doStartAnimation
		 */
		void doStartAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doStopAnimation
		 */
		void doStopAnimation( AnimationInstance & p_animation )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::doClearAnimations
		 */
		void doClearAnimations()override;

	protected:
		//!\~english	The submesh affected by the animations.
		//!\~french		Le sous-maillage affecté par les animations.
		Mesh & m_mesh;
		//!\~english	Currently playing animation.
		//!\~french		L'animation en cours de lecture.
		MeshAnimationInstanceRPtr m_playingAnimation{ nullptr };
	};
}

#endif

