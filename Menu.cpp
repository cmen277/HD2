#include "Menu.h"
#include "imgui.h"
#include "Scanning.h"
#include <Windows.h>
#include "minhook/MinHook.h"
#include "Logging.h"
#include <string>
#include "LuaHook.h"
#include <vector>


void PatchHelper(uintptr_t Ptr, std::vector<BYTE> bytes)
{
    int Index = 0;
    for (auto b : bytes)
    {
        *(BYTE*)(Ptr + Index) = b;
        Index++;
    }
}

bool bNoReload = false;
bool bNoReload2 = false;
bool bInfAmmo = false;
bool bInfNades = false;
bool bInfSyringes = false;
bool bInfSprint = false;
bool bInfShield = false;
bool bFastExtract = false;
bool bStratagems = false;
bool bRecoil = false;
bool bSway = false;
bool bBunker = false;
bool bUnlock = false;
bool bMap = false;
bool bSamples = false;
bool bInstaComplete = false;
bool bInfBackpack = false;
bool bInstaDrop = false;
bool bInfAmmoLaser = false;
bool bLaserOverheat = false;
bool bBoundary = false;
bool bOutpostComplete = false;
bool bHiddenOutposts = false;
bool bJetpack = false;
bool bNoStationaryTurretOverheat = false;
bool bRailgun = false;
bool bQuasar = false;


unsigned long long NoReloadV2Return = NULL;
unsigned long long ShieldReturn = NULL;
unsigned long long ExtractReturn = NULL;
unsigned long long SamplesReturn = NULL;
unsigned long long InstaComplete1Return = NULL;
unsigned long long InstaComplete2Return = NULL;
unsigned long long InstantCompleteOutpostReturn = NULL;
unsigned long long HiddenOutpostsReturn = NULL;
unsigned long long InstantDropReturn = NULL;


// Time Cheat
unsigned long long TimeReturn = NULL;
float fTimer = 1800.0f;
bool bSetTimer = false;
bool bFreezeTimer = false;

// Health
bool bInfHealth = false;
uintptr_t pPlayerHealth = NULL;
unsigned long long HealthReadReturn;
unsigned long long HealthWriteReturn;

// Samples
int NormaleSamples = 33;
int RareSamples = 33;
int SuperSamples = 33;

//Speedhack
bool bSpeedhack = false;
unsigned long long SpeedhackReturn = NULL;
float fPlayerSpeed = 2.0f;
bool bFreezeEnemies = false;

// Armory Editor
bool bArmoryEditor = false;
uintptr_t pWeapon = NULL;
uintptr_t pFireRate = NULL;
unsigned long long ArmoryEditor1Return = NULL;
unsigned long long ArmoryEditor2Return = NULL;
unsigned long long ArmoryEditor3Return = NULL;
unsigned long long ArmoryEditor4Return = NULL;
unsigned long long ArmoryEditor5Return = NULL;
unsigned long long ArmoryEditor6Return = NULL;
unsigned long long ArmoryEditor7Return = NULL;

//payload
bool bPayload = false;
int PayloadID = 0;
uintptr_t PayloadHash = 0;
unsigned long long StratagemPayloadItemTypeReturn = NULL;
std::vector<uintptr_t> StratagemPayloadItemTypeList;

// ProjID
bool bProjID = false;
bool bPlayerOnly = false;
unsigned long long GetProjIDReturn = NULL;
int iProjID = -1;
int iTargetID = -1;
int iNewID = -1;
struct ProjectilePair
{
public:
    int Original;
    int Swap;
};
std::vector<ProjectilePair> ProjectileList;


enum Menu
{
    None = 0,
    Character = 1,
    Weapon = 2,
    Mission = 3,
    Data = 4,
    Misc = 5,
    Experimental = 6,
    About = 7
};

Menu MenuState = Menu::Character;
bool bLua = false;
int iLastLuaError = 0;
char ConsoleBuffer[4096] = { 0 };


