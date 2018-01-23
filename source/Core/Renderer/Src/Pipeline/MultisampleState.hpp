/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MultisampleState_HPP___
#define ___Renderer_MultisampleState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Etat de multi-�chantillonnage.
	*/
	class MultisampleState
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] flags
		*	Les indicateurs de l'�tat.
		*\param[in] rasterisationSamples
		*	Le nombre d'�chantillons par pixel, lors de la rast�risation.
		*\param[in] sampleShadingEnable
		*	D�termine si l'ex�cution du shader de fragment s'ex�cute par fragment (\p false) ou par �chantillon (\p true).
		*\param[in] minSampleShading
		*	La fraction minimale de shader par �chantillon.
		*\param[in] alphaToCoverageEnable
		*	Dit si une valeur de couverture temporaire est g�n�r�e, � partir de la composante alpha de la premi�re couleur sortie du fragment.
		*\param[in] alphaToOneEnable
		*	Dit si la composante alpha de la premi�re couleur sortie du fragment est remplac�e par 1.
		*/
		MultisampleState( MultisampleStateFlags flags = 0
			, SampleCountFlag rasterisationSamples = SampleCountFlag::e1
			, bool sampleShadingEnable = false
			, float minSampleShading = 1.0f
			, bool alphaToCoverageEnable = false
			, bool alphaToOneEnable = false );
		/**
		*\brief
		*	Constructeur.
		*\param[in] flags
		*	Les indicateurs de l'�tat.
		*\param[in] rasterisationSamples
		*	Le nombre d'�chantillons par pixel, lors de la rast�risation.
		*\param[in] sampleShadingEnable
		*	D�termine si l'ex�cution du shader de fragment s'ex�cute par fragment (\p false) ou par �chantillon (\p true).
		*\param[in] minSampleShading
		*	La fraction minimale de shader par �chantillon.
		*\param[in] sampleMask
		*	Le masque de couverture, combin� via un ET binaire avec l'information de couverture g�n�r�e lors de la rast�risation.
		*\param[in] alphaToCoverageEnable
		*	Dit si une valeur de couverture temporaire est g�n�r�e, � partir de la composante alpha de la premi�re couleur sortie du fragment.
		*\param[in] alphaToOneEnable
		*	Dit si la composante alpha de la premi�re couleur sortie du fragment est remplac�e par 1.
		*/
		MultisampleState( MultisampleStateFlags flags
			, SampleCountFlag rasterisationSamples
			, bool sampleShadingEnable
			, float minSampleShading
			, uint32_t sampleMask
			, bool alphaToCoverageEnable
			, bool alphaToOneEnable );
		/**
		*\return
		*	Les indicateurs de l'�tat.
		*/
		inline MultisampleStateFlags getFlags()const
		{
			return m_flags;
		}
		/**
		*\return
		*	Le nombre d'�chantillons par pixel, lors de la rast�risation.
		*/
		inline SampleCountFlag getRasterisationSamples()const
		{
			return m_rasterizationSamples;
		}
		/**
		*\return
		*	D�termine si l'ex�cution du shader de fragment s'ex�cute par fragment (\p false) ou par �chantillon (\p true).
		*/
		inline bool isSampleShadingEnabled()const
		{
			return m_sampleShadingEnable;
		}
		/**
		*\return
		*	La fraction minimale de shader par �chantillon.
		*/
		inline float getMinSampleShading()const
		{
			return m_minSampleShading;
		}
		/**
		*\return
		*	Le masque de couverture, combin� via un ET binaire avec l'information de couverture g�n�r�e lors de la rast�risation.
		*/
		inline uint32_t const & getSampleMask()const
		{
			return m_sampleMask;
		}
		/**
		*\return
		*	Dit si une valeur de couverture temporaire est g�n�r�e, � partir de la composante alpha de la premi�re couleur sortie du fragment.
		*/
		inline bool isAlphaToCoverageEnabled()const
		{
			return m_alphaToCoverageEnable;
		}
		/**
		*\return
		*	Dit si la composante alpha de la premi�re couleur sortie du fragment est remplac�e par 1.
		*/
		inline bool isAlphaToOneEnabled()const
		{
			return m_alphaToOneEnable;
		}

	private:
		MultisampleStateFlags m_flags;
		SampleCountFlag m_rasterizationSamples;
		bool m_sampleShadingEnable;
		float m_minSampleShading;
		uint32_t m_sampleMask;
		bool m_alphaToCoverageEnable;
		bool m_alphaToOneEnable;
	};
}

#endif
