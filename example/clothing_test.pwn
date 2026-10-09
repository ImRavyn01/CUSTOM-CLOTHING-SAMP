/*
 *  Custom CJ Clothing Plugin — Example Gamemode
 *  clothing_test.pwn
 *
 *  This filterscript demonstrates all features of the clothing plugin.
 *  Load it alongside your main gamemode as a filterscript.
 *
 *  Requirements:
 *    - clothing.dll in server/plugins/
 *    - clothing.asi installed on client
 *    - clothing.inc in pawno/include/
 *    - server.cfg: plugins clothing.dll
 */

#include <a_samp>
#include <clothing>

// ============================================================================
//  COLORS
// ============================================================================
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_GREEN     0x00FF00FF
#define COLOR_RED       0xFF0000FF
#define COLOR_YELLOW    0xFFFF00FF
#define COLOR_CYAN      0x00FFFFFF
#define COLOR_ORANGE    0xFF8800FF

// ============================================================================
//  PREDEFINED OUTFITS
// ============================================================================
// These use built-in GTA SA texture/model names from player.img

// Outfit 1: Casual Street
#define OUTFIT1_TORSO_TEX   "vestblack"
#define OUTFIT1_TORSO_MDL   "vest"
#define OUTFIT1_LEGS_TEX    "jeansdenim"
#define OUTFIT1_LEGS_MDL    "jeans"
#define OUTFIT1_SHOES_TEX   "sneakerbincblk"
#define OUTFIT1_SHOES_MDL   "sneaker"

// Outfit 2: Business
#define OUTFIT2_TORSO_TEX   "shirtdress"
#define OUTFIT2_TORSO_MDL   "shirtdress"
#define OUTFIT2_LEGS_TEX    "chinosblack"
#define OUTFIT2_LEGS_MDL    "chinos"
#define OUTFIT2_SHOES_TEX   "shoedressblk"
#define OUTFIT2_SHOES_MDL   "shoedress"

// Outfit 3: Gangster
#define OUTFIT3_TORSO_TEX   "hockeytop"
#define OUTFIT3_TORSO_MDL   "hockey"
#define OUTFIT3_LEGS_TEX    "tracktr"
#define OUTFIT3_LEGS_MDL    "tracktr"
#define OUTFIT3_SHOES_TEX   "baskball"
#define OUTFIT3_SHOES_MDL   "baskball"

// ============================================================================
//  CUSTOM CLOTHING MODEL IDs (for AddClothingModel)
// ============================================================================
#define CUSTOM_TSHIRT_1     30001
#define CUSTOM_JEANS_1      30002
#define CUSTOM_SNEAKER_1    30003

// ============================================================================
//  GAMEMODE CALLBACKS
// ============================================================================

public OnFilterScriptInit()
{
    print("============================================");
    print(" Custom CJ Clothing — Test Filterscript");
    print(" Version: 0.0.1 (First Release)");
    print("============================================");

    // Check plugin version
    new version[32];
    GetClothingPluginVersion(version, sizeof(version));
    printf(" Plugin Version: %s", version);

    // Register custom clothing models (place .dff/.txd in server models/clothing/)
    // Uncomment these when you have actual custom model files:
    //
    // AddClothingModel(0, CUSTOM_TSHIRT_1, "custom_tshirt1.dff", "custom_tshirt1.txd");
    // AddClothingModel(0, CUSTOM_JEANS_1,  "custom_jeans1.dff",  "custom_jeans1.txd");
    // AddClothingModel(0, CUSTOM_SNEAKER_1,"custom_sneaker1.dff","custom_sneaker1.txd");

    print(" Clothing system initialized!");
    print("============================================");
    return 1;
}

public OnFilterScriptExit()
{
    print(" Custom CJ Clothing filterscript unloaded.");
    return 1;
}

public OnPlayerConnect(playerid)
{
    SendClientMessage(playerid, COLOR_CYAN, "[Clothing] Custom CJ Clothing System aktif!");
    SendClientMessage(playerid, COLOR_WHITE, "Ketik /clothing untuk melihat command yang tersedia.");
    return 1;
}

