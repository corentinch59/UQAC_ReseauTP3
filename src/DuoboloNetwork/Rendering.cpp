//
// Created by theo on 11/03/2025.
//

#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/ShadowShaders.h>
#include <DuoBoloNetwork/SkyboxShaders.h>
#include <DuoBoloNetwork/Helpers.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/RenderableComponent.h>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>


#define SHADOWMAP_RESOLUTION 4096
#define SHADOW_DISTANCE 75.0f

RenderTexture2D LoadDepthRenderTexture(int width, int height);
void UnloadDepthRenderTexture(RenderTexture2D target);

Renderer::Renderer() {
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

    mShadowMap = LoadDepthRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);

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

    // RT
    mRTWidth = DEFAULT_RENDERER_WIDTH;
    mRTHeight = DEFAULT_RENDERER_HEIGHT;

    mShouldRenderIntoTexture = false;

    mRenderTexture = {};
    mRenderTextureInitialized = false;

    // preload models
    Mesh cubeMesh = GenMeshCube(1, 1, 1);
    mModels["cube"] = LoadModelFromMesh(cubeMesh);
    UpdateMeshMaterialsToUseCorrectShader(mModels["cube"]);
    mBoundingBoxes["cube"] = GetModelBoundingBox(mModels["cube"]);

    Mesh planeMesh = GenMeshPlane(1, 1, 1, 1);
    mModels["plane"] = LoadModelFromMesh(planeMesh);
    UpdateMeshMaterialsToUseCorrectShader(mModels["plane"]);
    mBoundingBoxes["plane"] = GetModelBoundingBox(mModels["plane"]);

    Mesh sphereMesh = GenMeshSphere(1.f, 16, 32);
    mModels["sphere"] = LoadModelFromMesh(sphereMesh);
    UpdateMeshMaterialsToUseCorrectShader(mModels["sphere"]);
    mBoundingBoxes["sphere"] = GetModelBoundingBox(mModels["sphere"]);
}

Renderer::~Renderer() {
    for (auto[key, value] : mModels) {
        UnloadModel(value);
    }

    DestroyRenderTexture();
    UnloadShader(mSkyboxModel.materials[0].shader);
    UnloadModel(mSkyboxModel);
    UnloadShader(mShadowShader);
    UnloadDepthRenderTexture(mShadowMap);
}

void Renderer::Render(entt::registry &world, const Camera &camera) {
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

    for (auto && [ entity, transform, renderable ] : view.each()) {
        Model m;
        BoundingBox bb;
        if (PreloadModel(renderable.model)) {
            m = mModels[renderable.model];
            bb = mBoundingBoxes[renderable.model];
        } else {
            m = mModels["cube"];
            bb = mBoundingBoxes["cube"];
        }

        float radius = std::max(Vector3Length(bb.min), Vector3Length(bb.max));
        if (!IsSphereInsideCameraFrustum(mLightCam, radius, transform.position, 1.0f))
            continue;

        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(transform.rotation, &axis, &angle);

        DrawModelEx(m, transform.position, axis, angle * RAD2DEG, transform.scale, renderable.tint);
    }

    EndMode3D();
    EndTextureMode();

    if (mShouldRenderIntoTexture && mRenderTextureInitialized) {
        BeginTextureMode(mRenderTexture);
        ClearBackground(WHITE);
    }

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
    DrawModel(mSkyboxModel, camera.position, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();

    float aspect = 1.0;
    if (mShouldRenderIntoTexture && mRenderTextureInitialized) {
        aspect = (float)mRTWidth / (float)mRTHeight;
    } else {
        aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    }

    for (auto && [ entity, transform, renderable ] : view.each()) {
        Model m;
        BoundingBox bb;
        if (PreloadModel(renderable.model)) {
            m = mModels[renderable.model];
            bb = mBoundingBoxes[renderable.model];
        } else {
            m = mModels["cube"];
            bb = mBoundingBoxes["cube"];
        }

        float radius = std::max(Vector3Length(bb.min), Vector3Length(bb.max));
        if (!IsSphereInsideCameraFrustum(mLightCam, radius, transform.position, aspect))
            continue;

        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(transform.rotation, &axis, &angle);

        DrawModelEx(m, transform.position, axis, angle * RAD2DEG, transform.scale, renderable.tint);
    }

    EndMode3D();

#ifdef WITH_SCE_EDITOR
    DrawFPS(mRTWidth - 95, 10);
#endif

    if (mShouldRenderIntoTexture && mRenderTextureInitialized) {
        EndTextureMode();
    }
}

void Renderer::UpdateMeshMaterialsToUseCorrectShader(Model &model) {
    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].shader = GetShader();
    }
}

