# Custom CJ Clothing System for SA-MP

<div align="center">

**Version 1.0.0** | SA-MP 0.3.7 R2 | GTA SA 1.0 US

Sistem clothing CJ modular untuk SA-MP — memungkinkan server mengubah pakaian individual CJ (torso, celana, sepatu, topi, dll) secara per-bagian, tersinkronisasi ke semua player.

</div>

---

## Daftar Isi

- [Fitur](#fitur)
- [Arsitektur](#arsitektur)
- [Persyaratan](#persyaratan)
- [Struktur Project](#struktur-project)
- [Build & Compile](#build--compile)
- [Instalasi](#instalasi)
- [Penggunaan API](#penggunaan-api)
- [Daftar Clothing Slot](#daftar-clothing-slot)
- [Contoh Texture/Model Bawaan GTA](#contoh-texturemodel-bawaan-gta)
- [Custom Clothing Assets](#custom-clothing-assets)
- [Troubleshooting](#troubleshooting)

---

## Fitur

- ✅ **Set clothing per body part** — torso, celana, sepatu, topi, kacamata, kalung, dll (18 slot)
- ✅ **Built-in GTA SA clothing** — gunakan texture/model bawaan game
- ✅ **Custom clothing assets** — dukung DFF/TXD kustom buatan sendiri
- ✅ **Sinkronisasi multiplayer** — semua player melihat perubahan clothing
- ✅ **Pawn scripting API** — native functions lengkap dengan `.inc` include
- ✅ **Callback system** — `OnPlayerClothesChange`, `OnPlayerClothesLoaded`
- ✅ **Cross-platform server** — Windows (.dll) dan Linux (.so)
- ✅ **Auto-sync** — clothing otomatis di-sync saat player masuk streaming range

---

## Arsitektur

```
┌─────────────────────────┐
│    PAWN GAMEMODE (.pwn)  │
│    #include <clothing>   │
│    SetPlayerClothes(...)  │
└──────────┬──────────────┘
           │ AMX Natives
┌──────────▼──────────────┐
│  SERVER PLUGIN (.dll/.so)│
│  - Clothing data storage │
│  - RPC broadcasting      │
│  - Asset management      │
└──────────┬──────────────┘
           │ Custom RPC (200-204)
┌──────────▼──────────────┐
│  CLIENT PLUGIN (.asi)    │
│  - RPC handler           │
│  - Memory hooks          │
│  - CPedClothesDesc calls │
│  - Visual model rebuild  │
└─────────────────────────┘
```

---

## Persyaratan

### Build
- **CMake** 3.15+
- **Visual Studio 2019/2022** dengan C++ Desktop workload (untuk Windows)
- **GCC/G++** dengan 32-bit support (untuk Linux: `gcc-multilib g++-multilib`)

### Runtime
- **SA-MP Server** 0.3.7 R2
- **GTA San Andreas** 1.0 US (versi standar SA-MP)
- **ASI Loader** (Silent's ASI Loader) — sudah terinstall di client

---

## Struktur Project

```
CUSTOM CLOTHING CJ SAMP/
├── GTA SAMP/                          # Output: client-side ASI plugin
│   └── clothing.asi                   # ← auto-copied saat build
│
├── PLUGIN SOURCE CODE/                # Source code utama
│   ├── CMakeLists.txt                 # Root CMake
│   ├── build.bat                      # Build script (Windows)
│   ├── build.sh                       # Build script (Linux)
│   │
│   ├── shared/
│   │   └── clothing_common.h          # Definisi bersama (RPC, struct)
│   │
│   ├── sdk/                           # SA-MP Plugin SDK headers
│   │   ├── plugincommon.h
│   │   └── amx/amx.h
│   │
│   ├── server/                        # Server-side plugin
│   │   ├── CMakeLists.txt
│   │   └── src/
│   │       ├── main.cpp               # Entry point (Supports, Load, etc.)
│   │       ├── plugin.h               # Global declarations
│   │       ├── natives.h/.cpp         # AMX native implementations
│   │       ├── clothing_manager.h/.cpp # Data management
│   │       ├── network.h/.cpp         # RakServer RPC sending
│   │       └── callbacks.h/.cpp       # Pawn callback execution
│   │
│   ├── client/                        # Client-side ASI plugin
│   │   ├── CMakeLists.txt
│   │   └── src/
│   │       ├── main.cpp               # DllMain + init thread
│   │       ├── game_sa.h              # GTA SA internal structures
│   │       ├── samp_structs.h         # SA-MP 0.3.7 R2 structures
│   │       ├── hooks.h/.cpp           # Memory hooks + pattern scan
│   │       ├── clothing_client.h/.cpp # Clothing application logic
│   │       └── rpc_handler.h/.cpp     # Custom RPC handlers
│   │
│   ├── include/
│   │   └── clothing.inc              # Pawn scripting include
│   │
│   ├── example/
│   │   └── clothing_test.pwn         # Example filterscript
│   │
│   └── output/                        # Build outputs
│       ├── server/
│       │   ├── plugins/clothing.dll
│       │   └── include/clothing.inc
│       └── client/
│           └── clothing.asi
│
└── SA-MP SERVER/                      # Test server folder
    ├── plugins/
    │   └── clothing.dll               # ← auto-copied saat build
    └── pawno/include/
        └── clothing.inc               # ← auto-copied saat build
```

---

## Build & Compile

### Windows (Server + Client)

```batch
cd "PLUGIN SOURCE CODE"
build.bat
```

Atau manual dengan CMake:

```batch
cd "PLUGIN SOURCE CODE"
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

### Linux (Server only)

```bash
cd "PLUGIN SOURCE CODE"
chmod +x build.sh
./build.sh
```

> **Note:** Client ASI plugin hanya bisa di-build di Windows.

---

## Instalasi

### Server

1. Copy `clothing.dll` (atau `clothing.so`) ke folder `plugins/` server
2. Copy `clothing.inc` ke folder `pawno/include/`
3. Edit `server.cfg`:
   ```
   plugins clothing.dll
   ```
   Linux:
   ```
   plugins clothing.so
   ```

### Client

1. Pastikan **ASI Loader** sudah terinstall di GTA SA
2. Copy `clothing.asi` ke folder root GTA San Andreas (sejajar dengan `gta_sa.exe`)
3. Selesai — plugin akan aktif otomatis saat game dimulai

---

## Penggunaan API

### Basic — Set Clothing Bawaan GTA

```pawn
#include <a_samp>
#include <clothing>

public OnPlayerSpawn(playerid)
{
    // Pastikan skin CJ
    SetPlayerSkin(playerid, 0);
    
    // Set pakaian per bagian
    SetPlayerClothes(playerid, CLOTHING_TORSO, "vestblack", "vest");
    SetPlayerClothes(playerid, CLOTHING_LEGS, "jeansdenim", "jeans");
    SetPlayerClothes(playerid, CLOTHING_SHOES, "sneakerbincblk", "sneaker");
    SetPlayerClothes(playerid, CLOTHING_HAT, "capknitgrn", "capknit");
    SetPlayerClothes(playerid, CLOTHING_GLASSES, "groucho", "groucho");
    return 1;
}
```

### Advanced — Custom Clothing Assets

```pawn
public OnGameModeInit()
{
    // Register custom models (letakkan .dff/.txd di server/models/clothing/)
    AddClothingModel(0, 30001, "my_tshirt.dff", "my_tshirt.txd");
    AddClothingModel(0, 30002, "my_jeans.dff", "my_jeans.txd");
    return 1;
}

public OnPlayerSpawn(playerid)
{
    SetPlayerSkin(playerid, 0);
    SetPlayerCustomClothes(playerid, CLOTHING_TORSO, 30001);
    SetPlayerCustomClothes(playerid, CLOTHING_LEGS, 30002);
    return 1;
}
```

### Query & Remove

```pawn
// Cek apakah player memakai topi
if (IsPlayerWearingClothes(playerid, CLOTHING_HAT))
{
    // Hapus topi
    RemovePlayerClothes(playerid, CLOTHING_HAT);
}

// Hapus semua pakaian
RemoveAllPlayerClothes(playerid);

// Hitung jumlah item yang dipakai
new count = GetPlayerClothingCount(playerid);
printf("Player memakai %d item", count);
```

---

## Daftar Clothing Slot

| ID | Konstanta | Deskripsi |
|:--|:--|:--|
| 0 | `CLOTHING_TORSO` | Baju/kaos/jaket |
| 1 | `CLOTHING_HEAD` | Gaya rambut |
| 2 | `CLOTHING_LEGS` | Celana |
| 3 | `CLOTHING_SHOES` | Sepatu |
| 4 | `CLOTHING_TATTOO_LEFT_UPPER_ARM` | Tato lengan kiri atas |
| 5 | `CLOTHING_TATTOO_LEFT_LOWER_ARM` | Tato lengan kiri bawah |
| 6 | `CLOTHING_TATTOO_RIGHT_UPPER_ARM` | Tato lengan kanan atas |
| 7 | `CLOTHING_TATTOO_RIGHT_LOWER_ARM` | Tato lengan kanan bawah |
| 8 | `CLOTHING_TATTOO_BACK` | Tato punggung |
| 9 | `CLOTHING_TATTOO_LEFT_CHEST` | Tato dada kiri |
| 10 | `CLOTHING_TATTOO_RIGHT_CHEST` | Tato dada kanan |
| 11 | `CLOTHING_TATTOO_STOMACH` | Tato perut |
| 12 | `CLOTHING_TATTOO_LOWER_BACK` | Tato punggung bawah |
| 13 | `CLOTHING_NECKLACE` | Kalung |
| 14 | `CLOTHING_WATCH` | Jam tangan/gelang |
| 15 | `CLOTHING_GLASSES` | Kacamata |
| 16 | `CLOTHING_HAT` | Topi |
| 17 | `CLOTHING_EXTRA` | Pakaian khusus |

---

## Contoh Texture/Model Bawaan GTA

### Torso (Slot 0)
| Texture | Model | Deskripsi |
|:--|:--|:--|
| `vestblack` | `vest` | Vest hitam |
| `tshirtlocgrey` | `tshirt` | Kaos abu |
| `shirtdress` | `shirtdress` | Kemeja formal |
| `hockeytop` | `hockey` | Jersey hockey |
| `sleevlesshoop` | `sleevless` | Tanktop basketball |
| `bandana` | `bandana` | Bandana |

### Legs (Slot 2)
| Texture | Model | Deskripsi |
|:--|:--|:--|
| `jeansdenim` | `jeans` | Jeans |
| `chinosblack` | `chinos` | Chinos hitam |
| `tracktr` | `tracktr` | Training pants |
| `shorts` | `shorts` | Celana pendek |
| `cargoblue` | `cargo` | Cargo biru |

### Shoes (Slot 3)
| Texture | Model | Deskripsi |
|:--|:--|:--|
| `sneakerbincblk` | `sneaker` | Sneaker hitam |
| `baskball` | `baskball` | Basketball shoes |
| `shoedressblk` | `shoedress` | Sepatu formal |
| `flipflop` | `flipflop` | Sandal |
| `cowboyboot2` | `cowboy` | Cowboy boots |

### Hat (Slot 16)
| Texture | Model | Deskripsi |
|:--|:--|:--|
| `capknitgrn` | `capknit` | Topi rajut hijau |
| `cowboy` | `cowboy` | Topi koboi |
| `helmet` | `helmet` | Helm |
| `bikerhelmet` | `bikerhelmet` | Helm biker |

---

## Custom Clothing Assets

Untuk membuat custom clothing:

1. **Buat model 3D (.dff)** menggunakan 3ds Max / Blender dengan rigging CJ skeleton
2. **Buat texture (.txd)** menggunakan TXD Workshop
3. Letakkan file di server folder: `models/clothing/`
4. Register di script:
   ```pawn
   AddClothingModel(baseid, newid, "file.dff", "file.txd");
   ```
5. Gunakan di script:
   ```pawn
   SetPlayerCustomClothes(playerid, CLOTHING_TORSO, newid);
   ```

### Tips Membuat Custom Clothing
- Model harus di-rig ke CJ skeleton (bukan skeleton lain)
- Buat 3 variasi: normal, fat, ripped (opsional, minimal normal)
- Texture resolution sebaiknya 256x256 atau 512x512
- Format TXD harus DXT1 (opaque) atau DXT3/DXT5 (transparent)

---

## Troubleshooting

### Server plugin tidak terload
- Pastikan `clothing.dll` ada di folder `plugins/`
- Pastikan `server.cfg` mengandung `plugins clothing.dll`
- Cek server log untuk error messages

### Clothing tidak terlihat di client
- Pastikan `clothing.asi` terinstall di root GTA SA
- Pastikan ASI Loader aktif (cek ada `vorbisFile.dll` atau `d3d9.dll` dari ASI Loader)
- Cek `clothing_client.log` di folder GTA SA untuk error

### Clothing hanya terlihat di player sendiri
- Pastikan server plugin dan client plugin versinya sama
- Pastikan `SyncPlayerClothes()` dipanggil setelah set clothing

### Game crash saat ganti baju
- Pastikan hanya menggunakan skin CJ (ID 0)
- Pastikan nama texture/model valid dan ada di game files
- Cek `clothing_client.log` untuk crash info

---

## License

This project is open source for educational and community use.

## Credits

- **SA-MP Plugin SDK** — maddinat0r
- **GTA SA Plugin-SDK** — DK22Pac  
- **GTA SA Reversed** — Community project
- **SA-MP Team** — SA-MP 0.3.7 R2
