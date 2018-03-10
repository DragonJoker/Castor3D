/*
See LICENSE file in root folder
*/
#ifndef ___GL_CONTEXT_H___
#define ___GL_CONTEXT_H___

#include "GlRendererPrerequisites.hpp"

#include "Core/GlConnection.hpp"

namespace gl_renderer
{
	class Context
	{
	protected:
		Context( PhysicalDevice const & gpu
			, renderer::ConnectionPtr && connection );

	public:
		virtual ~Context() = default;
		/**
		*\brief
		*	Active le contexte.
		*/
		virtual void setCurrent()const = 0;
		/**
		*\brief
		*	Désactive le contexte.
		*/
		virtual void endCurrent()const = 0;
		/**
		*\brief
		*	Echange les tampons.
		*/
		virtual void swapBuffers()const = 0;
		/**
		*\brief
		*	Crée un contexte.
		*/
		static ContextPtr create( PhysicalDevice const & gpu
			, renderer::ConnectionPtr && connection );

	protected:
		PhysicalDevice const & m_gpu;
		renderer::ConnectionPtr m_connection;
	};
}

#endif
