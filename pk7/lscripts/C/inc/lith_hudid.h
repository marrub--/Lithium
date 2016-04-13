#ifndef LITH_HUDID_H
#define LITH_HUDID_H

enum
{
   //
   // hid_base         to hid_base + 99    reserved for Testing
   // hid_base +   100 to hid_base + 39999 reserved for CBI
   // hid_base + 40000 to hid_base + 40999 reserved for Misc.
   // hid_base + 41000 to hid_base + 41999 reserved for Scope
   // hid_base + 42000 to hid_base + 42999 reserved for HUD
   //
   
   hid_base = 500000,
   
   // Testing
   hid_base_test = hid_base,
   hid_end_test  = hid_base + 99,
   
   hid_test1 = hid_base_test,
   hid_test2,
   hid_test3,
   hid_test4,
   hid_test5,
   hid_test6,
   hid_test7,
   hid_test8,
   hid_test9,
   
   // CBI
   hid_base_cbi = hid_base + 100,
   hid_end_cbi  = hid_base + 39999,
   
   hid_cbi_cursor = hid_base_cbi,
   hid_cbi_underflow,
   
   // Misc.
   hid_base_misc = hid_base + 40000,
   hid_end_misc  = hid_base + 40999,
   
   hid_ack = hid_base_misc,
   hid_implyingE,
   hid_implyingS = hid_implyingE + 200,
   
   // Scope
   hid_base_scope = hid_base + 41000,
   hid_end_scope  = hid_base + 41999,
   
   hid_scope_sineE = hid_base_scope,
   hid_scope_sineS = hid_scope_sineE + 200,
   hid_scope_squareE,
   hid_scope_squareS = hid_scope_squareE + 200,
   hid_scope_triE,
   hid_scope_triS = hid_scope_triE + 200,
   hid_scope_stringstackE,
   hid_scope_stringstackS = hid_scope_stringstackE + 200,
   
   hid_rifle_scope_xhair,
   hid_rifle_scope_img,
   hid_rifle_scope_cam,
   
   hid_scope_clearS = hid_scope_sineE,
   hid_scope_clearE = hid_scope_stringstackS,
   
   // HUD
   hid_base_hud = hid_base + 42000,
   hid_end_hud  = hid_base + 42999,
   
   hid_jetE = hid_base_hud,
   hid_jetS = hid_jetE + 15,
   hid_jettext,
   hid_jetbg,
   
   hid_weapon1text,
   hid_weapon7text = hid_weapon1text + 8,
   
   hid_weapon1,
   hid_weapon7 = hid_weapon1 + 8,
   
   hid_weapontext,
   hid_weaponbg,
   
   hid_ammo,
   hid_ammobg,
   
   hid_slideind_fxE,
   hid_slideind_fxS = hid_slideind_fxE + 11,
   
   hid_healthhit,
   hid_health,
   hid_healthbg,
   hid_healthbg_fxE,
   hid_healthbg_fxS = hid_healthbg_fxE + 32,
   
   hid_armorhit,
   hid_armor,
   hid_armorbg,
   hid_armorbg_fxE,
   hid_armorbg_fxS = hid_armorbg_fxE + 32,
   
   hid_scorehit,
   hid_score,
   hid_scoreaccum,
   hid_scorelabel,
   
   hid_key_blue,
   hid_key_yellow,
   hid_key_red,
   
   hid_key_blueskull,
   hid_key_yellowskull,
   hid_key_redskull
};

#endif