void Renderer::SetRenderIntoTexture(bool renderIntoTexture) {
    mShouldRenderIntoTexture = renderIntoTexture;

    if (mShouldRenderIntoTexture) {
        BuildRenderTexture();
    } else {
        DestroyRenderTexture();
    }
}

void Renderer::SetRenderSize(int width, int height) {
    if (mRTWidth == width && mRTHeight == height)
        return;

    mRTWidth = width;
    mRTHeight = height;

    if (mShouldRenderIntoTexture) {
        DestroyRenderTexture();
        BuildRenderTexture();
    }
}

bool Renderer::PreloadModel(const std::string &model) {
    if (mUnknownModels.contains(model))
        return false;

    if (mModels.contains(model))
        return true;

    Model m = LoadModel(model.c_str());

    if (IsModelValid(m)) {
        mModels[model] = m;
        UpdateMeshMaterialsToUseCorrectShader(mModels[model]);
        mBoundingBoxes[model] = GetModelBoundingBox(mModels[model]);
        return true;
    } else {
        mUnknownModels.insert(model);
        return false;
    }
}

Model Renderer::GetModel(const std::string &model) {
    Model m;
    if (PreloadModel(model)) {
        m = mModels[model];
    } else {
        m = mModels["cube"];
    }

    return m;
}

void Renderer::BuildRenderTexture() {
    if (mRenderTextureInitialized) {
        spdlog::error("Render texture already initialized!");
        return;
    }

    mRenderTexture = LoadRenderTexture(mRTWidth, mRTHeight);
    mRenderTextureInitialized = true;
}

void Renderer::DestroyRenderTexture() {
    if (!mRenderTextureInitialized) {
        spdlog::error("Render texture not initialized!");
        return;
    }

    UnloadRenderTexture(mRenderTexture);
    mRenderTexture = {};
    mRenderTextureInitialized = false;
}

void Renderer::UpdateLightCam() {
    mLightCam.target = mLightCam.position + mLightDir;
    mLightCam.projection = CAMERA_ORTHOGRAPHIC;
    mLightCam.up = {0.0f, 1.0f, 0.0f};
    mLightCam.fovy = SHADOW_DISTANCE;
}

void Renderer::SetShaderUniforms() {
    SetShaderValue(mShadowShader, mLightDirLoc, &mLightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(mShadowShader, mLightColLoc, &mLightColorNormalized, SHADER_UNIFORM_VEC4);
    SetShaderValue(mShadowShader, mAmbientLoc, mAmbient, SHADER_UNIFORM_VEC4);
    SetShaderValue(mShadowShader, mShadowMapResolutionLoc, &mShadowMapResolution, SHADER_UNIFORM_INT);
}

RenderTexture2D LoadDepthRenderTexture(int width, int height) {
    RenderTexture2D target = {0};

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0) {
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
    } else
        spdlog::warn("FBO: Framebuffer object can not be created");

    return target;
}

// Unload shadowmap render texture from GPU memory (VRAM)
void UnloadDepthRenderTexture(RenderTexture2D target) {
    if (target.id > 0) {
        // NOTE: Depth texture/renderbuffer is automatically
        // queried and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}