void DrawMenu()
{
    if (!bScanDone)
        return;

    if (StratagemPayloadItemTypeList.size() == 0)
    {
        InitPayLoadHashes();
    }
    //// LUA

    if (bLua)
    {
        ImGui::Begin("Lua Executor");
        ImGui::InputTextMultiline("##text", ConsoleBuffer, 4096, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
        if (ImGui::Button("Execute"))
        {
            if (strlen(ConsoleBuffer) > 1)
            {
                iLastLuaError = DoString(ConsoleBuffer);
            }
        }
        if (iLastLuaError != 0)
        {
            ImGui::Text("Error Executing Last Lua Script: %d", iLastLuaError);
        }
        ImGui::End();
    }


    /////////// Menu

    ImGui::Begin("DX12 Cfemen | ThatRussianGuy | Gir489 | BEDaisy");

    /////////// Buttons
    if (ImGui::Button("Character"))
    {
        MenuState = Menu::Character;
    }
    ImGui::SameLine();
    if (ImGui::Button("Weapon"))
    {
        MenuState = Menu::Weapon;
    }
    ImGui::SameLine();
    if (ImGui::Button("Mission"))
    {
        MenuState = Menu::Mission;
    }
    ImGui::SameLine();
    if (ImGui::Button("Data"))
    {
        MenuState = Menu::Data;
    }
    ImGui::SameLine();
    if (ImGui::Button("Misc"))
    {
        MenuState = Menu::Misc;
    }
    ImGui::SameLine();
    if (ImGui::Button("Experimental"))
    {
        MenuState = Menu::Experimental;
    }
    ImGui::SameLine();
    if (ImGui::Button("About"))
    {
        MenuState = Menu::About;
    }
    ImGui::SameLine();
    if (ImGui::Button("Show Lua Console"))
    {
        bLua = !bLua;
    }
    /////////// Functions
    switch (MenuState)
    {
    case Menu::Character:
        if (ImGui::Checkbox("Inf. Ammo", &bInfAmmo))
        {
            InfAmmoPatch();
        }
        if (ImGui::Checkbox("Inf. Nades", &bInfNades))
        {
            InfNadePatch();
        }
        if (ImGui::Checkbox("Inf. Syringes", &bInfSyringes))
        {
            InfSyringePatch();
        }
        if (ImGui::Checkbox("Inf. Sprint", &bInfSprint))
        {
            InfSprintPatch();
        }
        if (ImGui::Checkbox("Infinite Backpack", &bInfBackpack))
        {
            InfBackpackPatch();
        }
        if (ImGui::Checkbox("Inf. Health", &bInfHealth))
        {
            InfHealthHook();
        }
        if (ImGui::Checkbox("Inf. Shield", &bInfShield))
        {
            InfShieldHook();
        }
        if (ImGui::Checkbox("Inf. Stratagems", &bStratagems))
        {
            StratagemsPatch();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Instant Drop", &bInstaDrop))
        {
            InstaDropHook();
        }
        if (ImGui::Checkbox("Speedhack", &bSpeedhack))
        {
            SpeedhackHook();
        }
        if (bSpeedhack)
        {
            ImGui::Checkbox("Freeze Enemies", &bFreezeEnemies);
            ImGui::Text("Player-Speed:");
            ImGui::SameLine();
            ImGui::SliderFloat("##text2", &fPlayerSpeed, 1, 20);
        }
        break;
    case Menu::Weapon:
        if (ImGui::Checkbox("No Reload", &bNoReload))
        {
            NoReloadPatch();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("No Reload V2", &bNoReload2))
        {
            NoReload2Hook();
        }
        if (ImGui::Checkbox("No Recoil", &bRecoil))
        {
            RecoilPatch();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("No Sway", &bSway))
        {
            SwayPatch();
        }
        if (ImGui::Checkbox("Armory Editor", &bArmoryEditor))
        {
            ArmoryEditorHook();
        }
        if (bArmoryEditor)
        {
            if (pWeapon == 0 || pFireRate == 0)
            {
                ImGui::Text("...Select A Weapon In The Armory...");
            }
            else
            {
                DWORD old;
                VirtualProtect((VOID*)pFireRate, 0x64, PAGE_EXECUTE_READWRITE, &old);
                VirtualProtect((VOID*)pWeapon, 0x64, PAGE_EXECUTE_READWRITE, &old);
                ImGui::SliderFloat("FireRate", (float*)(pFireRate + 8), 1, 3000);
                ImGui::SliderInt("Damage", (int*)(pWeapon + 4), 1, 9999);
                ImGui::SliderInt("Structure Penetration", (int*)(pWeapon + 8), 1, 1000);
                ImGui::SliderInt("Penetration(No Angle)", (int*)(pWeapon + 0xC), 1, 1000);
                ImGui::SliderInt("Penetration(Angle)", (int*)(pWeapon + 0x10), 1, 1000);
                ImGui::SliderInt("Demolition", (int*)(pWeapon + 0x1C), 1, 1000);
            }
        }
        if (ImGui::Checkbox("Inf. Ammo Laser", &bInfAmmoLaser))
        {
            InfAmmoLaserPatch();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("No Laser Overheat/Quasar", &bLaserOverheat))
        {
            LaserOverheatPatch();
        }
        if (ImGui::Checkbox("Instant Charge Railgun", &bRailgun))
        {
            InstantChargeRailgunPatch();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Instant Charge Quasar", &bQuasar))
        {
            InstantChargeQuasuarPatch();
        }
        if (ImGui::Checkbox("Get Projectile ID", &bProjID))
        {
            GetProjIDHook();
        }
        if (bProjID)
        {
            ImGui::Checkbox("Last Fired Projectile Player Only?", &bPlayerOnly);
            if (iProjID == -1)
            {
                ImGui::Text("Shoot To Get ID ...");
            }
            {
                ImGui::Text("Last Fired Projectile ID: %d", iProjID);
                ImGui::Text("Target Projectile (1-263)");
                ImGui::InputInt("##projtarget", &iTargetID);
                ImGui::Text("New Projectile (1-263)");
                ImGui::InputInt("##projnew", &iNewID);
                if (iTargetID != -1 && iNewID != -1)
                {
                    if (ImGui::Button("Replace Projectile"))
                    {
                        ProjectilePair tmpList;
                        tmpList.Original = iTargetID;
                        tmpList.Swap = iNewID;
                        bool bAdd = true;
                        for (auto o : ProjectileList)
                        {
                            if (o.Original == iTargetID)
                            {
                                bAdd = false;
                            }
                        }
                        if (bAdd)
                        {
                            ProjectileList.push_back(tmpList);
                        }
                    }
                }
            }
            for (auto o : ProjectileList)
            {
                ImGui::Text("%d -> %d", o.Original, o.Swap);
            }
            if (ProjectileList.size() > 0)
            {
                if (ImGui::Button("Clean List!"))
                {
                    ProjectileList.clear();
                }
            }
        }
        break;
    case Menu::Mission:
        if (ImGui::Checkbox("Insta Complete", &bInstaComplete))
        {
            InstaCompleteHook();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Outpost Complete", &bOutpostComplete))
        {
            InstantCompleteOutpostHook();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Show Hidden Outposts", &bHiddenOutposts))
        {
            HiddenOutpostsHook();
        }
        ImGui::Text("Mission-Time:");
        ImGui::SameLine();
        ImGui::SliderFloat("##text", &fTimer, 0, 2400);
        if (ImGui::Button("Set Timer"))
        {
            bSetTimer = true;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Freeze Time", &bFreezeTimer);
        if (ImGui::Checkbox("Fast Extraction", &bFastExtract))
        {
            FastExtractHook();
        }
        if (ImGui::Checkbox("Map Hack", &bMap))
        {
            MapPatch();
        }
        if (ImGui::Checkbox("Solo Bunker", &bBunker))
        {
            BunkerPatch();
        }
        if (ImGui::Checkbox("No Boundary", &bBoundary))
        {
            BoundaryPatch();
        }
        if (ImGui::Checkbox("Enable Samples Hack", &bSamples))
        {
            SamplesHook();
        }
        if (bSamples)
        {
            ImGui::SliderInt("<-Normal Samples On Pickup", &NormaleSamples, 0, 99);
            ImGui::SliderInt("<-Rare Samples On Pickup", &RareSamples, 0, 99);
            ImGui::SliderInt("<-Super Samples On Pickup", &SuperSamples, 0, 99);
            int Check = NormaleSamples + RareSamples + SuperSamples;
            if (Check > 99)
            {
                ImGui::TextColored(ImColor(255, 0, 0, 255), "WARNING: SAMPLES ARE OVER 99 - IT WILL PREVENT ANY REWARD");
            }
        }
        break;
    case Menu::Data:
        ImGui::Text("WIP");
        break;
    case Menu::Misc:
        if (ImGui::Checkbox("Inf. Jetpack", &bJetpack))
        {
            JetpackPatch();
        }
        if (ImGui::Checkbox("No Stationary Turret Overheat", &bNoStationaryTurretOverheat))
        {
            NoStationaryTurretOverheatPatch();
        }
        if (ImGui::Checkbox("Stratagem Payload", &bPayload))
        {
            StratagemPayloadItemTypeHook();
        }
        if (bPayload)
        {
            //ImGui::SliderInt("<- ID", &PayloadID, 0, 101);
            ImGui::InputInt("<- ID", &PayloadID);
            PayloadHash = StratagemPayloadItemTypeList[PayloadID];
        }
        break;
    case Menu::Experimental:
        ImGui::Text("WIP");
        break;
    case Menu::About:
        ImGui::Text("Menu Version:2.0.0");
        ImGui::SameLine();
        ImGui::Text("Created by Cfemen, Gir489, BEDaisy, That Russian Guy");
        ImGui::Text("Need help ? Contact That Russian Guy or create a ticket");
        break;
    }

    /*  if (ImGui::Button("World"))
      {
          MenuState = Menu::Cats;
      }
      ImGui::SameLine();
      if (ImGui::Button("Otter"))
      {
          MenuState = Menu::Otter;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cheats"))
      {
          MenuState = Menu::Cheats;
      }
      switch (MenuState)
      {
      case Menu::Cats:
          ImGui::Text("Cats");
          ImGui::Text("Kitty");
          break;
      case Menu::Otter:
          ImGui::Text("Big Otter");
          ImGui::Text("Where Is Gumi???");
          break;
      case Menu::Cheats:
          if (ImGui::RadioButton("Infinite Time", bInfiniteTime))
          {
              bInfiniteTime = !bInfiniteTime;
              TimeCheat();
          }
          if (ImGui::Button("Get Time"))
          {
              TimeCheatHook();
          }
          if (fTimer != NULL)
          {
              ImGui::Text("%f", *fTimer);
          }
          break;
      }*/
    ImGui::End();
}

void InitPayLoadHashes()
{
    StratagemPayloadItemTypeList.push_back(0);
    StratagemPayloadItemTypeList.push_back(0x6B72305013C2833F);
    StratagemPayloadItemTypeList.push_back(0x147BD99513726F88); // medal
    StratagemPayloadItemTypeList.push_back(0x818603ED54533506); // supercredits
    StratagemPayloadItemTypeList.push_back(0xBD6F4DE16B9AEDCD);
    StratagemPayloadItemTypeList.push_back(0x2ED3696E12A14D4C);
    StratagemPayloadItemTypeList.push_back(0x12C8D71AC3897A5C);
    StratagemPayloadItemTypeList.push_back(0x2C66C201B2543D2C);
    StratagemPayloadItemTypeList.push_back(0xDFB610642EFF47D7);
    StratagemPayloadItemTypeList.push_back(0xA8CFFB316F0B5C5F);
    StratagemPayloadItemTypeList.push_back(0x4EF9A47109239A58);
    StratagemPayloadItemTypeList.push_back(0x2B28E17FFED05F7C);
    StratagemPayloadItemTypeList.push_back(0x1B00BCA55E364292);
    StratagemPayloadItemTypeList.push_back(0xBCB3210CF8EF49D3);
    StratagemPayloadItemTypeList.push_back(0xA55E504092B981D);
    StratagemPayloadItemTypeList.push_back(0x5F9AEAC72B738298);
    StratagemPayloadItemTypeList.push_back(0x35F50DEC0EC647C6);
    StratagemPayloadItemTypeList.push_back(0x5EBAEA70C0D060B9);
    StratagemPayloadItemTypeList.push_back(0x80F1A156D9FA1E36);
    StratagemPayloadItemTypeList.push_back(0x73270650F859DD0);
    StratagemPayloadItemTypeList.push_back(0x807AEA5217E4767);
    StratagemPayloadItemTypeList.push_back(0x25AA2FD4643CF4EE);
    StratagemPayloadItemTypeList.push_back(0xF49227A0630A3F7F);
    StratagemPayloadItemTypeList.push_back(0xADD299F56916AE1F);
    StratagemPayloadItemTypeList.push_back(0x68BA814FF35BF19C);
    StratagemPayloadItemTypeList.push_back(0x4F8A477E577AABA9);
    StratagemPayloadItemTypeList.push_back(0xBCC2177439D231BE);
    StratagemPayloadItemTypeList.push_back(0xBFFCB4CD971A8EDA);
    StratagemPayloadItemTypeList.push_back(0x4E4A613EB9BF5C24);
    StratagemPayloadItemTypeList.push_back(0xB6AFF2195568767F);
    StratagemPayloadItemTypeList.push_back(0x9951ECAE4E403070);
    StratagemPayloadItemTypeList.push_back(0x65A167E3B6C57F06);
    StratagemPayloadItemTypeList.push_back(0x9F80D67A12A7E40F);
    StratagemPayloadItemTypeList.push_back(0x26BDDF070C31B275);
    StratagemPayloadItemTypeList.push_back(0x89C5493E08CA4207);
    StratagemPayloadItemTypeList.push_back(0x5FECAB819F96A3E8);
    StratagemPayloadItemTypeList.push_back(0x255EBC5767D7CEEC);
    StratagemPayloadItemTypeList.push_back(0x8F3A6DE385966F9E);
    StratagemPayloadItemTypeList.push_back(0x3E67A19B07C6523);
    StratagemPayloadItemTypeList.push_back(0x93F2C9D89A1B56C5);
    StratagemPayloadItemTypeList.push_back(0x96DFC6542AA22980);
    StratagemPayloadItemTypeList.push_back(0xD323DE60855898AC);
    StratagemPayloadItemTypeList.push_back(0x2EECD0B1FA49630);
    StratagemPayloadItemTypeList.push_back(0x46183B50961D1328);
    StratagemPayloadItemTypeList.push_back(0x43CB1033961A2276);
    StratagemPayloadItemTypeList.push_back(0x70BC8D3E18BB8F9);
    StratagemPayloadItemTypeList.push_back(0x96DE9CD50F7306E6);
    StratagemPayloadItemTypeList.push_back(0x5B596A4D21BC91A3);
    StratagemPayloadItemTypeList.push_back(0xE60AE045E0090F4C);
    StratagemPayloadItemTypeList.push_back(0x5D8D8C073B9D502);
    StratagemPayloadItemTypeList.push_back(0x9FF6AFD33975861F);
    StratagemPayloadItemTypeList.push_back(0x27EE1ED8F6FB6356);
    StratagemPayloadItemTypeList.push_back(0x32890A67670C5224);
    StratagemPayloadItemTypeList.push_back(0x7B75E5132FFD4CA6);
    StratagemPayloadItemTypeList.push_back(0x4D61156DD350A82B);
    StratagemPayloadItemTypeList.push_back(0xF2B4A99D1B0E40C8);
    StratagemPayloadItemTypeList.push_back(0x8A4DE24951A1726D);
    StratagemPayloadItemTypeList.push_back(0x5990123D142B16CB);
    StratagemPayloadItemTypeList.push_back(0xC89ED7AC3E425B25);
    StratagemPayloadItemTypeList.push_back(0x8EC3026B5F2E579A);
    StratagemPayloadItemTypeList.push_back(0x967ED15E0BAE363B);
    StratagemPayloadItemTypeList.push_back(0xAF9B683CCB6DDC02);
    StratagemPayloadItemTypeList.push_back(0xC8378469E1936877);
    StratagemPayloadItemTypeList.push_back(0x4C786785C79D44E7);
    StratagemPayloadItemTypeList.push_back(0x2152D5147B0AC418);
    StratagemPayloadItemTypeList.push_back(0x9571CA51F0DAF35B);
    StratagemPayloadItemTypeList.push_back(0x41EAC4A03987FAA0);
    StratagemPayloadItemTypeList.push_back(0x99870D0755F9CE68);
    StratagemPayloadItemTypeList.push_back(0xCD00BDC1149C2928);
    StratagemPayloadItemTypeList.push_back(0x26E40437EA275296);
    StratagemPayloadItemTypeList.push_back(0x8645F167B3C813A2);
    StratagemPayloadItemTypeList.push_back(0xFB3A19078694708A);
    StratagemPayloadItemTypeList.push_back(0xCD1F0C09CAE0D533);
    StratagemPayloadItemTypeList.push_back(0xF5C67AED5CB5ACA0);
    StratagemPayloadItemTypeList.push_back(0x52071F49263415E4);
    StratagemPayloadItemTypeList.push_back(0xD6B1FB05B9109353);
    StratagemPayloadItemTypeList.push_back(0x52E4334E6A128CAF);
    StratagemPayloadItemTypeList.push_back(0xC12A34F375BD5A87);
    StratagemPayloadItemTypeList.push_back(0x7B06196E90154C88);
    StratagemPayloadItemTypeList.push_back(0x59C5CA839449B379);
    StratagemPayloadItemTypeList.push_back(0x35B5AF8B1E859540);
    StratagemPayloadItemTypeList.push_back(0x71B285B6D2B161C9);
    StratagemPayloadItemTypeList.push_back(0x968211C0033DCE64);
    StratagemPayloadItemTypeList.push_back(0x80932FA0ED6901D3);
    StratagemPayloadItemTypeList.push_back(0x11C27D3BABB38956);
    StratagemPayloadItemTypeList.push_back(0xA7EE1EBF58FCDF1F);
    StratagemPayloadItemTypeList.push_back(0x4FB0F8C02F55C82B);
    StratagemPayloadItemTypeList.push_back(0x1D5943301A29C940);
    StratagemPayloadItemTypeList.push_back(0xCF5F176E0E322BE1);
    StratagemPayloadItemTypeList.push_back(0xE88A5AA58ABB9D81);
    StratagemPayloadItemTypeList.push_back(0x4F749E2EE26F532D);
    StratagemPayloadItemTypeList.push_back(0x3F92BA65EF65CCA9);
    StratagemPayloadItemTypeList.push_back(0xCE063AA33D95A812);
    StratagemPayloadItemTypeList.push_back(0x85C1EDB038EC24E);
    StratagemPayloadItemTypeList.push_back(0xC3C1BBA1F445A81E);
    StratagemPayloadItemTypeList.push_back(0x1D8F262F56227FB);
    StratagemPayloadItemTypeList.push_back(0x2E9D0BDC48B09E60);
    StratagemPayloadItemTypeList.push_back(0xE75CD68E858A12AC);
    StratagemPayloadItemTypeList.push_back(0xEEA5E3CEF1E12C14);
    StratagemPayloadItemTypeList.push_back(0x8886D27A6F6DB8C);
    StratagemPayloadItemTypeList.push_back(0x76DD5D4F4360204);
    StratagemPayloadItemTypeList.push_back(0xDFC8B9519169B67A);
}

__declspec(noinline) void GetProjIDHook()
{
    BYTE b = *(BYTE*)AOB_GetProjectileID;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_GetProjectileID);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_GetProjectileID);
    }
}

__declspec(noinline) void InstantChargeQuasuarPatch()
{
    BYTE b = *(BYTE*)AOB_InstantChargeQuasar;
    if (b == 0xF3)
    {
        // mov [rsi+rdi+08],42C80000 (float 100)
        PatchHelper(AOB_InstantChargeQuasar, { 0xC7 ,0x44 ,0x3E ,0x08 ,0x00 ,0x00 ,0xC8 ,0x42,0x90,0x90,0x90,0x90,0x90,0x90,0x90 });
    }
    else
    {
        PatchHelper(AOB_InstantChargeQuasar, { 0xF3 ,0x0F ,0x10 ,0x4C ,0x37, 0x08 ,0xF3 ,0x41 ,0x0F ,0x10 ,0x95 ,0x94 ,0x00 ,0x00 ,0x00 });
    }
}


__declspec(noinline) void InstantChargeRailgunPatch()
{
    BYTE b = *(BYTE*)(AOB_InstantChargeRailgun + 4);
    if (b == 0x44)
    {
        PatchHelper((AOB_InstantChargeRailgun + 4), { 0x64 }); // movss[r15 + r14 * 8 + 04], xmm4
    }
    else
    {
        PatchHelper((AOB_InstantChargeRailgun + 4), { 0x44 }); // movss [r15+r14*8+04],xmm0
    }
}

__declspec(noinline) void StratagemPayloadItemTypeHook()
{
    BYTE b = *(BYTE*)AOB_StratagemPayloadItemType;
    if (b == 0x4C)
    {
        MH_EnableHook((LPVOID)AOB_StratagemPayloadItemType);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_StratagemPayloadItemType);
    }
}

__declspec(noinline) void NoStationaryTurretOverheatPatch()
{
    BYTE b = *(BYTE*)AOB_NoStationaryTurretOverheat;
    if (b == 0xF3)
    {
        PatchHelper(AOB_NoStationaryTurretOverheat, { 0x90,0x90 ,0x90,0x90,0x90 ,0x90 });
    }
    else
    {
        PatchHelper(AOB_NoStationaryTurretOverheat, { 0xF3 ,0x0F ,0x58 ,0x44 ,0xB7 ,0x04 });
    }
}

__declspec(noinline) void JetpackPatch()
{
    BYTE b = *(BYTE*)AOB_Jetpack;
    if (b == 0x89)
    {
        PatchHelper(AOB_Jetpack, { 0x90,0x90 ,0x90 });
    }
    else
    {
        PatchHelper(AOB_Jetpack, { 0x89 ,0x04 ,0xB9 });
    }
}

__declspec(noinline) void HiddenOutpostsHook()
{
    BYTE b = *(BYTE*)AOB_HiddenOutposts;
    if (b == 0x80)
    {
        MH_EnableHook((LPVOID)AOB_HiddenOutposts);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_HiddenOutposts);
    }
}

__declspec(noinline) void InstantCompleteOutpostHook()
{
    BYTE b = *(BYTE*)AOB_InstantCompleteOutpost;
    if (b == 0x49)
    {
        MH_EnableHook((LPVOID)AOB_InstantCompleteOutpost);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_InstantCompleteOutpost);
    }
}

