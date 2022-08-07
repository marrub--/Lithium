// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

void main() {
   vec2 uv = TexCoord.xy;
   vec4 cr = texture(InputTexture, uv);
   float n = floor(1.0 + lith_t * 200.0 / 35.0);

   for(float i = 1.0; i < n; i++) {
      cr += texture(InputTexture, vec2(uv.x, uv.y + i / 1600.0));
      cr += vec4(0.001 * i, 0.0, 0.0, 0.0);
   }

   FragColor = cr / n;
}

/* EOF */
