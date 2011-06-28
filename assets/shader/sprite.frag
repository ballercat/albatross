#version 140
// Precision qualifiers are added for code portability with OpenGL ES, not for
// functionality. According to the GLSL 1.30 and later specs: the same object
// declared in different shaders that are linked together must have the same
// precision qualification. This applies to inputs, outputs, uniforms, and
// globals.

precision highp float;

uniform sampler2D ColorMapSampler;
uniform vec4 SpriteColor = vec4(1.0);

// Fragment shader input variable declarations must exactly match the vertex
// shader's output variable declarations. The output of the vertex shader and
// the input of the fragment shader form an interface. For this interface,
// vertex shader output variables and fragment shader input variables of the
// same name must match in type and qualification (other than out matching to
// in).

in vec2 vTexCoord;

// GLSL 1.3 deprecates gl_FragColor. Its replacement is user defined output
// variables. Fragment shader output variables can be explicitly bound to a
// render target. When a program is linked any output variables without an
// explicit binding specified through glBindFragDataLocation() will
// automatically be bound to fragment colors by the GL.

out vec4 FragColor;

void main()
{
    vec4 colorout = SpriteColor;

    FragColor = colorout * texture(ColorMapSampler, vTexCoord);
}