//
// Created by theo on 11/03/2025.
//

#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/ShadowShaders.h>
#include <DuoBoloNetwork/SkyboxShaders.h>

#include <raymath.h>
#include <rlgl.h>
#include <spdlog/spdlog.h>

#include "DuoBoloNetwork/Transform.h"

#define SHADOWMAP_RESOLUTION 4096
#define SHADOW_DISTANCE 60.0f

RenderTexture2D LoadShadowmapRenderTexture(int width, int height);
void UnloadShadowmapRenderTexture(RenderTexture2D target);

Renderer::Renderer()
{
    // Shadow
    mLightDir = Vector3Normalize({0.35f, -1.0f, -0.35f});
    mLightColor = WHITE;
    mLightColorNormalized = ColorNormalize(mLightColor);
    mAmbient[0] = mAmbient[1] = mAmbient[2] = mAmbient[3] = 0.1f;
    mShadowMapResolution = SHADOWMAP_RESOLUTION;

    mShadowShader = LoadShaderFromMemory(gShadowShaderVertex, gShadowShaderFragment);

    mShadowShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(mShadowShader, "viewPos");

    mLightDirLoc = GetShaderLocation(mShadowShader, "lightDir");
    mLightColLoc = GetShaderLocation(mShadowShader, "lightColor");
    mLightVPLoc = GetShaderLocation(mShadowShader, "lightVP");
    mShadowMapLoc = GetShaderLocation(mShadowShader, "shadowMap");
    mAmbientLoc = GetShaderLocation(mShadowShader, "ambient");
    mShadowMapResolutionLoc = GetShaderLocation(mShadowShader, "shadowMapResolution");

    mShadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);


    // Cubemap
    mSkyboxModel = LoadModelFromMesh(GenMeshCube(1, 1, 1));

    mSkyboxModel.materials[0].shader = LoadShaderFromMemory(gSkyboxVertexShader, gSkyboxFragmentShader);

    int environmentMap = MATERIAL_MAP_CUBEMAP;
    int doGamma = 0;
    int vflipped = 0;
    SetShaderValue(mSkyboxModel.materials[0].shader,
                   GetShaderLocation(mSkyboxModel.materials[0].shader, "environmentMap"),
                   &environmentMap, SHADER_UNIFORM_INT);
    SetShaderValue(mSkyboxModel.materials[0].shader, GetShaderLocation(mSkyboxModel.materials[0].shader, "doGamma"),
                   &doGamma, SHADER_UNIFORM_INT);
    SetShaderValue(mSkyboxModel.materials[0].shader, GetShaderLocation(mSkyboxModel.materials[0].shader, "vflipped"),
                   &vflipped, SHADER_UNIFORM_INT);

    Image img = LoadImage("assets/skybox.png");
    mSkyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    // CUBEMAP_LAYOUT_PANORAMA
    UnloadImage(img);
}

Renderer::~Renderer()
{
    UnloadShader(mSkyboxModel.materials[0].shader);
    UnloadModel(mSkyboxModel);
    UnloadShader(mShadowShader);
    UnloadShadowmapRenderTexture(mShadowMap);
}

void Renderer::Render(entt::registry& world, const Camera& camera)
{
    mLightDir = Vector3Normalize(mLightDir);
    mLightCam.position = camera.position + Vector3Scale(mLightDir, -15.0f);
    UpdateLightCam();

    SetShaderUniforms();

    auto view = world.view<TransformComponent, RenderableComponent>();

    Matrix lightView;
    Matrix lightProj;
    BeginTextureMode(mShadowMap);
    ClearBackground(WHITE);
    BeginMode3D(mLightCam);
    lightView = rlGetMatrixModelview();
    lightProj = rlGetMatrixProjection();

    for (auto&& [entity, transform, renderable] : view.each())
    {
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(transform.rotation, &axis, &angle);

        DrawModelEx(renderable.model, transform.position, axis, angle * RAD2DEG, {1, 1, 1}, renderable.tint);
    }

    EndMode3D();
    EndTextureMode();
    Matrix lightViewProj = MatrixMultiply(lightView, lightProj);

    SetShaderValue(mShadowShader, mShadowShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValueMatrix(mShadowShader, mLightVPLoc, lightViewProj);
    SetShaderUniforms();

    rlEnableShader(mShadowShader.id);
    int slot = 10; // Can be anything 0 to 15, but 0 will probably be taken up
    rlActiveTextureSlot(10);
    rlEnableTexture(mShadowMap.depth.id);
    rlSetUniform(mShadowMapLoc, &slot, SHADER_UNIFORM_INT, 1);

    BeginMode3D(camera);

    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(mSkyboxModel, {0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();

    for (auto&& [entity, transform, renderable] : view.each())
    {
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(transform.rotation, &axis, &angle);

        DrawModelEx(renderable.model, transform.position, axis, angle * RAD2DEG, {1, 1, 1}, renderable.tint);
    }

    EndMode3D();
}

void Renderer::UpdateMeshMaterialsToUseCorrectShader(Model& model)
{
    for (int i = 0; i < model.materialCount; i++)
    {
        model.materials[i].shader = GetShader();
    }
}

void Renderer::UpdateLightCam()
{
    mLightCam.target = mLightCam.position + mLightDir;
    mLightCam.projection = CAMERA_ORTHOGRAPHIC;
    mLightCam.up = {0.0f, 1.0f, 0.0f};
    mLightCam.fovy = SHADOW_DISTANCE;
}

void Renderer::SetShaderUniforms()
{
    SetShaderValue(mShadowShader, mLightDirLoc, &mLightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(mShadowShader, mLightColLoc, &mLightColorNormalized, SHADER_UNIFORM_VEC4);
    SetShaderValue(mShadowShader, mAmbientLoc, mAmbient, SHADER_UNIFORM_VEC4);
    SetShaderValue(mShadowShader, mShadowMapResolutionLoc, &mShadowMapResolution, SHADER_UNIFORM_INT);
}

RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
{
    RenderTexture2D target = {0};

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        // Create depth texture
        // We don't need a color texture for the shadowmap
        target.depth.id = rlLoadTextureDepth(width, height, false);
        rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
        rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19; //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        // Attach depth texture to FBO
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id))
            TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    }
    else spdlog::warn("FBO: Framebuffer object can not be created");

    return target;
}

// Unload shadowmap render texture from GPU memory (VRAM)
void UnloadShadowmapRenderTexture(RenderTexture2D target)
{
    if (target.id > 0)
    {
        // NOTE: Depth texture/renderbuffer is automatically
        // queried and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}
