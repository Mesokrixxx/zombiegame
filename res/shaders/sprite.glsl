@vertex
#version 330 core

layout (location = 0) in vec2 lPos;
layout (location = 1) in vec2 lTexCoord;

layout (location = 2) in vec2 iPos;
layout (location = 3) in vec2 iScale;
layout (location = 4) in float iZ;
layout (location = 5) in vec4 iColor;
layout (location = 6) in vec2 iUVMin;
layout (location = 7) in vec2 iUVMax;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec4 color;

void main() {
	texCoord = mix(iUVMin, iUVMax, lTexCoord);
	color = iColor;

	gl_Position = projection * view * vec4(lPos * iScale + iPos, iZ, 1);
}

@fragment
#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;

uniform sampler2D tex;

void main() {
	vec4 outColor = texture(tex, texCoord) * color;
	if (outColor.a < 0.1)
		discard ;

	fragColor = outColor;
}
