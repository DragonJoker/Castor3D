/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StructuredShaderBuffer_H___
#define ___C3D_StructuredShaderBuffer_H___

#include "Shader/ShaderBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Wrapper class to select between SSBO or TBO.
	\remarks	Allows to user either one or the other in the same way.
	\~french
	\brief		Classe permettant de choisir entre SSBO et TBO.
	\remarks	Permet d'utiliser l'un comme l'autre de la même manière.
	*/
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
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The maximum number of elements.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal d'éléments.
		 */
		C3D_API StructuredShaderBuffer( Engine & engine
			, ShaderBuffer & buffer
			, uint32_t & elementID
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~StructuredShaderBuffer();
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
		std::vector< ElementType * > m_elements;
		//!\~english	The modified elements.
		//!\~french		Les éléments modifiés.
		std::vector< ElementType const * > m_dirty;
		//!\~english	The maximum element count.
		//!\~french		Le nombre maximal d'éléments.
		uint32_t const m_maxElemCount;
		//!\~english	The connections to change signal for current elements.
		//!\~french		Les connexions aux signaux de changement des éléments actuels.
		std::vector< typename ElementType::OnChangedConnection > m_connections;
	};
}

#include "StructuredShaderBuffer.inl"

#endif
