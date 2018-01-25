#pragma once

#include "Prerequisites.hpp"

#include <Utils/DynamicLibrary.hpp>

namespace common
{
	/**
	*\brief
	*	Gère un plugin de rendu (wrappe la fonction de création).
	*/
	class RendererPlugin
	{
	private:
		using CreatorFunction = renderer::Renderer *( * )();

	public:
		RendererPlugin( RendererPlugin const & ) = delete;
		RendererPlugin( RendererPlugin && ) = default;
		RendererPlugin & operator=( RendererPlugin const & ) = delete;
		RendererPlugin & operator=( RendererPlugin && ) = default;

		RendererPlugin( renderer::DynamicLibrary && library
			, RendererFactory & factory );
		renderer::RendererPtr create();

	private:
		renderer::DynamicLibrary m_library;
		CreatorFunction m_creator;
	};
}
