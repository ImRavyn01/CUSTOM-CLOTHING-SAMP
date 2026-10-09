/*
 *  Custom CJ Clothing System for SA-MP 0.3.7 R2
 *  Pawn Include — clothing.inc
 *  Version: 1.0.0
 *
 *  Provides native functions to manage CJ's individual clothing parts.
 *  Requires: clothing.dll (server plugin) + clothing.asi (client plugin)
 *
 *  IMPORTANT: Clothing functions ONLY work on players using CJ skin (Skin ID 0).
 *  If a player is using a different skin, functions will return 0 (failure).
 */

#if defined _clothing_included
    #endinput
#endif
#define _clothing_included

// ============================================================================
//  PLUGIN VERSION
// ============================================================================
#define CLOTHING_VERSION                "1.0.0"
#define CLOTHING_PROTOCOL_VERSION       1

// ============================================================================
//  BODY PART SLOT CONSTANTS
//  Use these constants with SetPlayerClothes, GetPlayerClothes, etc.
// ============================================================================
#define CLOTHING_TORSO                  0   // Shirt, jacket, vest
#define CLOTHING_HEAD                   1   // Hair style
#define CLOTHING_LEGS                   2   // Trousers, shorts, jeans
#define CLOTHING_SHOES                  3   // Shoes, boots, sneakers
#define CLOTHING_TATTOO_LEFT_UPPER_ARM  4   // Tattoo - left upper arm
#define CLOTHING_TATTOO_LEFT_LOWER_ARM  5   // Tattoo - left lower arm
#define CLOTHING_TATTOO_RIGHT_UPPER_ARM 6   // Tattoo - right upper arm
#define CLOTHING_TATTOO_RIGHT_LOWER_ARM 7   // Tattoo - right lower arm
#define CLOTHING_TATTOO_BACK            8   // Tattoo - back
#define CLOTHING_TATTOO_LEFT_CHEST      9   // Tattoo - left chest
#define CLOTHING_TATTOO_RIGHT_CHEST     10  // Tattoo - right chest
#define CLOTHING_TATTOO_STOMACH         11  // Tattoo - stomach
#define CLOTHING_TATTOO_LOWER_BACK      12  // Tattoo - lower back
#define CLOTHING_NECKLACE               13  // Necklace, chain
#define CLOTHING_WATCH                  14  // Watch, bracelet
#define CLOTHING_GLASSES                15  // Glasses, shades
#define CLOTHING_HAT                    16  // Hat, cap, helmet
#define CLOTHING_EXTRA                  17  // Special clothes (valet, gimp, etc.)

#define MAX_CLOTHING_SLOTS              18

// ============================================================================
//  ERROR CODES
// ============================================================================
#define CLOTHING_ERROR_NONE             0   // Success
#define CLOTHING_ERROR_INVALID_PLAYER   -1  // Player ID invalid or not connected
#define CLOTHING_ERROR_INVALID_SLOT     -2  // Body part ID out of range (0-17)
#define CLOTHING_ERROR_NOT_CJ_SKIN      -3  // Player is not using CJ skin (ID 0)
#define CLOTHING_ERROR_ASSET_NOT_FOUND  -4  // Custom model ID not registered
#define CLOTHING_ERROR_PLUGIN_NOT_READY -5  // Plugin not initialized

// ============================================================================
//  NATIVE FUNCTIONS — Built-in GTA SA Clothing
// ============================================================================

/*
 * SetPlayerClothes
 *
 * Sets a clothing item on a specific body part for a player.
 * Uses built-in GTA SA texture and model names.
 *
 * @param playerid   The ID of the player
 * @param bodypart   The body part slot (use CLOTHING_* constants)
 * @param texture[]  The texture name (from GTA SA's .txd files)
 * @param model[]    The model name (from GTA SA's .dff files)
 *
 * @return           1 on success, 0 on failure
 *
 * Example:
 *   SetPlayerClothes(playerid, CLOTHING_TORSO, "vestblack", "vest");
 *   SetPlayerClothes(playerid, CLOTHING_LEGS, "jeansdenim", "jeans");
 *   SetPlayerClothes(playerid, CLOTHING_HAT, "capknitgrn", "capknit");
 */
