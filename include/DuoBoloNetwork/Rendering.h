//
// Created by theo on 11/03/2025.
//

#pragma once

#include <raylib.h>

#include <entt/entity/registry.hpp>

struct Renderable
{
    Model model;
    Color tint = WHITE;
};

class Renderer
{
public:
    Renderer();

    ~Renderer();

    void Render(entt::registry& world, const Camera& camera);

    Shader GetShader() { return mShadowShader; }

    void UpdateMeshMaterialsToUseCorrectShader(Model& model);

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

    void UpdateLightCam();
    void SetShaderUniforms();
};
