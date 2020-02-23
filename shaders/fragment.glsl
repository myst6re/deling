uniform sampler2D texture;
uniform sampler2D palettes;
uniform float palId;
uniform float palMultiplier;
varying mediump vec4 texc;

void main(void)
{
    //if (palMultiplier == 0.0f) {
        gl_FragColor = texture2D(texture, texc.st);
    /* } else {
        vec4 i = texture2D(texture, texc.st);
        gl_FragColor = texture2D(palettes, vec2(i.r * palMultiplier, palId));
    } */
}
