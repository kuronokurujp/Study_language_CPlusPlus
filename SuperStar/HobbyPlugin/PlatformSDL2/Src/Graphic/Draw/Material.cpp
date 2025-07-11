#include "PlatformSDL2/Graphic/Draw/Material.h"

#include "PlatformSDL2/Graphic/Draw/Texture.h"
#include "Engine/Math/Math.h"
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    /// <summary>
    /// OpenGLのシェーダー制御クラス
    /// </summary>
    class OpenGLShader
    {
    public:
        OpenGLShader() { this->_Clear(); }
        ~OpenGLShader() { this->Unload(); }

        // シェーダーをロード
        HE::Bool Load(const HE::Char* in_rVertName, const HE::Char* in_rFragName)
        {
            auto bVertexLoad =
                this->_CompileShader(in_rVertName, GL_VERTEX_SHADER, this->vertexShader);
            auto bFragmentLoad =
                this->_CompileShader(in_rFragName, GL_FRAGMENT_SHADER, this->fragShader);
            if (!bVertexLoad || !bFragmentLoad)
            {
                return FALSE;
            }

            // 頂点とフラグメントのシェーダーコンパイルが出来た

            // 頂点とフラグメントのシェーダを一つにまとめる
            this->shaderProgram = glCreateProgram();
            glAttachShader(this->shaderProgram, this->vertexShader);
            glAttachShader(this->shaderProgram, this->fragShader);
            glLinkProgram(this->shaderProgram);

            GLint linkStatus;
            glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE)
            {
                char infoLog[512];
                glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
                HE_LOG_LINE(HE_STR_TEXT("Shader Program Link Error: %s"), infoLog);
            }

            return this->_IsValidProgram();
        }

        // シェーダーをアンロード
        void Unload()
        {
            HE_LOG_LINE("unload shader");
            if (this->shaderProgram != GL_NONE)
            {
                glDeleteProgram(this->shaderProgram);
                this->shaderProgram = GL_NONE;
            }

            if (this->vertexShader != GL_NONE)
            {
                glDeleteShader(this->vertexShader);
                this->vertexShader = GL_NONE;
            }

            if (this->fragShader != GL_NONE)
            {
                glDeleteShader(this->fragShader);
                this->fragShader = GL_NONE;
            }
        }

        // シェーダープログラム内のuniformをリストアップする関数
        void LogUniformNames()
        {
#ifdef HE_ENGINE_DEBUG
            GLint numUniforms = 0;
            auto program      = this->shaderProgram;
            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

            GLint maxNameLength = 0;
            glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

            std::vector<GLchar> uniformName(maxNameLength);

            HE_LOG_LINE(HE_STR_TEXT("Shader Uniforms:"));
            for (GLint i = 0; i < numUniforms; ++i)
            {
                GLint size;
                GLenum type;
                glGetActiveUniform(program, i, maxNameLength, nullptr, &size, &type,
                                   uniformName.data());

                // Uniformのロケーションを取得
                GLint location = glGetUniformLocation(program, uniformName.data());

                HE_LOG_LINE(HE_STR_TEXT("Name %s. Type %d. Size %d. Location %d."),
                            uniformName.data(), type, size, location);
            }
#endif
        }

        // シェーダープログラム内の属性をリストアップする関数
        void LogAttributeNames()
        {
#ifdef HE_ENGINE_DEBUG
            GLint numAttributes = 0;
            auto program        = this->shaderProgram;

            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);

            GLint maxNameLength = 0;
            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

            std::vector<GLchar> attributeName(maxNameLength);

            HE_LOG_LINE(HE_STR_TEXT("Shader Attributes:"));
            for (GLint i = 0; i < numAttributes; ++i)
            {
                GLint size;
                GLenum type;
                glGetActiveAttrib(program, i, maxNameLength, nullptr, &size, &type,
                                  attributeName.data());

                // 属性の位置を取得
                GLint location = glGetAttribLocation(program, attributeName.data());

                HE_LOG_LINE(HE_STR_TEXT("Name: %s, Type: %d Location: %d"), attributeName.data(),
                            type, location);
            }
