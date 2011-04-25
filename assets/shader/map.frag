#version 330

smooth in vec4 theColor;
smooth in vec2 vTexCoord;

out vec4 outputColor;
uniform sampler2D textureMap;

void main()
{
    //gl_FragColor = vec4(0.0,0.0,1.0f,1.0f);
    //float lerpValue = gl_FragCoord.y / 600.0f;
    
    //outputColor = mix(vec4(1.0f,1.0f,1.0f,1.0f),
                    //vec4(0.2f,0.2f,0.2f,1.0f), lerpValue);
    
    //outputColor = theColor;
    gl_FragColor = theColor * texture(textureMap, vTexCoord);
    
}