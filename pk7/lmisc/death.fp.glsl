// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

vec4 Process(vec4 color)
{
   vec2 uv = gl_TexCoord[0].st;
   float a = getTexel(uv).a;
   float s = cos(sin(uv.x) * uv.y * 3.0 + timer) + 0.5;
   float tt = (uv.x * sin(timer) + uv.y * cos(timer)) * 3.0;
   vec4 fc = vec4(-(s * 0.05 * cos(tt)), 1.0, -(s * 0.05 * sin(tt)), a);
   return max(fc, 0.0) * color;
}

/* EOF */
