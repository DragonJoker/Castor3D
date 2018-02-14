/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RasterisationState_HPP___
#define ___Renderer_RasterisationState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Etat de rastérisation.
	*/
	class RasterisationState
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] flags
		*	Les indicateurs de l'état.
		*\param[in] depthClampEnable
		*	Le statut d'activation du bornage en profondeur.
		*\param[in] rasteriserDiscardEnable
		*	Le statut de désactivation du rastériseur.
		*\param[in] polygonMode
		*	Le mode d'affichage des polygones.
		*\param[in] cullMode
		*	Le mode de culling.
		*\param[in] frontFace
		*	L'orientation des faces qui seront considérées comme faisant face à la caméra.
		*\param[in] depthBiasEnable
		*	Le statut d'activation du biais de profondeur.
		*\param[in] depthBiasConstantFactor
		*	Le facteur du biais de profondeur.
		*\param[in] depthBiasClamp
		*	La borne maximale du biais de profondeur.
		*\param[in] depthBiasSlopeFactor
		*	Le facteur de biais de profondeur, par rapport à la pente.
		*\param[in] lineWidth
		*	La largeur des lignes.
		*/
		RasterisationState( RasterisationStateFlags flags = 0
			, bool depthClampEnable = false
			, bool rasteriserDiscardEnable = false
			, PolygonMode polygonMode = PolygonMode::eFill
			, CullModeFlags cullMode = CullModeFlag::eBack
			, FrontFace frontFace = FrontFace::eCounterClockwise
			, bool depthBiasEnable = false
			, float depthBiasConstantFactor = 0.0f
			, float depthBiasClamp = 0.0f
			, float depthBiasSlopeFactor = 0.0f
			, float lineWidth = 1.0f );
		/**
		*\~english
		*\return
		*	The hash for this state.
		*\~french
		*\return
		*	Le hash de cet état.
		*/
		inline uint16_t getHash()const
		{
			return m_hash;
		}
		/**
		*\return
		*	Les indicateurs de l'état.
		*/
		inline RasterisationStateFlags getFlags()const
		{
			return m_flags;
		}
		/**
		*\return
		*	Le statut d'activation du bornage en profondeur.
		*/
		inline bool isDepthClampEnabled()const
		{
			return m_depthClampEnable;
		}
		/**
		*\return
		*	Le statut de désactivation du rastériseur.
		*/
		inline bool isRasteriserDiscardEnabled()const
		{
			return m_rasteriserDiscardEnable;
		}
		/**
		*\return
		*	Le mode d'affichage des polygones.
		*/
		inline PolygonMode getPolygonMode()const
		{
			return m_polygonMode;
		}
		/**
		*\return
		*	Le mode de culling.
		*/
		inline CullModeFlags getCullMode()const
		{
			return m_cullMode;
		}
		/**
		*\return
		*	L'orientation des faces qui seront considérées comme faisant face à la caméra.
		*/
		inline FrontFace getFrontFace()const
		{
			return m_frontFace;
		}
		/**
		*\return
		*	Le statut d'activation du biais de profondeur.
		*/
		inline bool isDepthBiasEnabled()const
		{
			return m_depthBiasEnable;
		}
		/**
		*\return
		*	Le facteur du biais de profondeur.
		*/
		inline float getDepthBiasConstantFactor()const
		{
			return m_depthBiasConstantFactor;
		}
		/**
		*\return
		*	La borne maximale du biais de profondeur.
		*/
		inline float getDepthBiasClamp()const
		{
			return m_depthBiasClamp;
		}
		/**
		*\return
		*	Le facteur de biais de profondeur, par rapport à la pente.
		*/
		inline float getDepthBiasSlopeFactor()const
		{
			return m_depthBiasSlopeFactor;
		}
		/**
		*\return
		*	La largeur des lignes.
		*/
		inline float getLineWidth()const
		{
			return m_lineWidth;
		}

	private:
		RasterisationStateFlags m_flags;
		bool m_depthClampEnable;
		bool m_rasteriserDiscardEnable;
		PolygonMode m_polygonMode;
		CullModeFlags m_cullMode;
		FrontFace m_frontFace;
		bool m_depthBiasEnable;
		uint16_t m_hash;
		float m_depthBiasConstantFactor;
		float m_depthBiasClamp;
		float m_depthBiasSlopeFactor;
		float m_lineWidth;
		friend bool operator==( RasterisationState const & lhs, RasterisationState const & rhs );
	};

	inline bool operator==( RasterisationState const & lhs, RasterisationState const & rhs )
	{
		return lhs.m_hash == rhs.m_hash
			&& lhs.m_depthBiasConstantFactor == rhs.m_depthBiasConstantFactor
			&& lhs.m_depthBiasClamp == rhs.m_depthBiasClamp
			&& lhs.m_depthBiasSlopeFactor == rhs.m_depthBiasSlopeFactor
			&& lhs.m_lineWidth == rhs.m_lineWidth;
	}

	inline bool operator!=( RasterisationState const & lhs, RasterisationState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
