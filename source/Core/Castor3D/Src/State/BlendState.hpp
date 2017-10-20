/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BLEND_STATE_H___
#define ___C3D_BLEND_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
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
		inline bool isIndependantBlendEnabled()const
		{
			return m_independantBlend;
		}
		/**
		 *\~english
		 *\brief		sets the independant blending activation status.
		 *\param[in]	p_enable	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation du blending indépendant.
		 *\param[in]	p_enable	La nouvelle valeur.
		 */
		inline void enableIndependantBlend( bool p_enable )
		{
			m_independantBlend = p_enable;
		}
		/**
		 *\~english
		 *\return		The blend factors.
		 *\~french
		 *\return		Les facteurs de blend.
		 */
		inline const castor::Colour & getBlendFactors()const
		{
			return m_blendFactors;
		}
		/**
		 *\~english
		 *\brief		sets the blend factors.
		 *\param[in]	p_factors	The new value.
		 *\~french
		 *\brief		Définit les facteurs de blend.
		 *\param[in]	p_factors	La nouvelle valeur.
		 */
		inline void setBlendFactors( const castor::Colour & p_factors )
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
		inline bool isBlendEnabled( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_enableBlend;
		}
		/**
		 *\~english
		 *\brief		sets the blending activation status.
		 *\param[in]	p_enable	The new value.
		 *\param[in]	p_index		The value index.
		 *\~french
		 *\brief		Définit le statut d'activation du blending.
		 *\param[in]	p_enable	La nouvelle valeur.
		 *\param[in]	p_index		L'indice de la valeur.
		 */
		inline void enableBlend( bool p_enable, uint8_t p_index = 0 )
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
		inline BlendOperand getRgbSrcBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbSrcBlend;
		}
		/**
		 *\~english
		 *\brief		sets the RGB source blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setRgbSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
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
		inline BlendOperand getRgbDstBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbDstBlend;
		}
		/**
		 *\~english
		 *\brief		sets the RGB destination blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setRgbDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
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
		inline BlendOperation getRgbBlendOp( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_rgbBlendOp;
		}
		/**
		 *\~english
		 *\brief		sets the RGB blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setRgbBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		sets the RGB blending operation and operands.
		 *\param[in]	p_op	The blend operation.
		 *\param[in]	p_src	The blend source operand.
		 *\param[in]	p_dst	The blend destination operand.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération et les opérandes de blend RGB.
		 *\param[in]	p_op	L'opération de blend.
		 *\param[in]	p_src	L'opérande source du blend.
		 *\param[in]	p_dst	L'opérande destination du blend.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setRgbBlend( BlendOperation p_op, BlendOperand p_src, BlendOperand p_dst, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbBlendOp = p_op;
			m_states[p_index].m_rgbSrcBlend = p_src;
			m_states[p_index].m_rgbDstBlend = p_dst;
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
		inline BlendOperand getAlphaSrcBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaSrcBlend;
		}
		/**
		 *\~english
		 *\brief		sets the alpha source blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setAlphaSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
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
		inline BlendOperand getAlphaDstBlend( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaDstBlend;
		}
		/**
		 *\~english
		 *\brief		sets the alpha destination blending factor.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setAlphaDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
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
		inline BlendOperation getAlphaBlendOp( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_alphaBlendOp;
		}
		/**
		 *\~english
		 *\brief		sets the alpha blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setAlphaBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		sets the alpha blending operation and operands.
		 *\param[in]	p_op	The blend operation.
		 *\param[in]	p_src	The blend source operand.
		 *\param[in]	p_dst	The blend destination operand.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération et les opérandes de blend alpha.
		 *\param[in]	p_op	L'opération de blend.
		 *\param[in]	p_src	L'opérande source du blend.
		 *\param[in]	p_dst	L'opérande destination du blend.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setAlphaBlend( BlendOperation p_op, BlendOperand p_src, BlendOperand p_dst, uint8_t p_index = 0 )
		{
			m_states[p_index].m_alphaBlendOp = p_op;
			m_states[p_index].m_alphaSrcBlend = p_src;
			m_states[p_index].m_alphaDstBlend = p_dst;
		}
		/**
		 *\~english
		 *\brief		sets the source blending factor for both RGB and alpha.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur source pour l'alpha et le RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setSrcBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbSrcBlend = p_value;
			m_states[p_index].m_alphaSrcBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		sets the destination blending factor for both RGB and alpha.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le facteur destination pour l'alpha et le RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setDstBlend( BlendOperand p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_rgbDstBlend = p_value;
			m_states[p_index].m_alphaDstBlend = p_value;
		}
		/**
		 *\~english
		 *\brief		sets the alpha and RGB blending operation.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit l'opération de blend alpha et RGB.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
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
		inline uint8_t getWriteMask( uint8_t p_index = 0 )const
		{
			return m_states[p_index].m_writeMask;
		}
		/**
		 *\~english
		 *\brief		sets the render target write mask.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit le masque d'écriture de la cible de rendu.
		 *\param[in]	p_value	La nouvelle valeur.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		inline void setWriteMask( uint8_t p_value, uint8_t p_index = 0 )
		{
			m_states[p_index].m_writeMask = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the colour writing mask.
		 *\param[in]	p_r, p_g, p_b, p_a The mask for each colour PixelComponents.
		 *\~french
		 *\brief		Définit le masque d'écriture des couleurs.
		 *\param[in]	p_r, p_g, p_b, p_a	Le masque pour chaque composante.
		 */
		inline void setColourMask( WritingMask p_r, WritingMask p_g, WritingMask p_b, WritingMask p_a )
		{
			m_colourMask[0] = p_r;
			m_colourMask[1] = p_g;
			m_colourMask[2] = p_b;
			m_colourMask[3] = p_a;
		}
		/**
		 *\~english
		 *\return		The red PixelComponents writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante rouge.
		 */
		inline WritingMask getColourMaskR()const
		{
			return m_colourMask[0];
		}
		/**
		 *\~english
		 *\return		The green PixelComponents writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante verte.
		 */
		inline WritingMask getColourMaskG()const
		{
			return m_colourMask[1];
		}
		/**
		 *\~english
		 *\return		The blue PixelComponents writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante bleue.
		 */
		inline WritingMask getColourMaskB()const
		{
			return m_colourMask[2];
		}
		/**
		 *\~english
		 *\return		The alpha PixelComponents writing mask.
		 *\~french
		 *\return		Le masque d'écriture de la composante alpha.
		 */
		inline WritingMask getColourMaskA()const
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
		castor::Colour m_blendFactors;
		//!\~english	The blend states.
		//!\~french		Les états de mélange.
		std::array< stRT_BLEND_STATE, 8 > m_states;
		//!\~english	Colours writing mask.
		//!\~french		Masque d'écriture des couleurs.
		WritingMask m_colourMask[4] = { WritingMask::eAll, WritingMask::eAll, WritingMask::eAll, WritingMask::eAll };
	};
}

#endif
