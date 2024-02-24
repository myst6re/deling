attribute highp vec4 vertex;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
uniform mediump mat4 projMatrix;
uniform mediump mat4 mvMatrix;

void main(void)
{
    gl_Position = projMatrix * mvMatrix * vertex;
    texc = texCoord;
}
