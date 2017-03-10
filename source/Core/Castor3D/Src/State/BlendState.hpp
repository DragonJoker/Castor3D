/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_BLEND_STATE_H___
#define ___C3D_BLEND_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Blending configurations grouping class.
	\~french
	\brief		Classe regroupant les configurations de blend.
	*/
	class BlendState
	{
	public:
		/**
		 *\~english
		 *\return		The independant blending activation status.
		 *\~french
		 *\return		Le statut d'activation du blending indépendant.
		 */
		inline bool IsIndependantBlendEnabled()const
		{
			return m_independantBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the independant blending activation status.
		 *\param[in]	p_enable	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation du blending indépendant.
		 *\param[in]	p_enable	La nouvelle valeur.
		 */
		inline void EnableIndependantBlend( bool p_enable )
		{
			m_independantBlend = p_enable;
		}
		/**
		 *\~english
		 *\return		The blend factors.
		 *\~french
		 *\return		Les facteurs de blend.
		 */
		inline const Castor::Colour & GetBlendFactors()const
		{
			return m_blendFactors;
		}
		/**
		 *\~english
		 *\brief		Sets the blend factors.
		 *\param[in]	p_factors	The new value.
		 *\~french
		 *\brief		Définit les facteurs de blend.
		 *\param[in]	p_factors	La nouvelle valeur.
		 */
		inline void SetBlendFactors( const Castor::Colour & p_factors )
		{
			m_blendFactors = p_factors;
		}
		/**
		 *\~english
		 *\brief		Retrieves the blending activation status.
		 *\param[in]	p_index		The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le statut d'activation du blending.
		 *\param[in]	p_index		L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline bool IsBlendEnabled( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_enableBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the blending activation status.
		 *\param[in]	p_enable	The new value.
		 *\param[in]	p_index		The value index.
		 *\~french
		 *\brief		Définit le statut d'activation du blending.
		 *\param[in]	p_enable	La nouvelle valeur.
		 *\param[in]	p_index		L'indice de la valeur.
		 */
		inline void EnableBlend( bool p_enable, uint8_t p_index = 0 )
		{
			m_states[p_index].m_enableBlend = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB source blending factor.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le facteur source RGB.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperand GetRgbSrcBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbSrcBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB source blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetRgbSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbSrcBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB destination blending factor.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le facteur destination RGB.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperand GetRgbDstBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbDstBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB destination blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetRgbDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbDstBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB blending operation.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'opération de blend RGB.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperation GetRgbBlendOp( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbBlendOp;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetRgbBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha source blending factor.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le facteur source alpha.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperand GetAlphaSrcBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaSrcBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha source blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetAlphaSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaSrcBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha destination blending factor.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le facteur destination alpha.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperand GetAlphaDstBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaDstBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha destination blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetAlphaDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaDstBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha blending operation.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'opération de blend alpha.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline BlendOperation GetAlphaBlendOp( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaBlendOp;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetAlphaBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the source blending factor for both RGB and alpha.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source pour l'alpha et le RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbSrcBlend = p_value;
			m_states[p_index].m_alphaSrcBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the destination blending factor for both RGB and alpha.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination pour l'alpha et le RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbDstBlend = p_value;
			m_states[p_index].m_alphaDstBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha and RGB blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend alpha et RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaBlendOp = p_value;
			m_states[p_index].m_alphaBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the render target write mask.
		 *\param[in]	p_index	The value index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le masque d'écriture de la cible de rendu.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur.
		 */
		inline uint8_t GetWriteMask( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_writeMask;
		}
		/**
		 *\~english
		 *\brief		Sets the render target write mask.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le masque d'écriture de la cible de rendu.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void SetWriteMask( uint8_t p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_writeMask = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the colour writing mask.
		 *\param[in]	p_r, p_g, p_b, p_a The mask for each colour component.
		 *\~french
		 *\brief		Définit le masque d'écriture des couleurs.
		 *\param[in]	p_r, p_g, p_b, p_a	Le masque pour chaque composante.
		 */
		inline void SetColourMask( WritingMask p_r, WritingMask p_g, WritingMask p_b, WritingMask p_a )
		{
			m_colourMask[0] = p_r;
			m_colourMask[1] = p_g;
			m_colourMask[2] = p_b;
			m_colourMask[3] = p_a;
		}
		/**
		 *\~english
		 *\return		The red component writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante rouge.
		 */
		inline WritingMask GetColourMaskR()const
		{
			return m_colourMask[0];
		}
		/**
		 *\~english
		 *\return		The green component writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante verte.
		 */
		inline WritingMask GetColourMaskG()const
		{
			return m_colourMask[1];
		}
		/**
		 *\~english
		 *\return		The blue component writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante bleue.
		 */
		inline WritingMask GetColourMaskB()const
		{
			return m_colourMask[2];
		}
		/**
		 *\~english
		 *\return		The alpha component writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante alpha.
		 */
		inline WritingMask GetColourMaskA()const
		{
			return m_colourMask[3];
		}

	protected:
		struct C3D_API stRT_BLEND_STATE
		{
			bool m_enableBlend{ false };
			BlendOperand m_rgbSrcBlend{ BlendOperand::eOne };
			BlendOperand m_rgbDstBlend{ BlendOperand::eZero };
			BlendOperation m_rgbBlendOp{ BlendOperation::eAdd };
			BlendOperand m_alphaSrcBlend{ BlendOperand::eOne };
			BlendOperand m_alphaDstBlend{ BlendOperand::eZero };
			BlendOperation m_alphaBlendOp{ BlendOperation::eAdd };
			uint8_t m_writeMask{ 0xFF };
		};

	protected:
		//!\~english	Tells if the independant blend states are activated.
		//!\~french		Dit si les états indépendants de mélange sont activés.
		bool m_independantBlend{ false };
		//!\~english	Le blend colour.
		//!\~french		La couleur de mélange.
		Castor::Colour m_blendFactors;
		//!\~english	The blend states.
		//!\~french		Les états de mélange.
		std::array< stRT_BLEND_STATE, 8 > m_states;
		//!\~english	Colours writing mask.
		//!\~french		Masque d'écriture des couleurs.
		WritingMask m_colourMask[4] = { WritingMask::eAll, WritingMask::eAll, WritingMask::eAll, WritingMask::eAll };
	};
}

#endif
