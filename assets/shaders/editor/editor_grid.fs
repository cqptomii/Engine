#version 460 core

in vec2 vNdc;
out vec4 FragColor;

uniform mat4 uInvViewProj;
uniform mat4 uViewProj;
uniform vec3 uCameraPos;

uniform float uMinorStep;
uniform float uMajorEvery;
uniform float uLineThickness;
uniform float uFadeStart;
uniform float uFadeEnd;
uniform float uGridHeight;

uniform vec4 uMinorColor;
uniform vec4 uMajorColor;

vec3 unprojectPoint(vec2 ndcXY, float ndcZ)
{
    vec4 clip = vec4(ndcXY, ndcZ, 1.0);
    vec4 world = uInvViewProj * clip;
    return world.xyz / world.w;
}

float gridMask(vec2 worldXZ, float step, float thickness)
{
    vec2 cell = worldXZ / step;
    vec2 a = abs(fract(cell - 0.5) - 0.5) / max(fwidth(cell), vec2(1e-6));
    float d = min(a.x, a.y);
    return 1.0 - smoothstep(thickness, thickness + 1.0, d);
}

float axisMask(float distanceToAxis, float thickness)
{
    float d = abs(distanceToAxis) / max(fwidth(distanceToAxis), 1e-6);
    return 1.0 - smoothstep(thickness, thickness + 1.0, d);
}

void main()
{
    vec3 nearPoint = unprojectPoint(vNdc, -1.0);
    vec3 farPoint = unprojectPoint(vNdc, 1.0);

    vec3 rayDir = normalize(farPoint - nearPoint);

    // Intersect camera ray with XZ plane at configurable world height.
    // Use a smooth horizon fade and avoid mirroring behind the camera.
    float denom = rayDir.y;
    float denomAbs = max(abs(denom), 1e-6);
    // Fade near grazing angles to remove hard horizon seam growth while zooming out.
    float parallelFade = smoothstep(0.0, 0.08, denomAbs);
    float denomSafe = (denom >= 0.0) ? denomAbs : -denomAbs;

    float tRaw = (uGridHeight - nearPoint.y) / denomSafe;
    // Smooth transition across the forward/behind boundary.
    float forwardFade = smoothstep(-5.0, 5.0, tRaw);
    float t = max(tRaw, 1e-3);

    vec3 worldPos = nearPoint + rayDir * t;

    float majorStep = max(uMinorStep * uMajorEvery, uMinorStep);

    float minor = gridMask(worldPos.xz, uMinorStep, uLineThickness);
    float major = gridMask(worldPos.xz, majorStep, uLineThickness);

    vec4 color = vec4(0.0);
    color = mix(color, uMinorColor, minor);
    color = mix(color, uMajorColor, major);

    float distToCamera = length(worldPos - uCameraPos);
    float fade = 1.0 - smoothstep(uFadeStart, uFadeEnd, distToCamera);
    color.a *= fade * parallelFade * forwardFade;

    if (color.a <= 0.001)
    {
        discard;
    }

    // Write depth from world position so depth test behaves like regular geometry.
    vec4 clip = uViewProj * vec4(worldPos, 1.0);
    float ndcDepth = clip.z / clip.w;
    gl_FragDepth = clamp(ndcDepth * 0.5 + 0.5, 0.0, 1.0);

    FragColor = color;
}
