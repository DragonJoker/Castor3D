/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundFramePreparator_H___
#define ___C3D_BackgroundFramePreparator_H___

#include "Castor3D/Scene/Background/Visitor.hpp"

namespace castor3d
{
	class BackgroundFramePreparator
		: public BackgroundVisitor
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	commandBuffer	The command buffer.
		 *\param[in]	renderPass		The render pass.
		 *\param[in]	frameBuffer		The frame buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	commandBuffer	Le tampon de commandes.
		 *\param[in]	renderPass		La passe de rendu.
		 *\param[in]	frameBuffer		Le tampon de frame.
		 */
		C3D_API BackgroundFramePreparator( ashes::CommandBuffer & commandBuffer
			, ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~BackgroundFramePreparator();
		/**
		*\~english
		*\brief
		*	Visits a colour background.
		*\~french
		*\brief
		*	Visite un fond de type couleur.
		*/
		C3D_API void visit( ColourBackground const & background )override;
		/**
		*\~english
		*\brief
		*	Visits a skybox background.
		*\~french
		*\brief
		*	Visite un fond de type skybox.
		*/
		C3D_API void visit( SkyboxBackground const & background )override;
		/**
		*\~english
		*\brief
		*	Visits an image background.
		*\~french
		*\brief
		*	Visite un fond de type image.
		*/
		C3D_API void visit( ImageBackground const & background )override;

	private:
		ashes::CommandBuffer & m_commandBuffer;
	};
}

#endif