native SetPlayerClothes(playerid, bodypart, const texture[], const model[]);

/*
 * GetPlayerClothes
 *
 * Gets the currently equipped clothing on a specific body part.
 *
 * @param playerid   The ID of the player
 * @param bodypart   The body part slot (use CLOTHING_* constants)
 * @param texture[]  Buffer to store the texture name
 * @param texLen     Size of the texture buffer (default: sizeof)
 * @param model[]    Buffer to store the model name
 * @param modLen     Size of the model buffer (default: sizeof)
 *
 * @return           1 if clothing exists, 0 if slot is empty, negative on error
 *
 * Example:
 *   new texture[64], model[64];
 *   if (GetPlayerClothes(playerid, CLOTHING_TORSO, texture, sizeof(texture), model, sizeof(model)))
 *   {
 *       printf("Torso: %s / %s", texture, model);
 *   }
 */
native GetPlayerClothes(playerid, bodypart, texture[], texLen = sizeof(texture), model[], modLen = sizeof(model));

/*
 * RemovePlayerClothes
 *
 * Removes the clothing item from a specific body part, reverting to default.
 *
 * @param playerid   The ID of the player
 * @param bodypart   The body part slot (use CLOTHING_* constants)
 *
 * @return           1 on success, 0 on failure
 */
native RemovePlayerClothes(playerid, bodypart);

/*
 * RemoveAllPlayerClothes
 *
 * Removes ALL clothing from the player, reverting to default CJ appearance.
 *
 * @param playerid   The ID of the player
 *
 * @return           1 on success, 0 on failure
 */
native RemoveAllPlayerClothes(playerid);

// ============================================================================
//  NATIVE FUNCTIONS — Custom Clothing Models (DFF/TXD)
// ============================================================================

/*
 * AddClothingModel
 *
 * Registers a custom clothing model (DFF/TXD pair) for use with SetPlayerCustomClothes.
 * Call this in OnGameModeInit or OnFilterScriptInit.
 * Custom model files should be placed in the server's "models/clothing/" folder.
 *
 * @param baseid     Base GTA model ID (used as fallback if custom fails to load)
 * @param newid      Your unique custom model ID (use range 30000-39999)
 * @param dffname[]  Filename of the .dff model file
 * @param txdname[]  Filename of the .txd texture file
 *
 * @return           1 on success, 0 on failure
 *
 * Example:
 *   AddClothingModel(0, 30001, "custom_tshirt.dff", "custom_tshirt.txd");
 *   AddClothingModel(0, 30002, "custom_jeans.dff", "custom_jeans.txd");
 */
native AddClothingModel(baseid, newid, const dffname[], const txdname[]);

/*
 * SetPlayerCustomClothes
 *
 * Sets a custom clothing model on a specific body part.
 * The model must be registered with AddClothingModel first.
 *
 * @param playerid       The ID of the player
 * @param bodypart       The body part slot (use CLOTHING_* constants)
 * @param customModelId  The custom model ID registered with AddClothingModel
 *
 * @return               1 on success, 0 on failure
 *
 * Example:
 *   SetPlayerCustomClothes(playerid, CLOTHING_TORSO, 30001);
 */
native SetPlayerCustomClothes(playerid, bodypart, customModelId);

/*
 * RemoveClothingModel
 *
 * Unregisters a previously added custom clothing model.
 *
 * @param customModelId  The custom model ID to remove
 *
 * @return               1 on success, 0 if not found
 */
native RemoveClothingModel(customModelId);

/*
 * IsClothingModelRegistered
 *
 * Checks if a custom clothing model ID has been registered.
 *
 * @param customModelId  The custom model ID to check
 *
 * @return               1 if registered, 0 if not
 */
native IsClothingModelRegistered(customModelId);

// ============================================================================
//  NATIVE FUNCTIONS — Synchronization
// ============================================================================

