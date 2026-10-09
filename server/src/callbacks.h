#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

class Callbacks {
public:
    static void CallOnPlayerClothesChange(int playerid, int bodypart, const char* texture, const char* model);
    static void CallOnPlayerClothesLoaded(int playerid, int slotCount);
};

#endif // _CALLBACKS_H_
