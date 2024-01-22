#version 460

#define USING_VERTEX_TEXTURE_UV

#include uniform/Base3D.glsl
#include uniform/Model3D.glsl

#include globals/Vertex3DInputs.glsl
#include globals/Vertex3DOutputs.glsl

void main()
{
    mat4 modelMatrix = _modelMatrix;
    #include code/SetVertex3DOutputs.glsl
    gl_Position = _cameraMatrix * vec4(position, 1.0);
};