__declspec(noinline) void BoundaryPatch()
{
    BYTE b = *(BYTE*)AOB_Boundary;
    if (b == 0x0F)
    {
        PatchHelper(AOB_Boundary, { 0xE9,0xF8 ,0x00 ,0x00 ,0x00,0x90 });
    }
    else
    {
        PatchHelper(AOB_Boundary, { 0x0F ,0x85 ,0xF7 ,0x00 ,0x00,0x00 });
    }
}


__declspec(noinline) void LaserOverheatPatch()
{
    BYTE b = *(BYTE*)AOB_LaserOverheat;
    if (b == 0xF3)
    {
        PatchHelper(AOB_LaserOverheat, { 0x90,0x90 ,0x90 ,0x90 ,0x90,0x90 });
        PatchHelper(AOB_LaserOverheat2, { 0x90,0x90 ,0x90 ,0x90 ,0x90,0x90 });
    }
    else
    {
        PatchHelper(AOB_LaserOverheat, { 0xF3 ,0x0F ,0x11 ,0x4C ,0xA8 ,0x04 });
        PatchHelper(AOB_LaserOverheat2, { 0xF3 ,0x0F ,0x11, 0x44 ,0xAF ,0x04 });
    }
}


__declspec(noinline) void InfAmmoLaserPatch()
{
    BYTE b = *(BYTE*)AOB_InfAmmoLaser;
    if (b == 0x42)
    {
        PatchHelper(AOB_InfAmmoLaser, { 0x90,0x90 ,0x90 ,0x90 ,0x90 });
    }
    else
    {
        PatchHelper(AOB_InfAmmoLaser, { 0x42 ,0x83 ,0x2C ,0x81 ,0x01 });
    }
}


