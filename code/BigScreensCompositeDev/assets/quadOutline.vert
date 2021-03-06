#version 150 core
#extension all : warn

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 modelView;
uniform vec2 dimensions;
uniform int colorOffset;
uniform vec3 tankPosition;
uniform vec3 tankVector;

uniform vec3 groundScale;
uniform vec3 groundOffset;
uniform float mountainMultiplier;
uniform float alphaMultiplier;

uniform sampler2D heightMap;

uniform int count;
uniform int nearLimit;
uniform int farLimit;
uniform float fft[64]; // 64 == num fft channels

out VS_OUT
{
    vec4 color;
    float fog;
    
} vs_out;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void)
{    
    // NOTE: All scale and positioning is handled by the matrix.
    // The vectors exist in 0-1 space.

    vec4 modelPosition = modelView * vec4(position, 1.0);
    vec4 projectionPosition = projection * vec4(position, 1.0);
    
    float scalarPosX = position.x;
    float scalarPosY = position.y;
    float scalarPosZ = position.z;
    
    float wrappingPosX = abs(1.0 - (scalarPosX));
    float wrappingPosZ = abs(1.0 - (scalarPosZ));

    float spectrumScalar = wrappingPosX + wrappingPosZ;
    float frameScalar = float(colorOffset % 1000) / 1000.0;
    
    // Color based on position
    vec3 rgbColor = hsv2rgb(vec3(spectrumScalar, 1.0, 1.0));
    
    vec4 positionColor = vec4(rgbColor.rgb, 1.0);
    
    int fftBand = int(64.0 * spectrumScalar) % 64;
    
    float fftVolume = fft[fftBand];
    
    // Boost volume at the end of the spectrum
    float scalarSpectrum = float(fftBand) / 64.0;
    fftVolume *= 1.0 + (scalarSpectrum * 3);
    fftVolume *= 0.25;
    
    float fftHeightMulti = 1.0;
    vec4 pickedColor = positionColor;

    vs_out.color = vec4(pickedColor.rgb, 0.5 * alphaMultiplier);

    // Height and position:
    
    float textureHeight = texture( heightMap, vec2(position.x, position.z) ).x;
    
    if (mountainMultiplier > 0)
    {
        // Get the distance from the tank
        vec3 groundPosition = (groundOffset + position) * groundScale;
        vec3 antiVector = vec3(1.0,1.0,1.0) - tankVector;
        float unitDistanceFromTank = length(((groundPosition - tankPosition) / groundScale) * antiVector);
        // non-linear
        unitDistanceFromTank *= unitDistanceFromTank;
        textureHeight *= 1.0 + (unitDistanceFromTank * mountainMultiplier);
    }
    
    // This sets the outlines slightly above the ground so theyre visible
    // This should actually be a normal, but a Y offset is simpler
    
    //float lineVertOffset = 5.0 / groundScale.y;
    // Better for IAC
    float lineVertOffset = 1.0 / groundScale.y;
    
    gl_Position = projection * modelView * vec4(position.x,
                                                position.y + textureHeight + lineVertOffset,
                                                position.z,
                                                1.0);

    vs_out.fog = gl_Position.z;
}
