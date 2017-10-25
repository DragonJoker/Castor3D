/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetallicRoughnessPassBuffer_H___
#define ___C3D_MetallicRoughnessPassBuffer_H___

#include "Shader/PassBuffer/PassBuffer.hpp"

#include <Design/ArrayView.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		09/02/2010
	\~english
	\brief		SSBO holding the Passes data.
	\~french
	\brief		SSBO contenant les données des Pass.
	*/
	class MetallicRoughnessPassBuffer
		: public PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API MetallicRoughnessPassBuffer( Engine & engine
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MetallicRoughnessPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( LegacyPass const & pass )override;
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( MetallicRoughnessPbrPass const & pass )override;

	private:
		struct PassData
		{
			RgbaColour albRough;
			RgbaColour metDiv;
			RgbaColour common;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

	private:
		static constexpr uint32_t DataSize = ( sizeof( RgbaColour ) * 4 )
			+ ( PassBuffer::ExtendedDataSize );

	private:
		//!\~english	The metallic/roughness PBR passes data.
		//!\~french		Les données des passes PBR metallic/roughness.
		PassesData m_data;
	};
}

#endif