__declspec(noinline) void InstaDropHook()
{
    BYTE b = *(BYTE*)AOB_InstantDrop;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_InstantDrop);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_InstantDrop);
    }
}

__declspec(noinline) void InfBackpackPatch()
{
    BYTE b = *(BYTE*)AOB_InfBackpack;
    if (b == 0x2B)
    {
        PatchHelper(AOB_InfBackpack, { 0x90,0x90 });
    }
    else
    {
        PatchHelper(AOB_InfBackpack, { 0x2B,0xC8 });
    }
}

__declspec(noinline) void InstaCompleteHook()
{
    BYTE b = *(BYTE*)AOB_InstaComplete1;
    if (b == 0x45)
    {
        MH_EnableHook((LPVOID)AOB_InstaComplete1);
        MH_EnableHook((LPVOID)AOB_InstaComplete2);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_InstaComplete1);
        MH_DisableHook((LPVOID)AOB_InstaComplete2);
    }
}

__declspec(noinline) void ArmoryEditorHook()
{
    BYTE b = *(BYTE*)AOB_ReadWeapon;
    if (b == 0x66)
    {
        MH_EnableHook((LPVOID)AOB_ReadWeapon);
        MH_EnableHook((LPVOID)AOB_BlitzerFire);
        MH_EnableHook((LPVOID)AOB_BlitzerDamage);
        MH_EnableHook((LPVOID)AOB_FireRate);
        MH_EnableHook((LPVOID)AOB_ReadDMG);
        MH_EnableHook((LPVOID)AOB_DamageSpecial);
        MH_EnableHook((LPVOID)AOB_ReadSpecialDamage);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_ReadWeapon);
        MH_DisableHook((LPVOID)AOB_BlitzerFire);
        MH_DisableHook((LPVOID)AOB_BlitzerDamage);
        MH_DisableHook((LPVOID)AOB_FireRate);
        MH_DisableHook((LPVOID)AOB_ReadDMG);
        MH_DisableHook((LPVOID)AOB_DamageSpecial);
        MH_DisableHook((LPVOID)AOB_ReadSpecialDamage);
        pWeapon = 0;
        pFireRate = 0;
    }
}

