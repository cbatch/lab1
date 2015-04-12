attribute vec4 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform vec3 uLightPos;
uniform vec3 UaColor;
uniform vec3 UdColor;
uniform vec3 UsColor;
uniform float Ushine;

varying vec3 vNormals;
varying vec3 vLight;

void main()
{
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
	vNormals = vec3(uViewMatrix * uModelMatrix * vec4(normalize(aNormal), 0.0));
	vLight = uLightPos - vec3(uViewMatrix * uModelMatrix * aPosition);
}