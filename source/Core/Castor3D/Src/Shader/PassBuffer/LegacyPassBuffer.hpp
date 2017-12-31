/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LegacyPassBuffer_H___
#define ___C3D_LegacyPassBuffer_H___

#include "Shader/PassBuffer/PassBuffer.hpp"

#include <Design/ArrayView.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		09/02/2010
	\~english
	\brief		SSBO holding the LegacyPasses data.
	\~french
	\brief		SSBO contenant les données des LegacyPass.
	*/
	class LegacyPassBuffer
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
		C3D_API LegacyPassBuffer( Engine & engine
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~LegacyPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::visit
		 */
		C3D_API void visit( LegacyPass const & pass )override;

	public:
		/*!
		\~english
		\brief		Holds a Legacy pass data.
		\~french
		\brief		Contient les données d'une passe Legacy.
		*/
		struct PassData
		{
			RgbaColour diffAmb;
			RgbaColour specShin;
			RgbaColour common;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

	private:
		static constexpr uint32_t DataSize = ( sizeof( RgbaColour ) * 4 )
			+ ( PassBuffer::ExtendedDataSize );

	private:
		//!\~english	The legacy passes data.
		//!\~french		Les données des passes legacy.
		PassesData m_data;
	};
}

#endif