__declspec(noinline) void SpeedhackHook()
{
    BYTE b = *(BYTE*)AOB_Speedhack;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_Speedhack);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_Speedhack);
    }
}


__declspec(noinline) void SamplesHook()
{
    BYTE b = *(BYTE*)AOB_Samples;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_Samples);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_Samples);
    }
}

__declspec(noinline) void MapPatch()
{
    BYTE b = *(BYTE*)aob_CheckIsBlipSeen;
    if (b == 0x44)
    {
        PatchHelper(aob_CheckIsBlipSeen, { 0x66 ,0x41 ,0xBD ,0x01 ,0x00 });
        PatchHelper(aob_CheckIfAlienHiveSeen, { 0x0F,0x87 });
        PatchHelper(aob_CheckIfMinorInterestBlipIsDiscovered, { 0x90,0xE9 });
        PatchHelper(aob_CheckMissionBlip, { 0x90,0xE9 });
    }
    else
    {
        PatchHelper(aob_CheckIsBlipSeen, { 0x44 ,0x0F ,0xB6 ,0x68 ,0x33 });
        PatchHelper(aob_CheckIfAlienHiveSeen, { 0x0F,0x87 });
        PatchHelper(aob_CheckIfMinorInterestBlipIsDiscovered, { 0x0F,0x85 });
        PatchHelper(aob_CheckMissionBlip, { 0x0F,0x85 });
    }
}

