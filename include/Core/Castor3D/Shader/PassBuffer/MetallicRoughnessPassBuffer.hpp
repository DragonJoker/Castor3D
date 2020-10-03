/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetallicRoughnessPassBuffer_H___
#define ___C3D_MetallicRoughnessPassBuffer_H___

#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

namespace castor3d
{
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
			, RenderDevice const & device
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
		C3D_API void visit( PhongPass const & pass )override;
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( MetallicRoughnessPbrPass const & pass )override;

	public:
#if C3D_MaterialsStructOfArrays

		struct PassesData
		{
			castor::ArrayView< RgbaColour > albRough;
			castor::ArrayView< RgbaColour > metDiv;
			castor::ArrayView< RgbaColour > common;
			castor::ArrayView< RgbaColour > reflRefr;
			ExtendedData extended;
		};

#else

		struct PassData
		{
			RgbaColour albRough;
			RgbaColour metDiv;
			RgbaColour common;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

#endif

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
