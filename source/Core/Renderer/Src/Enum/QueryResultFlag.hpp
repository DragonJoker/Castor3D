/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueryResultFlag_HPP___
#define ___Renderer_QueryResultFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des résultats de requête d'occlusion.
	*/
	enum class QueryResultFlag
		: uint32_t
	{
		e64 = 0x00000001,
		eWait = 0x00000002,
		eWithAvailability = 0x00000004,
		ePartial = 0x00000008,
	};
	Utils_ImplementFlag( QueryResultFlag )
}

#endif
