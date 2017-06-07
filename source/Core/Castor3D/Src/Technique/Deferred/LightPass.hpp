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
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/Viewport.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/MatrixUbo.hpp"

#include <Miscellaneous/BlockTracker.hpp>

namespace Castor3D
{
	enum class DsTexture
		: uint8_t
	{
		eDepth,
		eNormal,
		eDiffuse,
		eSpecular,
		eEmissive,
		CASTOR_SCOPED_ENUM_BOUNDS( eDepth ),
	};
	Castor::String GetTextureName( DsTexture p_texture );
	Castor::PixelFormat GetTextureFormat( DsTexture p_texture );
	AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture );
	uint32_t GetTextureAttachmentIndex( DsTexture p_texture );
	float GetMaxDistance( LightCategory const & p_light
		, Castor::Point3f const & p_attenuation
		, float p_max );
	void Declare_EncodeMaterial( GLSL::GlslWriter & p_writer );
	void Declare_DecodeMaterial( GLSL::GlslWriter & p_writer );
	void Declare_DecodeReceiver( GLSL::GlslWriter & p_writer );
	void EncodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex
		, GLSL::Float const & p_encoded );
	void DecodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Float const & p_encoded
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex );
	void DecodeReceiver( GLSL::GlslWriter & p_writer
		, GLSL::Float & p_encoded
		, GLSL::Int const & p_receiver );

	class GpInfo
	{
	public:
		GpInfo( Engine & p_engine );
		~GpInfo();
		void Update( Castor::Size const & p_size
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline UniformBuffer & GetUbo()
		{
			return m_gpInfoUbo;
		}

	public:
		static const Castor::String GPInfo;
		static const Castor::String InvViewProj;
		static const Castor::String InvView;
		static const Castor::String InvProj;
		static const Castor::String View;
		static const Castor::String Proj;
		static const Castor::String RenderSize;

	private:
		//!\~english	The uniform buffer containing Geometry pass informations.
		//!\~french		Le tampon d'uniformes contenant les informations de la geometry pass.
		UniformBuffer m_gpInfoUbo;
		//!\~english	The uniform variable containing inverted projection-view matrix.
		//!\~french		La variable uniforme contenant la matrice projection-vue inversée.
		Uniform4x4fSPtr m_invViewProjUniform;
		//!\~english	The uniform variable containing inverted view matrix.
		//!\~french		La variable uniforme contenant la matrice vue inversée.
		Uniform4x4fSPtr m_invViewUniform;
		//!\~english	The uniform variable containing inverted projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection inversés.
		Uniform4x4fSPtr m_invProjUniform;
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Uniform4x4fSPtr m_gViewUniform;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Uniform4x4fSPtr m_gProjUniform;
		//!\~english	The shader variable containing the render area size.
		//!\~french		La variable de shader contenant les dimensions de la zone de rendu.
		Uniform2fSPtr m_renderSize;
	};

	using GeometryPassResult = std::array< TextureUnitUPtr, size_t( DsTexture::eCount ) >;

	class LightPass
	{
	protected:
		struct Program
		{
		public:
			Program( Engine & p_engine
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl );
			virtual ~Program()noexcept;
			void Initialise( VertexBuffer & p_vbo
				, IndexBufferSPtr p_ibo
				, MatrixUbo & p_matrixUbo
				, SceneUbo & p_sceneUbo
				, UniformBuffer & p_gpInfoUbo
				, ModelMatrixUbo * p_modelMatrixUbo );
			void Cleanup();
			void Bind( Light const & p_light );
			void Render( Castor::Size const & p_size
				, Castor::Point3f const & p_colour
				, uint32_t p_count
				, bool p_first )const;

		private:
			virtual RenderPipelineUPtr DoCreatePipeline( bool p_blend ) = 0;
			virtual void DoBind( Light const & p_light ) = 0;

		public:
			//!\~english	The shader program used to render lights.
			//!\~french		Le shader utilisé pour rendre les lumières.
			ShaderProgramSPtr m_program;
			//!\~english	Geometry buffers holder.
			//!\~french		Conteneur de buffers de géométries.
			GeometryBuffersSPtr m_geometryBuffers;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			RenderPipelineSPtr m_blendPipeline;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			RenderPipelineSPtr m_firstPipeline;
			//!\~english	The variable containing the light colour.
			//!\~french		La variable contenant la couleur de la lumière.
			PushUniform3fSPtr m_lightColour;
		};
		using ProgramPtr = std::unique_ptr< Program >;

	public:
		virtual ~LightPass() = default;
		virtual void Initialise( Scene const & p_scene
			, SceneUbo & p_sceneUbo ) = 0;
		virtual void Cleanup() = 0;
		virtual void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Light const & p_light
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool p_first );
		virtual uint32_t GetCount()const = 0;

	protected:
		LightPass( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, bool p_shadows );
		void DoInitialise( Scene const & p_scene
			, LightType p_type
			, VertexBuffer & p_vbo
			, IndexBufferSPtr p_ibo
			, SceneUbo & p_sceneUbo
			, ModelMatrixUbo * p_modelMatrixUbo );
		void DoCleanup();
		virtual void DoUpdate( Castor::Size const & p_size
			, Light const & p_light
			, Camera const & p_camera ) = 0;
		void DoRender( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor::Point3f const & p_colour
			, bool p_first );
		virtual Castor::String DoGetPixelShaderSource( SceneFlags const & p_sceneFlags
			, LightType p_type )const;
		virtual Castor::String DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const = 0;
		virtual ProgramPtr DoCreateProgram( Castor::String const & p_vtx
			, Castor::String const & p_pxl )const = 0;

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	Tells if shadows are enabled.
		//!\~french		Dit si les ombres sont activées.
		bool m_shadows;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target RBO attach.
		//!\~french		L'attache de RBO cible.
		FrameBufferAttachment & m_depthAttach;
		//!\~english	The light pass program.
		//!\~french		Le programme de la passe de lumière.
		ProgramPtr m_program;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		std::unique_ptr< GpInfo > m_gpInfo;
	};
}

#define UBO_GPINFO( p_writer )\
	GLSL::Ubo l_gpInfo{ p_writer, GpInfo::GPInfo };\
	auto c3d_mtxInvViewProj = l_gpInfo.GetUniform< GLSL::Mat4 >( GpInfo::InvViewProj );\
	auto c3d_mtxInvView = l_gpInfo.GetUniform< GLSL::Mat4 >( GpInfo::InvView );\
	auto c3d_mtxInvProj = l_gpInfo.GetUniform< GLSL::Mat4 >( GpInfo::InvProj );\
	auto c3d_mtxGView = l_gpInfo.GetUniform< GLSL::Mat4 >( GpInfo::View );\
	auto c3d_mtxGProj = l_gpInfo.GetUniform< GLSL::Mat4 >( GpInfo::Proj );\
	auto c3d_renderSize = l_gpInfo.GetUniform< GLSL::Vec2 >( GpInfo::RenderSize );\
	l_gpInfo.End()

#endif
