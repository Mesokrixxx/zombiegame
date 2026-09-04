@vertex
#version 330 core

layout (location = 0) in vec2 lPos;
layout (location = 1) in vec2 lTexCoord;
layout (location = 2) in vec2 iPos;
layout (location = 3) in vec2 iScale;
layout (location = 4) in float iZ;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main() {
	texCoord = lTexCoord;
	
	gl_Position = projection * view * vec4(lPos * iScale + iPos, iZ, 1);
}

@fragment
#version 330 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;

void main() {
	vec4 color = texture(tex, texCoord);

	if (color.a < 0.1)
		discard ;
	fragColor = color;
}