__declspec(noinline) void UnlockPatch()
{
    BYTE b = *(BYTE*)AOB_Unlock1;
    if (b == 0x48)
    {
        PatchHelper(AOB_Unlock1, { 0xB0,0x01,0xC3 });
        PatchHelper(AOB_Unlock2, { 0xB0,0x01,0xC3 });
        PatchHelper(AOB_Unlock3, { 0xB0,0x01,0xC3 });
    }
    else
    {
        PatchHelper(AOB_Unlock1, { 0x48 ,0x89 ,0x5C });
        PatchHelper(AOB_Unlock2, { 0x83 ,0xB9 ,0xA0 });
        PatchHelper(AOB_Unlock3, { 0x48 ,0x83 ,0xEC });
    }
}

__declspec(noinline) void BunkerPatch()
{
    BYTE b = *(BYTE*)(AOB_Bunker + 6);
    if (b == 0x01)
    {
        PatchHelper((AOB_Bunker + 6), { 0x03 });
    }
    else
    {
        PatchHelper((AOB_Bunker + 6), { 0x01 });
    }
}

__declspec(noinline) void SwayPatch()
{
    BYTE b = *(BYTE*)AOB_Sway;
    if (b == 0x90)
    {
        PatchHelper(AOB_Sway, { 0xF3 ,0x0F ,0x51 ,0xFA });
    }
    else
    {
        PatchHelper(AOB_Sway, { 0x90,0x90,0x90,0x90 });
    }
}
__declspec(noinline) void RecoilPatch()
{
    BYTE b = *(BYTE*)AOB_Recoil;
    if (b == 0x48)
    {
        PatchHelper(AOB_Recoil, { 0xC3 });
    }
    else
    {
        PatchHelper(AOB_Recoil, { 0x48 });
    }
}

__declspec(noinline) void FastExtractHook()
{
    BYTE b = *(BYTE*)AOB_Extract;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_Extract);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_Extract);
    }
}