#endif
        }

        // シェーダーを有効化
        void Enable()
        {
            glUseProgram(this->shaderProgram);
            HE_ASSERT(glGetError() == GL_NO_ERROR);
        }

        // シェーダーに行列を渡す
        void SetMatrixUniform(const HE::UTF8* in_pName, const Core::Math::Matrix4* in_pMat)
        {
            // TODO: 失敗した場合を今は考えないで後で対処
            GLint loc = glGetUniformLocation(this->shaderProgram, in_pName);
            HE_ASSERT(loc != -1);

#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Location", in_pName);
#endif

            glUniformMatrix4fv(loc, 1, GL_TRUE,
                               // 行列クラスのfloat配列の先頭ポインタを渡す
                               in_pMat->GetAsFloatPtr());

#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Matrix4fv", in_pName);
#endif
        }

        // シェーダーにテクスチャを渡す
        void SetTextureUniform(const HE::UTF8* in_pName, const HE::Uint32 in_textureID)
        {
            int textureLocation = glGetUniformLocation(this->shaderProgram, in_pName);

#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Location Texture", in_pName);
#endif
            glUniform1i(textureLocation, in_textureID);
#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("texture", in_pName);
#endif
        }

        // シェーダーにベクトル3dを渡す
        void SetVector3Uniform(const HE::UTF8* in_pName, const Core::Math::Vector3& in_rVec3)
        {
            GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);

#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Location", in_pName);
#endif

            glUniform3fv(loc, 1, in_rVec3.GetAsFloatPtr());
#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("3fv", in_pName);
#endif
        }

        // シェーダーにベクトル4dを渡す
        void SetVector4Uniform(const HE::UTF8* in_pName, const Core::Math::Vector4& in_rVec4)
        {
            GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);

#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Location", in_pName);
#endif

            glUniform4fv(loc, 1, in_rVec4.GetAsFloatPtr());
#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("4fv", in_pName);
#endif
        }

        // シェーダーにfloatを渡す
        void SetFloatUniform(const HE::UTF8* in_pName, const HE::Float32 in_value)
        {
            GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);
#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("Location", in_pName);
#endif

            glUniform1f(loc, in_value);
#ifdef HE_ENGINE_DEBUG
            this->_ValidateUniformByDebug("1f", in_pName);
#endif
        }

    private:
        // シェーダーをコンパイル
        HE::Bool _CompileShader(const HE::Char* in_pShaderCode, GLenum in_shaderType,
                                GLuint& in_rOutShader)
        {
            const HE::UTF8* szContensChar = in_pShaderCode;

            // shaderオブジェクト作成
            in_rOutShader = glCreateShader(in_shaderType);
            // シェーダー文字列を指定
            glShaderSource(in_rOutShader, 1, &(szContensChar), nullptr);
            // シェーダーコンパイル
            glCompileShader(in_rOutShader);

            // コンパイル成功かチェック
            if (this->_IsCompiled(in_rOutShader) == FALSE)
            {
                HE_ASSERT(0);
                return FALSE;
            }

            // コンパイル成功

            return TRUE;
        }

        // シェーダーがコンパイルしているか
        HE::Bool _IsCompiled(GLuint in_shader)
        {
            GLint status;
            // コンパイル状態を取得
            glGetShaderiv(in_shader, GL_COMPILE_STATUS, &status);
            if (status == GL_TRUE)
            {
                return TRUE;
            }

            // コンパイル失敗
#ifdef HE_ENGINE_DEBUG
            HE::UTF8 szBuffer[512];
            ::memset(szBuffer, 0, 512);
            glGetShaderInfoLog(in_shader, 511, NULL, szBuffer);
            HE_LOG_LINE(HE_STR_TEXT("GLSL Compile Failed: %s"), szBuffer);
#endif

            return FALSE;
        }

        // シェーダーが有効かチェック
        HE::Bool _IsValidProgram()
        {
            GLint status;
            // コンパイル状態を取得
            glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &status);
            if (status == GL_TRUE)
            {
                return TRUE;
            }

            // コンパイル失敗
