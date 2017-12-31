/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Reflection mapping pass implementation.
	\~french
	\brief		Implémentation d'une passe de reflection mapping.
	*/
	class EnvironmentMapPass
		: castor::OwnedBy< EnvironmentMap >
	{
	public:
		EnvironmentMapPass( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass & operator=( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass( EnvironmentMapPass && )= default;
		EnvironmentMapPass & operator=( EnvironmentMapPass && )=default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_reflectionMap	The parent reflection map.
		 *\param[in]	p_node			The node from which the camera is created.
		 *\param[in]	p_objectNode	The node to which the object is attached.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_reflectionMap	Le reflection map parente.
		 *\param[in]	p_node			Le noeud depuis lequel on crée la caméra.
		 *\param[in]	p_objectNode	Le noeud auquel l'objet est attaché.
		 */
		C3D_API EnvironmentMapPass( EnvironmentMap & p_reflectionMap
			, SceneNodeSPtr p_node
			, SceneNode const & p_objectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~EnvironmentMapPass();
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool initialise( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	p_node		The base node.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	p_node		Le noeud de base.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( SceneNode const & p_node, RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void render();

	private:
		//!\~english	The camera node.
		//!\~french		Le noeud de la camera.
		SceneNodeSPtr m_node;
		//!\~english	The camera.
		//!\~french		La camera.
		CameraSPtr m_camera;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		//!\~english	The pass used to render transparent nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds transparents.
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
	};
}

#endif