__declspec(noinline) void StratagemsPatch()
{
    BYTE b = *(BYTE*)AOB_Stratagems;
    if (b == 0x0F)
    {
        PatchHelper(AOB_Stratagems, { 0x90,0xE9 });
    }
    else
    {
        PatchHelper(AOB_Stratagems, { 0x0F,0x86 });
    }
}

__declspec(noinline) void InfShieldHook()
{
    BYTE b = *(BYTE*)AOB_InfShield;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_InfShield);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_InfShield);
    }
}


__declspec(noinline) void InfHealthHook()
{
    BYTE b = *(BYTE*)AOB_HealthRead;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_HealthRead);
        MH_EnableHook((LPVOID)AOB_HealthWrite);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_HealthRead);
        MH_DisableHook((LPVOID)AOB_HealthWrite);
    }
}

__declspec(noinline) void InfSprintPatch()
{
    BYTE b = *(BYTE*)AOB_InfSprint;
    if (b == 0x90)
    {
        PatchHelper(AOB_InfSprint, { 0xF3 ,0x41 ,0x0F ,0x11 ,0x08 });
    }
    else
    {
        PatchHelper(AOB_InfSprint, { 0x90,0x90,0x90,0x90,0x90 });
    }
}

__declspec(noinline) void InfSyringePatch()
{
    BYTE b = *(BYTE*)AOB_InfSyringes;
    if (b == 0x90)
    {
        PatchHelper(AOB_InfSyringes, { 0x41 ,0x89 ,0x54 ,0xCF ,0x04 });
    }
    else
    {
        PatchHelper(AOB_InfSyringes, { 0x90,0x90,0x90,0x90,0x90 });
    }
}
__declspec(noinline) void InfNadePatch()
{
    BYTE b = *(BYTE*)AOB_InfNades;
    if (b == 0x90)
    {
        PatchHelper(AOB_InfNades, { 0x42 ,0xFF ,0x0C ,0xE0 });
    }
    else
    {
        PatchHelper(AOB_InfNades, { 0x90,0x90,0x90,0x90 });
    }
}
__declspec(noinline) void NoReloadPatch()
{
    BYTE b = *(BYTE*)AOB_NoReload;
    if (b == 0x90)
    {
        PatchHelper(AOB_NoReload, { 0x44,0x89 ,0x74 ,0x01 ,0x04 });
    }
    else
    {
        PatchHelper(AOB_NoReload, { 0x90,0x90,0x90,0x90,0x90 });
    }
}

__declspec(noinline) void NoReload2Hook()
{
    BYTE b = *(BYTE*)AOB_NoReload2;
    if (b != 0xE9)
    {
        MH_EnableHook((LPVOID)AOB_NoReload2);
    }
    else
    {
        MH_DisableHook((LPVOID)AOB_NoReload2);
    }
}

__declspec(noinline) void InfAmmoPatch()
{
    BYTE b = *(BYTE*)AOB_InfAmmo;
    if (b == 0x90)
    {
        PatchHelper(AOB_InfAmmo, { 0x42, 0x83, 0x2C, 0x81, 0x01 });
    }
    else
    {
        PatchHelper(AOB_InfAmmo, { 0x90,0x90,0x90,0x90,0x90 });
    }
}

// ASM Caves:

int IDSwap(int iD)
{
    for (auto o : ProjectileList)
    {
        if (o.Original == iD)
        {
            return o.Swap;
        }
    }
    return 0;
}

__attribute__((naked)) void GetProjIDASM()
{
    __asm
    {
        cmp byte ptr[bPlayerOnly], 1
        je PlayerCheck
        mov iProjID, eax
        jmp SkipCheck

        PlayerCheck:
        push rbx
        mov rbx,[rsp+490h] // Function that calls this function
        movzx ebx,byte ptr[rbx]
        cmp ebx,45h
        jne NotPlayer
            mov iProjID, eax
        NotPlayer:
        pop rbx

        SkipCheck:
        mov edi, eax
        mov ecx, eax
        call IDSwap
        test eax, eax
        je Original
        mov edi, eax
        Original :
        mov eax, edi
        mov rdi, GetProjIDReturn
        jmp rdi
    }
}

float fInstantDrop = 0.5f;
__attribute__((naked)) void InstantDropASM()
{
    __asm
    {
        comiss xmm0, [fInstantDrop]
        jna short Skip
        movss xmm0, [fInstantDrop]
        Skip:
        mov rdx, InstantDropReturn
            jmp rdx
    }
}

__attribute__((naked)) void StratagemPayloadItemTypeASM()
{
    __asm
    {
        mov r8, PayloadHash
        test r8, r8
        jne short Skip
        mov r8, [rbp + 000000E0h]
        Skip:
        mov r9, StratagemPayloadItemTypeReturn
            jmp r9
    }
}

__attribute__((naked)) void HiddenOutpostsASM()
{
    __asm
    {
        mov byte ptr[rsi + 0000028Ch], 0Fh
        mov byte ptr[rsi + 00000294h], 04h
        mov byte ptr[rsi + 00000296h], 01h
        mov byte ptr[rsi + 00000297h], 01h
        mov rdx, HiddenOutpostsReturn
        jmp rdx
    }
}

__attribute__((naked)) void InstantCompleteOutpostASM()
{
    __asm
    {
        mov rax, [r12 + 50h]
        mov byte ptr[rcx + rax + 31h], 1
        mov rax, InstantCompleteOutpostReturn
        jmp rax
    }
}


__attribute__((naked)) void InstaComplete1ASM()
{
    __asm
    {
        mov dword ptr[r10 + 0018h], 1
        mov r9, InstaComplete1Return
        jmp r9
    }
}

