#version 330 core

uniform sampler2D TEXTURE0;
uniform sampler2D TEXTURE1;

in Vertex{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
}IN;

out vec4 fragColour;

void main(void) {
	if (IN.worldPos.y > 0) {
		fragColour = texture2D(TEXTURE0, IN.texCoord);
	}
	else {
		fragColour = texture2D(TEXTURE1, IN.texCoord);
	}
}