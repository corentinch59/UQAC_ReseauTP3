//
// Created by theo on 11/03/2025.
//

#pragma once

#include <raylib.h>

#include <entt/container/dense_map.hpp>
#include <entt/container/dense_set.hpp>
#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

#define DEFAULT_RENDERER_WIDTH 1280
#define DEFAULT_RENDERER_HEIGHT 720

class Renderer
{
public:
    Renderer();

    ~Renderer();

    void Render(entt::registry& world, const Camera& camera);

    Shader GetShader() { return mShadowShader; }

    void UpdateMeshMaterialsToUseCorrectShader(Model& model);

    void SetRenderIntoTexture(bool renderIntoTexture);

    void SetRenderSize(int width, int height);

    RenderTexture2D GetRenderTexture() {
        if (!mRenderTextureInitialized)
        {
            spdlog::error("Retrieving unitialized render texture!");
        }
        return mRenderTexture;
    }

    bool PreloadModel(const std::string& model);

    Model GetModel(const std::string& model);

    int GetWidth() const { if (mShouldRenderIntoTexture) return mRTWidth; else return GetRenderWidth(); }

    int GetHeight() const { if (mShouldRenderIntoTexture) return mRTHeight; else return GetRenderHeight(); }

private:
    Shader mShadowShader;
    RenderTexture2D mShadowMap;
    Camera3D mLightCam;
    Vector3 mLightDir;
    Color mLightColor;
    Vector4 mLightColorNormalized;
    float mAmbient[4];
    int mShadowMapResolution;
    int mLightDirLoc;
    int mLightColLoc;
    int mLightVPLoc;
    int mShadowMapLoc;
    int mAmbientLoc;
    int mShadowMapResolutionLoc;
    Model mSkyboxModel;
    RenderTexture2D mRenderTexture;
    bool mRenderTextureInitialized;
    bool mShouldRenderIntoTexture;
    int mRTWidth, mRTHeight;

    entt::dense_map<std::string, Model> mModels;
    entt::dense_map<std::string, BoundingBox> mBoundingBoxes;
    entt::dense_set<std::string> mUnknownModels;

    void BuildRenderTexture();
    void DestroyRenderTexture();

    void UpdateLightCam();
    void SetShaderUniforms();
};
