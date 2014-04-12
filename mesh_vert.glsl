/* Christina Taggart
 * Program 3: Lighting
 * CSC 471 with Professor Zoe Wood 
 * Vertex shader runs for every vertex
 */

attribute vec3 aPosition;
attribute vec3 aColor;
attribute vec3 aNormal;
attribute vec3 aViewPosition;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uRotMat;
uniform float uMaterial;
uniform vec3 ulPos;

//varying gets interpolated
varying vec3 vColor;
varying vec3 lightDir; 
varying vec3 vDist;
varying vec3 normal;
varying vec3 lRef; 

float ka = 0.5; //amount of ambient light
float Il = 1.5; //incoming light's intensity/color

void main() {
  vec4 vPosition;
  vec3 lDir;

  /* First model transforms */
  vPosition = uModelMatrix* vec4(aPosition.x, aPosition.y, aPosition.z, 1);
  lDir = vec3( ulPos.x - vPosition.x, ulPos.y - vPosition.y, ulPos.z - vPosition.z );  //v world position, aposition 1 -1 object 

  vDist = normalize(vec3( aViewPosition.x - vPosition.x, aViewPosition.y - vPosition.y, aViewPosition.z - vPosition.z));
  lRef = normalize(-ulPos + 2.0*( max(dot(ulPos, normal), 0.0) ) * normal) ;

  vPosition = uViewMatrix* vPosition; 
  gl_Position = uProjMatrix*vPosition;

  lightDir = normalize(lDir);
  vec4 n = normalize(uRotMat*vec4(aNormal.x, aNormal.y, aNormal.z, 0.0));
  normal = vec3(n.x, n.y, n.x);

  gl_FrontColor = vec4(aColor.r, aColor.g, aColor.b, 1.0);
  float alpha = 1.0;
  float diffuse = uMaterial * max( (dot(normal, lightDir)), 0.0) * Il; 
  float specular = uMaterial * max( pow(dot(vDist, lRef), alpha) ,0.0) * Il;
  float ambient = ka*Il;

  vColor = vec3(  aColor.r * ( diffuse + specular + ambient ), 
			      aColor.g * ( diffuse + specular + ambient ), 
			      aColor.b * ( diffuse + specular + ambient ) );


}