#ifdef HE_ENGINE_DEBUG
            // TODO: ログファイルにも出したいな
            HE::UTF8 szBuffer[512];
            ::memset(szBuffer, 0, 512);
            glGetProgramInfoLog(this->shaderProgram, 511, nullptr, szBuffer);
            HE_LOG_LINE(HE_STR_TEXT("GLSL Link Failed Status: %s"), szBuffer);
#endif

            return FALSE;
        }

#ifdef HE_ENGINE_DEBUG
        void _ValidateUniformByDebug(const HE::UTF8* in_szUniformName, const HE::UTF8* in_pAttrName)
        {
            switch (glGetError())
            {
                case GL_INVALID_VALUE:
                {
                    HE_LOG_LINE(HE_STR_TEXT("shader uniform invalid value: %s uniform name => %s"),
                                in_szUniformName, in_pAttrName);
                    break;
                }
                case GL_INVALID_OPERATION:
                {
                    HE_LOG_LINE(HE_STR_TEXT(
                                    "shader uniform invalid operation: %s uniform name => %s"),
                                in_szUniformName, in_pAttrName);
                    break;
                }
                default:
                    break;
            }
        }
#endif

        void _Clear()
        {
            vertexShader  = GL_NONE;
            fragShader    = GL_NONE;
            shaderProgram = GL_NONE;
        }

    private:
        GLuint vertexShader;
        GLuint fragShader;
        GLuint shaderProgram;
    };

    HE::Bool Material::LoadShader(const HE::Char* in_pVertexMem, const HE::Char* in_pPixelMem)
    {
        OpenGLShader* pShader = HE_NEW_MEM(OpenGLShader, 0);
        pShader->Load(in_pVertexMem, in_pPixelMem);

        this->_pShader = reinterpret_cast<void*>(pShader);

        return TRUE;
    }

    void Material::UnLoadShader()
    {
        if (this->_pShader == NULL) return;

        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->Unload();

        HE_SAFE_DELETE_MEM(this->_pShader);
    }

    void Material::Enable()
    {
        // OpenGLのシェーダを有効
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->Enable();
    }

    void Material::Disable()
    {
    }

    void Material::SetPropertyMatrix(const HE::UTF8* in_pName, const Core::Math::Matrix4& in_rMat)
    {
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->SetMatrixUniform(in_pName, &in_rMat);
    }

    void Material::SetPropertyVector3(const HE::UTF8* in_pName, const Core::Math::Vector3& in_rVec3)
    {
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->SetVector3Uniform(in_pName, in_rVec3);
    }

    void Material::SetPropertyVector4(const HE::UTF8* in_pName, const Core::Math::Vector4& in_rVec4)
    {
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->SetVector4Uniform(in_pName, in_rVec4);
    }

    void Material::SetPropertyFloat(const HE::UTF8* in_pName, const HE::Float32 in_value)
    {
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->SetFloatUniform(in_pName, in_value);
    }

    void Material::SetPropertyTexture(const HE::UTF8* in_pName, const TextureBase* in_pTexture)
    {
        OpenGLShader* pShader = reinterpret_cast<OpenGLShader*>(this->_pShader);
        pShader->SetTextureUniform(in_pName, in_pTexture->Handle());
    }

    HE::Bool Material::VRelease()
    {
        this->UnLoadShader();
        return TRUE;
    }

    void FontMaterial::CopyGlyphs(GlyphMap&& in_rrGlyphs)
    {
        this->_mGlyphs = std::move(in_rrGlyphs);
    }

    HE::Bool FontMaterial::VRelease()
    {
        Material::VRelease();
        this->_mGlyphs.clear();

        return TRUE;
    }

    const FontMaterial::Glyph* FontMaterial::GetGlyph(const HE::Uint32 in_uUnicode)
    {
        if (this->_mGlyphs.find(in_uUnicode) == this->_mGlyphs.end()) return NULL;

        return &this->_mGlyphs[in_uUnicode];
    }
}  // namespace PlatformSDL2
