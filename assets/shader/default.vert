#version 140

// Precision qualifiers are added for code portability with OpenGL ES, not for
// functionality. According to the GLSL 1.30 and later specs: the same object
// declared in different shaders that are linked together must have the same
// precision qualification. This applies to inputs, outputs, uniforms, and
// globals.

precision highp float;


// GLSL 1.30 (OpenGL 3.0) deprecates most of the built-in uniform state
// variables. In GLSL 1.40 (OpenGL 3.1) they have been removed. Typically we
// need to supply projection, view, and model matrices in order to transform
// vertices from model space to clip space.

uniform mat4 ProjectionMatrix = mat4(1.0);
uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);

// GLSL 1.30 (OpenGL 3.0) deprecates vertex shader attribute variables. They
// have been replaced with user-defined generic shader input variables. The
// application code uses the new glVertexAttribPointer() function to map vertex
// data stored in a vertex buffer object to these generic vertex shader input
// variables. This replaces the existing method of specifying vertex array data
// by calling glVertexPointer(), glNormalPointer(), etc.

in vec3 vPosition;
in vec4 vColor;
in vec2 vUV;

// GLSL 1.30 (OpenGL 3.0) also deprecates varying shader variables. This has
// been replaced with user-defined generic shader output variables. The output
// of the vertex shader and the input of the fragment shader form an interface.
// For this interface, vertex shader output variables and fragment shader input
// variables of the same name must match in type and qualification (other than
// out matching to in).

out vec2 vTexCoord;
out vec4 theColor;

void main()
{
    vec4 pos    = vec4(vPosition, 1.0);
    mat4 mvpMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
    gl_Position = mvpMatrix * pos;

    theColor = vColor;
    vTexCoord = vUV;
}