__attribute__((naked)) void InstaComplete2ASM()
{
    __asm
    {
        mov dword ptr[rsi + 0020h], 2
        mov rax, InstaComplete2Return
        jmp rax
    }
}

__attribute__((naked)) void ArmoryEditor1ASM()
{
    __asm
    {
        mov pWeapon, rax
        mov r8, ArmoryEditor1Return
        jmp r8
    }
}

__attribute__((naked)) void ArmoryEditor2ASM()
{
    __asm
    {
        lea rcx, [rax - 4h]
        mov pFireRate, rcx
        mov rcx, ArmoryEditor2Return
        jmp rcx
    }
}
__attribute__((naked)) void ArmoryEditor3ASM()
{
    __asm
    {
        mov pWeapon, rax
        mov r15, ArmoryEditor3Return
        jmp r15
    }
}
__attribute__((naked)) void ArmoryEditor4ASM()
{
    __asm
    {
        push rbx
        mov ebx, [rax + 8]
        test ebx, ebx
        je short Skip
        mov pFireRate, rax
        Skip :
        pop rbx
            mov rsi, ArmoryEditor4Return
            jmp rsi
    }
}
__attribute__((naked)) void ArmoryEditor5ASM()
{
    __asm
    {
        mov pWeapon, rax
        mov r8, ArmoryEditor5Return
        jmp r8
    }
}
__attribute__((naked)) void ArmoryEditor6ASM()
{
    __asm
    {
        mov pWeapon, rax
        mov r8, ArmoryEditor6Return
        jmp r8
    }
}
__attribute__((naked)) void ArmoryEditor7ASM()
{
    __asm
    {
        mov pWeapon, rax
        mov r8, ArmoryEditor7Return
        jmp r8
    }
}



uintptr_t MissionHash = 0xF2F926ABF2F926AB;
uintptr_t ShipHash = 0x712947D6712947D6;
__attribute__((naked)) void SpeedhackASM()
{
    __asm
    {
        push rbx
        mov rbx, MissionHash // local hash for the missions
        cmp qword ptr[rax], rbx
        je Player
        mov rbx, ShipHash // local hash for the ship
        cmp qword ptr[rax], rbx
        je Player
        jmp Enemy
        Player :
        lea rbx, fPlayerSpeed
            mulss xmm2, [rbx]
            pop rbx
            mulss xmm2, [r14 + 00Ch]
            mulss xmm2, [r14 + 0010h]
            movaps xmm4, xmm2
            mov rax, SpeedhackReturn
            jmp rax
            Enemy :
        pop rbx
            cmp byte ptr[bFreezeEnemies], 1
            jne short Skip
            xorps xmm2, xmm2
            Skip :
        mulss xmm2, [r14 + 0Ch]
            mulss xmm2, [r14 + 10h]
            movaps xmm4, xmm2
            mov rax, SpeedhackReturn
            jmp rax
    }
}

__attribute__((naked)) void SamplesASM()
{
    __asm
    {
        mov eax, NormaleSamples
        mov[rcx + 04h], eax
        mov eax, RareSamples
        mov[rcx + 08h], eax
        mov eax, SuperSamples
        mov[rcx + 0Ch], eax
        xor eax, eax
        mov r8, SamplesReturn
        jmp r8
    }
}

float compare_float_check = 1.0f;
__attribute__((naked)) void ExtractASM()
{
    __asm
    {
        //subss xmm0, xmm3
        ucomiss xmm0, DWORD ptr[compare_float_check]
        jbe short Skip
        movss xmm0, DWORD ptr[compare_float_check]
        Skip:
        mov rdx, ExtractReturn
            jmp rdx
            //mov edx, 6109F766h
            //movss[rcx + rax], xmm0
    }
}
__attribute__((naked)) void ShieldCheatASM()
{
    __asm
    {
        test rsi, rsi
        je short Skip
        push rax
        mov rax, 12C8D71AC3897A5Ch
        cmp qword ptr[rsi], rax
        jne short Skip
        pop rax
        subss xmm1, xmm1
        movss[r14 + r15 + 0008h], xmm1
        movaps xmm2, xmm1
        mov rax, ShieldReturn
        add rax, 5
        jmp rax
        Skip :
        pop rax
            mov rax, ShieldReturn
            add rax, 5
            jmp rax
    }
}

__attribute__((naked)) void HealthWriteASM()
{
    __asm
    {
        push rbx
        mov rbx, pPlayerHealth
        lea rcx, [r8 + rax]
        cmp rbx, rcx
        jne short Skip
        mov ebp, 270Fh
        Skip :
        pop rbx
            mov rcx, HealthWriteReturn
            jmp rcx
    }
}
__attribute__((naked)) void HealthReadASM()
{
    __asm
    {
        lea rax, [rax + rcx * 8]
        mov dword ptr[rax], 9999
        mov pPlayerHealth, rax
        mov rax, HealthReadReturn
        jmp rax
    }
}
__attribute__((naked)) void NoReloadV2ASM()
{
    __asm
    {
        cmp rcx, 0
        jne short Skip
        ret
        Skip :
        mov rax, NoReloadV2Return
            jmp rax
    }
}
__attribute__((naked)) void TimeCheatAsm()
{
    __asm
    {
        cmp byte ptr[bFreezeTimer], 0
        je Skip
        maxss xmm0, xmm10
        jmp End
        Skip :
        cmp byte ptr[bSetTimer], 0
            je Skip2
            mov byte ptr[bSetTimer], 0
            lea rax, fTimer
            movss xmm0, [rax]
            Skip2 :
            subss xmm0, xmm7
            maxss xmm0, xmm15
            End :
        lea rax, TimeReturn
            mov rax, [rax]
            add rax, 9
            jmp rax
    }
}
