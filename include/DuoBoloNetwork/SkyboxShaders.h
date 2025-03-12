//
// Created by theo on 12/03/2025.
//

#pragma once

inline const char* gSkyboxVertexShader =
"#version 330\n"
"\n"
"// Input vertex attributes\n"
"in vec3 vertexPosition;\n"
"\n"
"// Input uniform values\n"
"uniform mat4 matProjection;\n"
"uniform mat4 matView;\n"
"\n"
"// Output vertex attributes (to fragment shader)\n"
"out vec3 fragPosition;\n"
"\n"
"void main()\n"
"{\n"
"    // Calculate fragment position based on model transformations\n"
"    fragPosition = vertexPosition;\n"
"\n"
"    // Remove translation from the view matrix\n"
"    mat4 rotView = mat4(mat3(matView));\n"
"    vec4 clipPos = matProjection*rotView*vec4(vertexPosition, 1.0);\n"
"\n"
"    // Calculate final vertex position\n"
"    gl_Position = clipPos;\n"
"}\n";


inline const char* gSkyboxFragmentShader =
"#version 330\n"
"\n"
"// Input vertex attributes (from vertex shader)\n"
"in vec3 fragPosition;\n"
"\n"
"// Input uniform values\n"
"uniform samplerCube environmentMap;\n"
"uniform bool vflipped;\n"
"uniform bool doGamma;\n"
"\n"
"// Output fragment color\n"
"out vec4 finalColor;\n"
"\n"
"void main()\n"
"{\n"
"    // Fetch color from texture map\n"
"    vec3 color = vec3(0.0);\n"
"\n"
"    if (vflipped) color = texture(environmentMap, vec3(fragPosition.x, -fragPosition.y, fragPosition.z)).rgb;\n"
"    else color = texture(environmentMap, fragPosition).rgb;\n"
"\n"
"    if (doGamma)// Apply gamma correction\n"
"    {\n"
"        color = color/(color + vec3(1.0));\n"
"        color = pow(color, vec3(1.0/2.2));\n"
"    }\n"
"\n"
"    // Calculate final fragment color\n"
"    finalColor = vec4(color, 1.0);\n"
"}\n";