public OnPlayerSpawn(playerid)
{
    // Pastikan player menggunakan skin CJ
    new skinid = GetPlayerSkin(playerid);
    if (skinid == 0) // CJ skin
    {
        // Beri default clothing saat spawn
        SetPlayerClothes(playerid, CLOTHING_TORSO, OUTFIT1_TORSO_TEX, OUTFIT1_TORSO_MDL);
        SetPlayerClothes(playerid, CLOTHING_LEGS, OUTFIT1_LEGS_TEX, OUTFIT1_LEGS_MDL);
        SetPlayerClothes(playerid, CLOTHING_SHOES, OUTFIT1_SHOES_TEX, OUTFIT1_SHOES_MDL);

        SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Default clothing applied.");
    }
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    // Cleanup: remove all clothing data
    RemoveAllPlayerClothes(playerid);
    return 1;
}

// ============================================================================
//  COMMANDS
// ============================================================================

// /clothing — show help
public OnPlayerCommandText(playerid, cmdtext[])
{
    // ---- /clothing ----
    if (!strcmp(cmdtext, "/clothing", true, 9) && (cmdtext[9] == '\0' || cmdtext[9] == ' '))
    {
        SendClientMessage(playerid, COLOR_ORANGE, "=== Custom CJ Clothing Commands ===");
        SendClientMessage(playerid, COLOR_WHITE, "/outfit [1-3]  — Pilih preset outfit");
        SendClientMessage(playerid, COLOR_WHITE, "/settorso [tex] [mdl]  — Set baju torso");
        SendClientMessage(playerid, COLOR_WHITE, "/setlegs [tex] [mdl]   — Set celana");
        SendClientMessage(playerid, COLOR_WHITE, "/setshoes [tex] [mdl]  — Set sepatu");
        SendClientMessage(playerid, COLOR_WHITE, "/sethat [tex] [mdl]    — Set topi");
        SendClientMessage(playerid, COLOR_WHITE, "/setglasses [tex] [mdl] — Set kacamata");
        SendClientMessage(playerid, COLOR_WHITE, "/removeclothes [slot]  — Hapus slot (0-17)");
        SendClientMessage(playerid, COLOR_WHITE, "/removeall             — Hapus semua baju");
        SendClientMessage(playerid, COLOR_WHITE, "/myclothes             — Lihat baju yang dipakai");
        SendClientMessage(playerid, COLOR_WHITE, "/clothesinfo           — Info slot names");
        return 1;
    }

    // ---- /outfit [1-3] ----
    if (!strcmp(cmdtext, "/outfit", true, 7))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new outfitId;
        if (sscanf_simple(cmdtext[8], outfitId) == 0 || outfitId < 1 || outfitId > 3)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /outfit [1-3]");
            return 1;
        }

        // Remove existing clothes first
        RemoveAllPlayerClothes(playerid);

        switch (outfitId)
        {
            case 1:
            {
                SetPlayerFullOutfit(playerid,
                    OUTFIT1_TORSO_TEX, OUTFIT1_TORSO_MDL,
                    OUTFIT1_LEGS_TEX, OUTFIT1_LEGS_MDL,
                    OUTFIT1_SHOES_TEX, OUTFIT1_SHOES_MDL);
                SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Outfit 1: Casual Street applied!");
            }
            case 2:
            {
                SetPlayerFullOutfit(playerid,
                    OUTFIT2_TORSO_TEX, OUTFIT2_TORSO_MDL,
                    OUTFIT2_LEGS_TEX, OUTFIT2_LEGS_MDL,
                    OUTFIT2_SHOES_TEX, OUTFIT2_SHOES_MDL);
                SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Outfit 2: Business applied!");
            }
            case 3:
            {
                SetPlayerFullOutfit(playerid,
                    OUTFIT3_TORSO_TEX, OUTFIT3_TORSO_MDL,
                    OUTFIT3_LEGS_TEX, OUTFIT3_LEGS_MDL,
                    OUTFIT3_SHOES_TEX, OUTFIT3_SHOES_MDL);
                SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Outfit 3: Gangster applied!");
            }
        }
        return 1;
    }

    // ---- /settorso [texture] [model] ----
    if (!strcmp(cmdtext, "/settorso", true, 9))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new tex[64], mdl[64];
        if (sscanf_two(cmdtext[10], tex, mdl) < 2)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /settorso [texture] [model]");
            SendClientMessage(playerid, COLOR_YELLOW, "Contoh: /settorso vestblack vest");
            return 1;
        }

        if (SetPlayerClothes(playerid, CLOTHING_TORSO, tex, mdl))
            SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Torso berhasil diubah!");
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal mengubah torso!");
        return 1;
    }

    // ---- /setlegs [texture] [model] ----
    if (!strcmp(cmdtext, "/setlegs", true, 8))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new tex[64], mdl[64];
        if (sscanf_two(cmdtext[9], tex, mdl) < 2)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /setlegs [texture] [model]");
            SendClientMessage(playerid, COLOR_YELLOW, "Contoh: /setlegs jeansdenim jeans");
            return 1;
        }

        if (SetPlayerClothes(playerid, CLOTHING_LEGS, tex, mdl))
            SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Legs berhasil diubah!");
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal mengubah legs!");
        return 1;
    }

    // ---- /setshoes [texture] [model] ----
    if (!strcmp(cmdtext, "/setshoes", true, 9))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new tex[64], mdl[64];
        if (sscanf_two(cmdtext[10], tex, mdl) < 2)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /setshoes [texture] [model]");
            return 1;
        }

        if (SetPlayerClothes(playerid, CLOTHING_SHOES, tex, mdl))
            SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Shoes berhasil diubah!");
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal mengubah shoes!");
        return 1;
    }

    // ---- /sethat [texture] [model] ----
    if (!strcmp(cmdtext, "/sethat", true, 7))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new tex[64], mdl[64];
        if (sscanf_two(cmdtext[8], tex, mdl) < 2)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /sethat [texture] [model]");
            return 1;
        }

        if (SetPlayerClothes(playerid, CLOTHING_HAT, tex, mdl))
            SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Hat berhasil diubah!");
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal mengubah hat!");
        return 1;
    }

    // ---- /setglasses [texture] [model] ----
    if (!strcmp(cmdtext, "/setglasses", true, 11))
    {
        if (!IsPlayerCJSkin(playerid))
        {
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Clothing hanya bekerja pada skin CJ (ID 0)!");
            return 1;
        }

        new tex[64], mdl[64];
        if (sscanf_two(cmdtext[12], tex, mdl) < 2)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /setglasses [texture] [model]");
            return 1;
        }

        if (SetPlayerClothes(playerid, CLOTHING_GLASSES, tex, mdl))
            SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Glasses berhasil diubah!");
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal mengubah glasses!");
        return 1;
    }

    // ---- /removeclothes [slot] ----
    if (!strcmp(cmdtext, "/removeclothes", true, 14))
    {
        new slotId;
        if (sscanf_simple(cmdtext[15], slotId) == 0 || slotId < 0 || slotId > 17)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "Penggunaan: /removeclothes [0-17]");
            return 1;
        }

        new slotName[32];
        GetClothingSlotName(slotId, slotName);

        if (RemovePlayerClothes(playerid, slotId))
        {
            new str[128];
            format(str, sizeof(str), "[Clothing] %s berhasil dihapus!", slotName);
            SendClientMessage(playerid, COLOR_GREEN, str);
        }
        else
            SendClientMessage(playerid, COLOR_RED, "[ERROR] Gagal menghapus clothing!");
        return 1;
    }

    // ---- /removeall ----
    if (!strcmp(cmdtext, "/removeall", true))
    {
        RemoveAllPlayerClothes(playerid);
        SendClientMessage(playerid, COLOR_GREEN, "[Clothing] Semua pakaian dihapus!");
        return 1;
    }

    // ---- /myclothes ----
    if (!strcmp(cmdtext, "/myclothes", true))
    {
        new count = GetPlayerClothingCount(playerid);
        new str[128];
        format(str, sizeof(str), "[Clothing] Kamu memakai %d item pakaian:", count);
        SendClientMessage(playerid, COLOR_ORANGE, str);

        new tex[64], mdl[64], slotName[32];
        for (new i = 0; i < MAX_CLOTHING_SLOTS; i++)
        {
            if (IsPlayerWearingClothes(playerid, i))
            {
                GetPlayerClothes(playerid, i, tex, sizeof(tex), mdl, sizeof(mdl));
                GetClothingSlotName(i, slotName);
                format(str, sizeof(str), "  [%d] %s: %s / %s", i, slotName, tex, mdl);
                SendClientMessage(playerid, COLOR_WHITE, str);
            }
        }
        return 1;
    }

    // ---- /clothesinfo ----
    if (!strcmp(cmdtext, "/clothesinfo", true))
    {
        SendClientMessage(playerid, COLOR_ORANGE, "=== Clothing Slot IDs ===");
        new str[128], slotName[32];
        for (new i = 0; i < MAX_CLOTHING_SLOTS; i++)
        {
            GetClothingSlotName(i, slotName);
            format(str, sizeof(str), "  [%d] %s", i, slotName);
            SendClientMessage(playerid, COLOR_WHITE, str);
        }
        return 1;
    }

    return 0;
}

