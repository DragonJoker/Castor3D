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
#ifndef ___C3D_LightPass_H___
#define ___C3D_LightPass_H___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>

#include <Miscellaneous/BlockTracker.hpp>

namespace deferred_common
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
	Castor3D::AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture );
	uint32_t GetTextureAttachmentIndex( DsTexture p_texture );
	float GetMaxDistance( Castor3D::LightCategory const & p_light
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
		GpInfo( Castor3D::Engine & p_engine );
		~GpInfo();
		void Update( Castor::Size const & p_size
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline Castor3D::UniformBuffer & GetUbo()
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
		Castor3D::UniformBuffer m_gpInfoUbo;
		//!\~english	The uniform variable containing inverted projection-view matrix.
		//!\~french		La variable uniforme contenant la matrice projection-vue inversée.
		Castor3D::Uniform4x4fSPtr m_invViewProjUniform;
		//!\~english	The uniform variable containing inverted view matrix.
		//!\~french		La variable uniforme contenant la matrice vue inversée.
		Castor3D::Uniform4x4fSPtr m_invViewUniform;
		//!\~english	The uniform variable containing inverted projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection inversés.
		Castor3D::Uniform4x4fSPtr m_invProjUniform;
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Castor3D::Uniform4x4fSPtr m_gViewUniform;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Castor3D::Uniform4x4fSPtr m_gProjUniform;
		//!\~english	The shader variable containing the render area size.
		//!\~french		La variable de shader contenant les dimensions de la zone de rendu.
		Castor3D::Uniform2fSPtr m_renderSize;
	};

	using GeometryPassResult = std::array< Castor3D::TextureUnitUPtr, size_t( DsTexture::eCount ) >;

	class LightPass
	{
	protected:
		struct Program
		{
		public:
			Program( Castor3D::Scene const & p_scene
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl
				, bool p_ssao );
			virtual ~Program()noexcept;
			void Initialise( Castor3D::VertexBuffer & p_vbo
				, Castor3D::IndexBufferSPtr p_ibo
				, Castor3D::MatrixUbo & p_matrixUbo
				, Castor3D::SceneUbo & p_sceneUbo
				, Castor3D::UniformBuffer & p_gpInfoUbo
				, Castor3D::ModelMatrixUbo * p_modelMatrixUbo );
			void Cleanup();
			void Render( Castor::Size const & p_size
				, Castor3D::Light const & p_light
				, uint32_t p_count
				, bool p_first );

		private:
			virtual Castor3D::RenderPipelineUPtr DoCreatePipeline( bool p_blend ) = 0;
			virtual void DoBind( Castor3D::Light const & p_light ) = 0;

		public:
			//!\~english	The shader program used to render lights.
			//!\~french		Le shader utilisé pour rendre les lumières.
			Castor3D::ShaderProgramSPtr m_program;
			//!\~english	Geometry buffers holder.
			//!\~french		Conteneur de buffers de géométries.
			Castor3D::GeometryBuffersSPtr m_geometryBuffers;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			Castor3D::RenderPipelineSPtr m_blendPipeline;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			Castor3D::RenderPipelineSPtr m_firstPipeline;
			//!\~english	The variable containing the light colour.
			//!\~french		La variable contenant la couleur de la lumière.
			Castor3D::PushUniform3fSPtr m_lightColour;
			//!\~english	The variable containing the light intensities.
			//!\~french		La variable contenant les intensités de la lumière.
			Castor3D::PushUniform3fSPtr m_lightIntensity;
		};
		using ProgramPtr = std::unique_ptr< Program >;
		using Programs = std::array< ProgramPtr, size_t( GLSL::FogType::eCount ) >;

	public:
		virtual ~LightPass() = default;
		virtual void Initialise( Castor3D::Scene const & p_scene
			, Castor3D::SceneUbo & p_sceneUbo ) = 0;
		virtual void Cleanup() = 0;
		virtual void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, GLSL::FogType p_fogType
			, Castor3D::TextureUnit const * p_ssao
			, bool p_first );
		virtual uint32_t GetCount()const = 0;

	protected:
		LightPass( Castor3D::Engine & p_engine
			, Castor3D::FrameBuffer & p_frameBuffer
			, Castor3D::FrameBufferAttachment & p_depthAttach
			, bool p_ssao
			, bool p_shadows );
		void DoInitialise( Castor3D::Scene const & p_scene
			, Castor3D::LightType p_type
			, Castor3D::VertexBuffer & p_vbo
			, Castor3D::IndexBufferSPtr p_ibo
			, Castor3D::SceneUbo & p_sceneUbo
			, Castor3D::ModelMatrixUbo * p_modelMatrixUbo );
		void DoCleanup();
		virtual void DoUpdate( Castor::Size const & p_size
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera ) = 0;
		void DoRender( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::Light const & p_light
			, GLSL::FogType p_fogType
			, Castor3D::TextureUnit const * p_ssao
			, bool p_first );
		Castor::String DoGetPixelShaderSource( Castor3D::SceneFlags const & p_sceneFlags
			, Castor3D::LightType p_type )const;
		virtual Castor::String DoGetVertexShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const = 0;
		virtual ProgramPtr DoCreateProgram( Castor3D::Scene const & p_scene
			, Castor::String const & p_vtx
			, Castor::String const & p_pxl )const = 0;

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Castor3D::Engine & m_engine;
		//!\~english	Tells if SSAO is enabled.
		//!\~french		Dit si le SSAO est activé.
		bool m_ssao;
		//!\~english	Tells if shadows are enabled.
		//!\~french		Dit si les ombres sont activées.
		bool m_shadows;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		Castor3D::MatrixUbo m_matrixUbo;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		Castor3D::FrameBuffer & m_frameBuffer;
		//!\~english	The target RBO attach.
		//!\~french		L'attache de RBO cible.
		Castor3D::FrameBufferAttachment & m_depthAttach;
		//!\~english	The light pass' programs.
		//!\~french		Les programme de la passe de lumière.
		Programs m_programs;
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