/*
 * SyncPlayerClothes
 *
 * Forces a re-sync of the player's clothing data to all nearby players.
 * Useful after making multiple clothing changes at once.
 *
 * @param playerid   The ID of the player to sync
 *
 * @return           1 on success, 0 on failure
 */
native SyncPlayerClothes(playerid);

/*
 * SyncPlayerClothesToPlayer
 *
 * Syncs a player's clothing data to a specific other player.
 * Useful when a player enters streaming range.
 *
 * @param playerid       The ID of the player whose clothes to sync
 * @param forplayerid    The ID of the player who should receive the sync
 *
 * @return               1 on success, 0 on failure
 */
native SyncPlayerClothesToPlayer(playerid, forplayerid);

// ============================================================================
//  NATIVE FUNCTIONS — Query / Utility
// ============================================================================

/*
 * IsPlayerWearingClothes
 *
 * Checks if a player has clothing equipped on a specific body part.
 *
 * @param playerid   The ID of the player
 * @param bodypart   The body part slot to check
 *
 * @return           1 if wearing clothes on that slot, 0 if empty
 */
native IsPlayerWearingClothes(playerid, bodypart);

/*
 * GetPlayerClothingCount
 *
 * Counts the number of body part slots that have clothing equipped.
 *
 * @param playerid   The ID of the player
 *
 * @return           Number of active clothing slots (0-18)
 */
native GetPlayerClothingCount(playerid);

/*
 * IsPlayerCJSkin
 *
 * Checks if the player is currently using the CJ skin (ID 0).
 * Clothing functions only work on CJ skin.
 *
 * @param playerid   The ID of the player
 *
 * @return           1 if using CJ skin, 0 if not
 */
native IsPlayerCJSkin(playerid);

/*
 * GetClothingPluginVersion
 *
 * Gets the clothing plugin version string.
 *
 * @param version[]  Buffer to store the version string
 * @param len        Buffer size
 *
 * @return           1 on success
 */
native GetClothingPluginVersion(version[], len = sizeof(version));

// ============================================================================
//  CALLBACKS
// ============================================================================

/*
 * OnPlayerClothesChange
 *
 * Called when a player's clothing changes on any body part.
 *
 * @param playerid   The ID of the player
 * @param bodypart   The body part that changed
 * @param texture[]  The new texture name (empty if removed)
 * @param model[]    The new model name (empty if removed)
 *
 * @return           This callback does not handle returns.
 *
 * Example:
 *   public OnPlayerClothesChange(playerid, bodypart, const texture[], const model[])
 *   {
 *       new name[MAX_PLAYER_NAME], str[256];
 *       GetPlayerName(playerid, name, sizeof(name));
 *       if (strlen(texture) > 0)
 *           format(str, sizeof(str), "%s changed clothing slot %d to %s/%s", name, bodypart, texture, model);
 *       else
 *           format(str, sizeof(str), "%s removed clothing from slot %d", name, bodypart);
 *       SendClientMessageToAll(0xFFFFFFFF, str);
 *       return 1;
 *   }
 */
forward OnPlayerClothesChange(playerid, bodypart, const texture[], const model[]);

/*
 * OnPlayerClothesLoaded
 *
 * Called when a player's clothing has been fully synced to their client.
 * Useful for knowing when clothing visuals are ready after a spawn.
 *
 * @param playerid   The ID of the player
 * @param slotCount  Number of clothing slots that were synced
 *
 * @return           This callback does not handle returns.
 */
forward OnPlayerClothesLoaded(playerid, slotCount);

// ============================================================================
//  STOCK HELPER FUNCTIONS
// ============================================================================

/*
 * GetClothingSlotName
 *
 * Returns the human-readable name of a clothing slot.
 */