// ============================================================================
//  CLOTHING CALLBACKS
// ============================================================================

public OnPlayerClothesChange(playerid, bodypart, const texture[], const model[])
{
    new name[MAX_PLAYER_NAME], str[256], slotName[32];
    GetPlayerName(playerid, name, sizeof(name));
    GetClothingSlotName(bodypart, slotName);

    if (strlen(texture) > 0)
        format(str, sizeof(str), "[Clothing] %s mengubah %s ke: %s/%s", name, slotName, texture, model);
    else
        format(str, sizeof(str), "[Clothing] %s menghapus %s", name, slotName);

    printf("%s", str); // Log to server console
    return 1;
}

public OnPlayerClothesLoaded(playerid, slotCount)
{
    new str[128];
    format(str, sizeof(str), "[Clothing] %d clothing slots telah di-sync ke client.", slotCount);
    SendClientMessage(playerid, COLOR_CYAN, str);
    return 1;
}

// ============================================================================
//  HELPER: Simple string parsing (since we don't require sscanf plugin)
// ============================================================================

stock sscanf_simple(const str[], &value)
{
    if (strlen(str) == 0) return 0;
    new i = 0;
    // Skip whitespace
    while (str[i] == ' ' && str[i] != '\0') i++;
    if (str[i] == '\0') return 0;
    value = strval(str[i]);
    // Parse full number
    new numStr[12];
    new j = 0;
    while (str[i] != ' ' && str[i] != '\0' && j < 11)
    {
        numStr[j] = str[i];
        i++;
        j++;
    }
    numStr[j] = '\0';
    value = strval(numStr);
    return 1;
}

stock sscanf_two(const str[], dest1[], dest2[], maxlen1 = sizeof(dest1), maxlen2 = sizeof(dest2))
{
    new i = 0, j = 0, count = 0;

    // Skip leading whitespace
    while (str[i] == ' ' && str[i] != '\0') i++;
    if (str[i] == '\0') return 0;

    // Read first word
    j = 0;
    while (str[i] != ' ' && str[i] != '\0' && j < maxlen1 - 1)
    {
        dest1[j] = str[i];
        i++;
        j++;
    }
    dest1[j] = '\0';
    if (j > 0) count++;

    // Skip whitespace
    while (str[i] == ' ' && str[i] != '\0') i++;
    if (str[i] == '\0') return count;

    // Read second word
    j = 0;
    while (str[i] != ' ' && str[i] != '\0' && j < maxlen2 - 1)
    {
        dest2[j] = str[i];
        i++;
        j++;
    }
    dest2[j] = '\0';
    if (j > 0) count++;

    return count;
}
