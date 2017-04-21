#ifndef LITH_HUDID_H
#define LITH_HUDID_H

enum
{
   // hid_base         to hid_base + 99    reserved for testing
   // hid_base +   100 to hid_base + 29999 reserved for CBI
   // hid_base + 30000 to hid_base + 39999 reserved for the Gameboy
   // hid_base + 40000 to hid_base + 40999 reserved for Payout
   // hid_base + 41000 to hid_base + 41999 reserved for Misc.
   // hid_base + 42000 to hid_base + 42999 reserved for HUD
   // hid_base + 43000 to hid_base + 44999 reserved for Scope
   hid_base = 0x100000,
   
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
   hid_base_cbi = hid_base + 200,
   hid_end_cbi  = hid_base + 29999,
   
   // Gameboy
   hid_base_gb = hid_base + 30000,
   hid_end_gb  = hid_base + 39999,
   
   // Payout
   hid_base_payout = hid_base + 40000,
   hid_end_payout  = hid_base + 40999,
   
   // Misc.
   hid_base_misc = hid_base + 41000,
   hid_end_misc  = hid_base + 41999,
   
   hid_implyingE = hid_base_misc,
   hid_implyingS = hid_implyingE + 200,
   
   hid_sigil_title,
   hid_sigil_subtitle,
   
   hid_zoomvignette,
   hid_zoombase,
   hid_zoomcomp,
   hid_zoomcam = hid_zoomcomp + 8,
   
   // HUD
   hid_base_hud = hid_base + 42000,
   hid_end_hud  = hid_base + 42999,
   
   hid_logAddE = hid_base_hud,
   hid_logAddS = hid_logAddE + 20,
   
   hid_logE,
   hid_logS = hid_logE + 20,
   
   hid_vitalscannerhp,
   hid_vitalscannertag,
   hid_vitalscanhitE,
   hid_vitalscanhitS = hid_vitalscanhitE + 5,
   
   hid_jetE,
   hid_jetS = hid_jetE + 15,
   hid_jettext,
   hid_jetbg,
   
   hid_weaponcurE,
   hid_weaponcurS = hid_weaponcurE + 9,
   
   hid_weapontextE,
   hid_weapontextS = hid_weapontextE + 9,
   
   hid_weaponE,
   hid_weaponS = hid_weaponE + 9,
   hid_weaponbg,
   
   hid_ammo,
   hid_ammotype,
   hid_ammotypebg,
   hid_ammobg,
   hid_riflemode,
   hid_riflemodebg,
   hid_rifleheat = hid_riflemode,
   hid_rifleheatbg = hid_riflemodebg,
   
   hid_slideind_fxE,
   hid_slideind_fxS = hid_slideind_fxE + 11,
   
   hid_healthhit,
   hid_health,
   hid_healthtxt,
   hid_healthbg,
   hid_healthbg_fxE,
   hid_healthbg_fxS = hid_healthbg_fxE + 32,
   
   hid_rarmortxt,
   hid_rarmorbg,
   
   hid_armorhit,
   hid_armor,
   hid_armortxt,
   hid_armorbg,
   hid_armorbg_fxE,
   hid_armorbg_fxS = hid_armorbg_fxE + 32,
   
   hid_scorehit,
   hid_score,
   hid_scoreaccum,
   
   hid_key_blue,
   hid_key_yellow,
   hid_key_red,
   
   hid_key_blueskull,
   hid_key_yellowskull,
   hid_key_redskull,
   
   // Scope
   hid_base_scope = hid_base + 43000,
   hid_end_scope  = hid_base + 44999,
   
   hid_scope_sineE = hid_base_scope,
   hid_scope_sineS = hid_scope_sineE + 200,
   hid_scope_squareE,
   hid_scope_squareS = hid_scope_squareE + 200,
   hid_scope_triE,
   hid_scope_triS = hid_scope_triE + 200,
   hid_scope_stringstackE,
   hid_scope_stringstackS = hid_scope_stringstackE + 200,
   hid_scope_overlayE,
   hid_scope_overlayS = hid_scope_overlayE + 5,
   hid_scope_lineE,
   hid_scope_lineS = hid_scope_lineE + 200,
   
   hid_rifle_scope_xhair,
   hid_rifle_scope_img,
   hid_rifle_scope_cam,
   
   hid_scope_clearS = hid_scope_sineE,
   hid_scope_clearE = hid_scope_stringstackS,
};

#endif