stock GetClothingSlotName(bodypart, name[], len = sizeof(name))
{
    switch (bodypart)
    {
        case CLOTHING_TORSO:                    strcat((name[0] = EOS, name), "Torso", len);
        case CLOTHING_HEAD:                     strcat((name[0] = EOS, name), "Head", len);
        case CLOTHING_LEGS:                     strcat((name[0] = EOS, name), "Legs", len);
        case CLOTHING_SHOES:                    strcat((name[0] = EOS, name), "Shoes", len);
        case CLOTHING_TATTOO_LEFT_UPPER_ARM:    strcat((name[0] = EOS, name), "Tattoo Left Upper Arm", len);
        case CLOTHING_TATTOO_LEFT_LOWER_ARM:    strcat((name[0] = EOS, name), "Tattoo Left Lower Arm", len);
        case CLOTHING_TATTOO_RIGHT_UPPER_ARM:   strcat((name[0] = EOS, name), "Tattoo Right Upper Arm", len);
        case CLOTHING_TATTOO_RIGHT_LOWER_ARM:   strcat((name[0] = EOS, name), "Tattoo Right Lower Arm", len);
        case CLOTHING_TATTOO_BACK:              strcat((name[0] = EOS, name), "Tattoo Back", len);
        case CLOTHING_TATTOO_LEFT_CHEST:        strcat((name[0] = EOS, name), "Tattoo Left Chest", len);
        case CLOTHING_TATTOO_RIGHT_CHEST:       strcat((name[0] = EOS, name), "Tattoo Right Chest", len);
        case CLOTHING_TATTOO_STOMACH:           strcat((name[0] = EOS, name), "Tattoo Stomach", len);
        case CLOTHING_TATTOO_LOWER_BACK:        strcat((name[0] = EOS, name), "Tattoo Lower Back", len);
        case CLOTHING_NECKLACE:                 strcat((name[0] = EOS, name), "Necklace", len);
        case CLOTHING_WATCH:                    strcat((name[0] = EOS, name), "Watch", len);
        case CLOTHING_GLASSES:                  strcat((name[0] = EOS, name), "Glasses", len);
        case CLOTHING_HAT:                      strcat((name[0] = EOS, name), "Hat", len);
        case CLOTHING_EXTRA:                    strcat((name[0] = EOS, name), "Extra", len);
        default:                                strcat((name[0] = EOS, name), "Unknown", len);
    }
    return 1;
}

/*
 * SetPlayerFullOutfit
 *
 * Helper: sets multiple clothing parts at once.
 *
 * @param playerid   The ID of the player
 * @param torsoTex[] Torso texture (empty string "" to skip)
 * @param torsoMdl[] Torso model
 * @param legsTex[]  Legs texture (empty string "" to skip)
 * @param legsMdl[]  Legs model
 * @param shoesTex[] Shoes texture (empty string "" to skip)
 * @param shoesMdl[] Shoes model
 * @param hatTex[]   Hat texture (empty string "" to skip)
 * @param hatMdl[]   Hat model
 *
 * @return           Number of slots successfully set
 */
stock SetPlayerFullOutfit(playerid,
    const torsoTex[], const torsoMdl[],
    const legsTex[], const legsMdl[],
    const shoesTex[], const shoesMdl[],
    const hatTex[] = "", const hatMdl[] = "")
{
    new count = 0;

    if (strlen(torsoTex) > 0 && strlen(torsoMdl) > 0)
    {
        if (SetPlayerClothes(playerid, CLOTHING_TORSO, torsoTex, torsoMdl))
            count++;
    }

    if (strlen(legsTex) > 0 && strlen(legsMdl) > 0)
    {
        if (SetPlayerClothes(playerid, CLOTHING_LEGS, legsTex, legsMdl))
            count++;
    }

    if (strlen(shoesTex) > 0 && strlen(shoesMdl) > 0)
    {
        if (SetPlayerClothes(playerid, CLOTHING_SHOES, shoesTex, shoesMdl))
            count++;
    }

    if (strlen(hatTex) > 0 && strlen(hatMdl) > 0)
    {
        if (SetPlayerClothes(playerid, CLOTHING_HAT, hatTex, hatMdl))
            count++;
    }

    // Force sync after bulk update
    SyncPlayerClothes(playerid);
    return count;
}
