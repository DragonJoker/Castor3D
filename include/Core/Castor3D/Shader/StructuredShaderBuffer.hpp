/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StructuredShaderBuffer_H___
#define ___C3D_StructuredShaderBuffer_H___

#include "Castor3D/Shader/ShaderBuffer.hpp"

namespace castor3d
{
	template< typename ElementTypeTraits >
	class StructuredShaderBuffer
	{
	public:
		using ElementType = typename ElementTypeTraits::ElementType;
		static uint32_t constexpr ElementSize = ElementTypeTraits::ElementSize;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	buffer		The shader buffer.
		 *\param[in]	elementID	The ID for the next element to add.
		 *\param[in]	count		The maximum number of elements.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	buffer		Le tampon shader.
		 *\param[in]	elementID	L'ID du prochain élément à ajouter.
		 *\param[in]	count		Le nombre maximal d'éléments.
		 */
		C3D_API StructuredShaderBuffer( Engine & engine
			, ShaderBuffer & buffer
			, uint32_t & elementID
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds an element to the buffer.
		 *\remarks		Sets the element's ID.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Ajoute un élément au tampon.
		 *\remarks		Définit l'ID de l'élément.
		 *\param[in]	element	L'élément.
		 */
		C3D_API uint32_t add( ElementType & element );
		/**
		 *\~english
		 *\brief		Removes an element from the buffer.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Supprime un élément du tampon.
		 *\param[in]	element	L'élément.
		 */
		C3D_API void remove( ElementType & element );
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Binds the buffer.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Active le tampon.
		 *\param[in]	index	Le point d'activation.
		 */
		C3D_API void bind( uint32_t index )const;

	protected:
		//!\~english	The ShaderBuffer.
		//!\~french		Le ShaderBuffer.
		ShaderBuffer & m_buffer;
		//!\~english	The next element ID.
		//!\~french		L'ID de l'élément suivant.
		uint32_t & m_elementID;
		//!\~english	The traits.
		//!\~french		Les traits.
		ElementTypeTraits m_traits;
		//!\~english	The current elements.
		//!\~french		Les éléments actuels.
		castor::Vector< ashes::AttributeFormat * > m_elements;
		//!\~english	The modified elements.
		//!\~french		Les éléments modifiés.
		castor::Vector< ashes::AttributeFormat const * > m_dirty;
		//!\~english	The maximum element count.
		//!\~french		Le nombre maximal d'éléments.
		uint32_t const m_maxElemCount;
		//!\~english	The connections to change signal for current elements.
		//!\~french		Les connexions aux signaux de changement des éléments actuels.
		castor::Vector< typename ElementType::OnChangedConnection > m_connections;
	};
}

#include "StructuredShaderBuffer.inl"

#endif
