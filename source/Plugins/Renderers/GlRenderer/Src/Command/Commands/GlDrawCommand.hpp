/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de dessin non index�.
	*/
	class DrawCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] vtxCount
		*	Nombre de sommets.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstVertex
		*	Index du premier sommet.
		*\param[in] firstInstance
		*	Index de la premi�re instance.
		*/
		DrawCommand( uint32_t vtxCount
			, uint32_t instCount
			, uint32_t firstVertex
			, uint32_t firstInstance
			, renderer::PrimitiveTopology mode );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		uint32_t m_vtxCount;
		uint32_t m_instCount;
		uint32_t m_firstVertex;
		uint32_t m_firstInstance;
		GlPrimitiveTopology m_mode;
	};
}
