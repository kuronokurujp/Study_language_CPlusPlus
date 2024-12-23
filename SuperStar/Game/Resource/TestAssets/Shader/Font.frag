// Request GLSL 3.3
#version 330

in vec2 fragTexCoord;
out vec4 outColor;

uniform sampler2D uTexture;

void main()
{
	vec4 tex = texture2D(uTexture, fragTexCoord);
	//vec4 tex = textureLod(uTexture, fragTexCoord, 0.0);
    outColor = tex;
    //outColor = vec4(fragTexCoord.xy, 0, 1);
}