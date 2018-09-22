/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundFramePreparator_H___
#define ___C3D_BackgroundFramePreparator_H___

#include "Scene/Background/Visitor.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.11.0
	\date		02/04/2018
	\~english
	\brief		Background visitor used to prepare the frame command buffer.
	\~french
	\brief		Visiteur de fond utilisé pour préparer le tampon de commandes d'une frame.
	*/
	class BackgroundFramePreparator
		: public BackgroundVisitor
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
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
