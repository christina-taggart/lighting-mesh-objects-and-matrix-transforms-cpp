/* Christina Taggart
 * Program 3: Lighting
 * CSC 471 with Professor Zoe Wood 
 * Fragment shader for every pixel
 */

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uRotMat;
uniform float uMaterial;
uniform vec3 ulPos;

varying vec3 lightDir; 
varying vec3 vDist;
varying vec3 normal;
varying vec3 vColor;
varying vec3 lRef;

float ka = 0.5; //amount of ambient light
float Il = 0.5; //incoming light's intensity/color

void main() {

  float alpha = 5.0;
  float diffuse = uMaterial * max( (dot(normal, lightDir)), 0.0) * Il; 
  float specular = uMaterial * max( pow(dot(vDist, lRef), alpha) ,0.0) * Il;
  float ambient = ka*Il;
  gl_FragColor = vec4(vColor.r * ( diffuse + specular + ambient ), 
			          vColor.g * ( diffuse + specular + ambient ), 
			          vColor.b * ( diffuse + specular + ambient ),
			          1.0);

}