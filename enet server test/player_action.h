#pragma once

int getState(PlayerInfo* info) {
    int val = 0;
    val |= info->canWalkInBlocks << 0;
    val |= info->canDoubleJump << 1;
    val |= info->isInvisible << 2;
    val |= info->noHands << 3;
    val |= info->noEyes << 4;
    val |= info->noBody << 5;
    val |= info->devilHorns << 6;
    val |= info->goldenHalo << 7;
    val |= info->isFrozen << 11;
    val |= info->isCursed << 12;
    val |= info->isDuctaped << 13;
    val |= info->haveCigar << 14;
    val |= info->isShining << 15;
    val |= info->isZombie << 16;
    val |= info->isHitByLava << 17;
    val |= info->haveHauntedShadows << 18;
    val |= info->haveGeigerRadiation << 19;
    val |= info->haveReflector << 20;
    val |= info->isEgged << 21;
    val |= info->havePineappleFloag << 22;
    val |= info->haveFlyingPineapple << 23;
    val |= info->haveSuperSupporterName << 24;
    val |= info->haveSupperPineapple << 25;
    return val;
}

string PlayerDB::getProperName(string name) {
    string newS;
    for (char c : name) newS += (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
    string ret;
    for (int i = 0; i < newS.length(); i++) {
        if (newS[i] == '`') i++;
        else ret += newS[i];
    }
    string ret2;
    for (char c : ret)
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) ret2 += c;

    string username = ret2;
    if (username == "prn" || username == "con" || username == "aux" || username == "nul" || username == "com1" || username == "com2" || username == "com3" || username == "com4" || username == "com5" || username == "com6" || username == "com7" || username == "com8" || username == "com9" || username == "lpt1" || username == "lpt2" || username == "lpt3" || username == "lpt4" || username == "lpt5" || username == "lpt6" || username == "lpt7" || username == "lpt8" || username == "lpt9") {
        return "";
    }

    return ret2;
}

string PlayerDB::fixColors(string text) {
    string ret = "";
    int colorLevel = 0;
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '`') {
            ret += text[i];
            if (i + 1 < text.length())
                ret += text[i + 1];

            if (i + 1 < text.length() && text[i + 1] == '`') {
                colorLevel--;
            }
            else {
                colorLevel++;
            }
            i++;
        }
        else {
            ret += text[i];
        }
    }
    for (int i = 0; i < colorLevel; i++) {
        ret += "``";
    }
    for (int i = 0; i > colorLevel; i--) {
        ret += "`w";
    }
    return ret;
}

int PlayerDB::playerLogin(ENetPeer* peer, string username, string password) {
    std::ifstream ifs("players/" + PlayerDB::getProperName(username) + ".json");
    if (ifs.is_open()) {
        json j;
        ifs >> j;
        string pss = j["password"];
        int adminLevel = j["adminLevel"];
        if (password == pss) {
            ((PlayerInfo*)(peer->data))->hasLogon = true;
            //after verify password add adminlevel not before
            bool found = false;
            for (int i = 0; i < admins.size(); i++) {
                if (admins[i].username == username) {
                    found = true;
                }
            }
            if (!found) { //not in vector
                if (adminLevel != 0) {
                    Admin admin;
                    admin.username = PlayerDB::getProperName(username);
                    admin.password = pss;
                    admin.level = adminLevel;
                    admins.push_back(admin);
                }
            }
            ENetPeer* currentPeer;

            for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
                ++currentPeer) {
                if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
                    continue;
                if (currentPeer == peer)
                    continue;
                if (((PlayerInfo*)(currentPeer->data))->rawName == PlayerDB::getProperName(username)) {
                    {
                        gamepacket_t p;
                        p.Insert("OnConsoleMessage");
                        p.Insert("Someone else logged into this account!");
                        p.CreatePacket(peer);
                    } {
                        gamepacket_t p;
                        p.Insert("OnConsoleMessage");
                        p.Insert("Someone else was logged into this account! He was kicked out now.");
                        p.CreatePacket(peer);
                    }
                    enet_peer_disconnect_later(currentPeer, 0);
                }
            }
            return 1;
        }
        else {
            return -1;
        }
    }
    else {
        return -2;
    }
}

int PlayerDB::guildRegister(ENetPeer* peer, string guildName, string guildStatement, string guildFlagfg, string guildFlagbg) {
    if (guildName.find(" ") != string::npos || guildName.find(".") != string::npos || guildName.find(",") != string::npos || guildName.find("@") != string::npos || guildName.find("[") != string::npos || guildName.find("]") != string::npos || guildName.find("#") != string::npos || guildName.find("<") != string::npos || guildName.find(">") != string::npos || guildName.find(":") != string::npos || guildName.find("{") != string::npos || guildName.find("}") != string::npos || guildName.find("|") != string::npos || guildName.find("+") != string::npos || guildName.find("_") != string::npos || guildName.find("~") != string::npos || guildName.find("-") != string::npos || guildName.find("!") != string::npos || guildName.find("$") != string::npos || guildName.find("%") != string::npos || guildName.find("^") != string::npos || guildName.find("&") != string::npos || guildName.find("`") != string::npos || guildName.find("*") != string::npos || guildName.find("(") != string::npos || guildName.find(")") != string::npos || guildName.find("=") != string::npos || guildName.find("'") != string::npos || guildName.find(";") != string::npos || guildName.find("/") != string::npos) {
        return -1;
    }

    if (guildName.length() < 3) {
        return -2;
    }
    if (guildName.length() > 15) {
        return -3;
    }
    int fg;
    int bg;

    try {
        fg = stoi(guildFlagfg);
    }
    catch (std::invalid_argument& e) {
        return -6;
    }
    try {
        bg = stoi(guildFlagbg);
    }
    catch (std::invalid_argument& e) {
        return -5;
    }
    if (guildFlagbg.length() > 4) {
        return -7;
    }
    if (guildFlagfg.length() > 4) {
        return -8;
    }

    string fixedguildName = PlayerDB::getProperName(guildName);

    std::ifstream ifs("guilds/" + fixedguildName + ".json");
    if (ifs.is_open()) {
        return -4;
    }

    /*std::ofstream o("guilds/" + fixedguildName + ".json");
    if (!o.is_open()) {
      cout << GetLastError() << endl;
      _getch();
    }

    json j;

    //  Guild Detail
    j["GuildName"] = guildName;
    j["GuildStatement"] = guildStatement;
    j["GuildWorld"] = ((PlayerInfo*)(peer->data))->currentWorld;

    //  Guild Level
    j["GuildLevel"] = 0;
    j["GuildExp"] = 0;

    // Guild Leader
    j["Leader"] = ((PlayerInfo*)(peer->data))->rawName;


    // Guild Flag
    j["foregroundflag"] = 0;
    j["backgroundflag"] = 0;


    // Role
    vector<string>guildmember;
    vector<string>guildelder;
    vector<string>guildco;

    j["CoLeader"] = guildelder;
    j["ElderLeader"] = guildco;
    j["Member"] = guildmem;

    o << j << std::endl; */
    return 1;
}

int PlayerDB::playerRegister(ENetPeer* peer, string username, string password, string passwordverify, string email, string discord) {
    string name = username;
    if (name == "CON" || name == "PRN" || name == "AUX" || name == "NUL" || name == "COM1" || name == "COM2" || name == "COM3" || name == "COM4" || name == "COM5" || name == "COM6" || name == "COM7" || name == "COM8" || name == "COM9" || name == "LPT1" || name == "LPT2" || name == "LPT3" || name == "LPT4" || name == "LPT5" || name == "LPT6" || name == "LPT7" || name == "LPT8" || name == "LPT9") return -1;
    username = PlayerDB::getProperName(username);
    if (discord.find("#") == std::string::npos && discord.length() != 0) return -5;
    if (email.find("@") == std::string::npos && email.length() != 0) return -4;
    if (passwordverify != password) return -3;
    if (username.length() < 3) return -2;
    std::ifstream ifs("players/" + username + ".json");
    if (ifs.is_open()) {
        return -1;
    }

    std::ofstream o("players/" + username + ".json");
    if (!o.is_open()) {
        cout << GetLastError() << endl;
        _getch();
    }
    json j;
    j["username"] = username;
    j["password"] = password;
    j["email"] = email;
    j["discord"] = discord;
    j["adminLevel"] = 0;
    j["level"] = 1;
    j["xp"] = 0;
    j["ClothBack"] = 0;
    j["ClothHand"] = 0;
    j["ClothFace"] = 0;
    j["ClothShirt"] = 0;
    j["ClothPants"] = 0;
    j["ClothNeck"] = 0;
    j["ClothHair"] = 0;
    j["ClothFeet"] = 0;
    j["ClothMask"] = 0;
    j["ClothAnces"] = 0;
    j["guild"] = "";
    j["joinguild"] = false;
    j["premwl"] = 0;
    j["effect"] = 8421376;
    j["skinColor"] = 0x8295C3FF;
    j["worldsowned"] = ((PlayerInfo*)(peer->data))->createworldsowned;
    o << j << std::endl;

    std::ofstream oo("inventory/" + username + ".json");
    if (!oo.is_open()) {
        cout << GetLastError() << endl;
        _getch();
    }

    json items;
    json jjall = json::array();

    json jj;
    jj["aposition"] = 1;
    jj["itemid"] = 18;
    jj["quantity"] = 1;
    jjall.push_back(jj);

    jj["aposition"] = 2;
    jj["itemid"] = 32;
    jj["quantity"] = 1;
    jjall.push_back(jj);

    for (int i = 2; i < 200; i++) {
        jj["aposition"] = i + 1;
        jj["itemid"] = 0;
        jj["quantity"] = 0;
        jjall.push_back(jj);
    }

    items["items"] = jjall;
    oo << items << std::endl;

    return 1;
}

void savegem(ENetPeer* peer) {
    if (((PlayerInfo*)(peer->data))->haveGrowId == true) {
        PlayerInfo* p5 = ((PlayerInfo*)(peer->data));
        string username = PlayerDB::getProperName(p5->rawName);
        ifstream fg("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
        json j;
        fg >> j;
        fg.close();
        ofstream fs("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
        fs << j;
        fs.close();
    }
}
void sendConsole(ENetPeer* x, string e) {
    GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), e));
    ENetPacket* packet = enet_packet_create(p.data,
        p.len,
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(x, 0, packet);
    delete p.data;
}
void savejson(ENetPeer* peer) {
    if (((PlayerInfo*)(peer->data))->haveGrowId == true) {
        PlayerInfo* p5 = ((PlayerInfo*)(peer->data));
        string username = PlayerDB::getProperName(p5->rawName);
        ifstream fg("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
        json j;
        fg >> j;
        fg.close();
        j["premwl"] = ((PlayerInfo*)(peer->data))->premwl;
        j["level"] = ((PlayerInfo*)(peer->data))->level;
        j["xp"] = ((PlayerInfo*)(peer->data))->xp;
        ofstream fs("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
        fs << j;
        fs.close();
    }
}

bool CheckItemMaxed(ENetPeer* peer, int fItemId, int fQuantityAdd) {
    bool isMaxed = false;
    for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemId && ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount + fQuantityAdd > 200) {
            isMaxed = true;
            break;
        }
    }
    return isMaxed;
}

bool CheckItemExists(ENetPeer* peer,
    const int fItemId) {
    auto isExists = false;
    for (auto i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemId) {
            isExists = true;
            break;
        }
    }
    return isExists;
}

bool canSB(string username, string password) {
    for (int i = 0; i < admins.size(); i++) {
        Admin admin = admins[i];
        if (admin.username == username && admin.password == password && admin.level > 1) {
            using namespace std::chrono;
            if (admin.lastSB + 900000 < (duration_cast <milliseconds> (system_clock::now().time_since_epoch())).count() || admin.level == 999) {
                admins[i].lastSB = (duration_cast <milliseconds> (system_clock::now().time_since_epoch())).count();
                return true;
            }
            else {
                return false;
            }
        }
    }
    return false;
}

bool canClear(string username, string password) {
    for (int i = 0; i < admins.size(); i++) {
        Admin admin = admins[i];
        if (admin.username == username && admin.password == password) {
            return admin.level > 0;
        }
    }
    return false;
}

bool isSuperAdmin(string username, string password) {
    for (int i = 0; i < admins.size(); i++) {
        Admin admin = admins[i];
        if (admin.username == username && admin.password == password && admin.level == 999) {
            return true;
        }
    }
    return false;
}

bool isHere(ENetPeer* peer, ENetPeer* peer2) {
    return ((PlayerInfo*)(peer->data))->currentWorld == ((PlayerInfo*)(peer2->data))->currentWorld;
}

void sendInventory(ENetPeer* peer, PlayerInventory inventory) {
    string asdf2 = "0400000009A7379237BB2509E8E0EC04F8720B050000000000000000FBBB0000010000007D920100FDFDFDFD04000000040000000000000000000000000000000000";
    int inventoryLen = inventory.items.size();
    int packetLen = (asdf2.length() / 2) + (inventoryLen * 4) + 4;
    BYTE* data2 = new BYTE[packetLen];
    for (int i = 0; i < asdf2.length(); i += 2) {
        char x = ch2n(asdf2[i]);
        x = x << 4;
        x += ch2n(asdf2[i + 1]);
        memcpy(data2 + (i / 2), &x, 1);
    }
    int endianInvVal = _byteswap_ulong(inventoryLen);
    memcpy(data2 + (asdf2.length() / 2) - 4, &endianInvVal, 4);
    endianInvVal = _byteswap_ulong(((PlayerInfo*)(peer->data))->currentInventorySize);
    memcpy(data2 + (asdf2.length() / 2) - 8, &endianInvVal, 4);
    int val = 0;
    for (int i = 0; i < inventoryLen; i++) {
        val = 0;
        val |= inventory.items.at(i).itemID;
        val |= inventory.items.at(i).itemCount << 16;
        val &= 0x00FFFFFF;
        val |= 0x00 << 24;
        memcpy(data2 + (i * 4) + (asdf2.length() / 2), &val, 4);
    }
    ENetPacket* packet3 = enet_packet_create(data2,
        packetLen,
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet3);
    delete data2;
}

BYTE* packPlayerMoving(PlayerMoving* dataStruct) {
    BYTE* data = new BYTE[56];
    for (int i = 0; i < 56; i++) {
        data[i] = 0;
    }
    memcpy(data, &dataStruct->packetType, 4);
    memcpy(data + 4, &dataStruct->netID, 4);
    memcpy(data + 12, &dataStruct->characterState, 4);
    memcpy(data + 20, &dataStruct->plantingTree, 4);
    memcpy(data + 24, &dataStruct->x, 4);
    memcpy(data + 28, &dataStruct->y, 4);
    memcpy(data + 32, &dataStruct->XSpeed, 4);
    memcpy(data + 36, &dataStruct->YSpeed, 4);
    memcpy(data + 44, &dataStruct->punchX, 4);
    memcpy(data + 48, &dataStruct->punchY, 4);
    return data;
}

PlayerMoving* unpackPlayerMoving(BYTE* data) {
    PlayerMoving* dataStruct = new PlayerMoving;
    memcpy(&dataStruct->packetType, data, 4);
    memcpy(&dataStruct->netID, data + 4, 4);
    memcpy(&dataStruct->characterState, data + 12, 4);
    memcpy(&dataStruct->plantingTree, data + 20, 4);
    memcpy(&dataStruct->x, data + 24, 4);
    memcpy(&dataStruct->y, data + 28, 4);
    memcpy(&dataStruct->XSpeed, data + 32, 4);
    memcpy(&dataStruct->YSpeed, data + 36, 4);
    memcpy(&dataStruct->punchX, data + 44, 4);
    memcpy(&dataStruct->punchY, data + 48, 4);
    return dataStruct;
}

void sendState(ENetPeer* peer) {
    PlayerInfo* info = ((PlayerInfo*)(peer->data));
    int netID = info->netID;
    ENetPeer* currentPeer;
    int state = getState(info);
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer)) {
            PlayerMoving data;
            data.packetType = 0x14;
            data.characterState = 0; // animation
            data.x = 1000;
            data.y = 100;
            data.punchX = 0;
            data.punchY = 0;
            data.XSpeed = 300;
            data.YSpeed = 600;
            data.netID = netID;
            data.plantingTree = state;
            BYTE* raw = packPlayerMoving(&data);
            int
                var = info->peffect; // placing and breking
            memcpy(raw + 1, &
                var, 3);
            float waterspeed = 125.0f;
            memcpy(raw + 16, &waterspeed, 4);
            SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}

void updateAllClothes(ENetPeer* peer) {
    ENetPeer* currentPeer;
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer)) {
            GamePacket p3 = packetEnd(appendFloat(appendIntx(appendFloat(appendFloat(appendFloat(appendString(createPacket(), "OnSetClothing"), ((PlayerInfo*)(peer->data))->cloth_hair, ((PlayerInfo*)(peer->data))->cloth_shirt, ((PlayerInfo*)(peer->data))->cloth_pants), ((PlayerInfo*)(peer->data))->cloth_feet, ((PlayerInfo*)(peer->data))->cloth_face, ((PlayerInfo*)(peer->data))->cloth_hand), ((PlayerInfo*)(peer->data))->cloth_back, ((PlayerInfo*)(peer->data))->cloth_mask, ((PlayerInfo*)(peer->data))->cloth_necklace), ((PlayerInfo*)(peer->data))->skinColor), ((PlayerInfo*)(peer->data))->cloth_ances, 0.0f, 0.0f));
            memcpy(p3.data + 8, &(((PlayerInfo*)(peer->data))->netID), 4); // ffloor
            ENetPacket* packet3 = enet_packet_create(p3.data,
                p3.len,
                ENET_PACKET_FLAG_RELIABLE);

            enet_peer_send(currentPeer, 0, packet3);
            delete p3.data;
            //enet_host_flush(server);
            GamePacket p4 = packetEnd(appendFloat(appendIntx(appendFloat(appendFloat(appendFloat(appendString(createPacket(), "OnSetClothing"), ((PlayerInfo*)(currentPeer->data))->cloth_hair, ((PlayerInfo*)(currentPeer->data))->cloth_shirt, ((PlayerInfo*)(currentPeer->data))->cloth_pants), ((PlayerInfo*)(currentPeer->data))->cloth_feet, ((PlayerInfo*)(currentPeer->data))->cloth_face, ((PlayerInfo*)(currentPeer->data))->cloth_hand), ((PlayerInfo*)(currentPeer->data))->cloth_back, ((PlayerInfo*)(currentPeer->data))->cloth_mask, ((PlayerInfo*)(currentPeer->data))->cloth_necklace), ((PlayerInfo*)(currentPeer->data))->skinColor), ((PlayerInfo*)(currentPeer->data))->cloth_ances, 0.0f, 0.0f));
            memcpy(p4.data + 8, &(((PlayerInfo*)(currentPeer->data))->netID), 4); // ffloor
            ENetPacket* packet4 = enet_packet_create(p4.data,
                p4.len,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet4);
            delete p4.data;
            //enet_host_flush(server);
        }
    }
}

void sendClothes(ENetPeer* peer) {
    GamePacket p3 = packetEnd(appendFloat(appendIntx(appendFloat(appendFloat(appendFloat(appendString(createPacket(), "OnSetClothing"), ((PlayerInfo*)(peer->data))->cloth_hair, ((PlayerInfo*)(peer->data))->cloth_shirt, ((PlayerInfo*)(peer->data))->cloth_pants), ((PlayerInfo*)(peer->data))->cloth_feet, ((PlayerInfo*)(peer->data))->cloth_face, ((PlayerInfo*)(peer->data))->cloth_hand), ((PlayerInfo*)(peer->data))->cloth_back, ((PlayerInfo*)(peer->data))->cloth_mask, ((PlayerInfo*)(peer->data))->cloth_necklace), ((PlayerInfo*)(peer->data))->skinColor), ((PlayerInfo*)(peer->data))->cloth_ances, 0.0f, 0.0f));
    for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
        if (isHere(peer, currentPeer)) {
            string text = "action|play_sfx\nfile|audio/change_clothes.wav\ndelayMS|0\n";
            BYTE* data = new BYTE[5 + text.length()];
            BYTE zero = 0;
            int type = 3;
            memcpy(data, &type, 4);
            memcpy(data + 4, text.c_str(), text.length());
            memcpy(data + 4 + text.length(), &zero, 1);
            ENetPacket* packet2 = enet_packet_create(data,
                5 + text.length(),
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet2);
            delete[] data;
            memcpy(p3.data + 8, &(((PlayerInfo*)(peer->data))->netID), 4); // ffloor
            ENetPacket* packet3 = enet_packet_create(p3.data,
                p3.len,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet3);
        }
    }
    ifstream fg("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
    json j;
    fg >> j;
    fg.close();
    PlayerInfo* p = ((PlayerInfo*)(peer->data));
    string username = PlayerDB::getProperName(p->rawName);
    int wls = p->premwl;
    int clothback = p->cloth_back;
    int clothhand = p->cloth_hand;
    int clothface = p->cloth_face;
    int clothhair = p->cloth_hair;
    int clothfeet = p->cloth_feet;
    int clothpants = p->cloth_pants;
    int clothneck = p->cloth_necklace;
    int clothshirt = p->cloth_shirt;
    int clothmask = p->cloth_mask;
    int clothances = p->cloth_ances;
    j["ClothBack"] = clothback;
    j["ClothHand"] = clothhand;
    j["ClothFace"] = clothface;
    j["ClothShirt"] = clothshirt;
    j["ClothPants"] = clothpants;
    j["ClothNeck"] = clothneck;
    j["ClothHair"] = clothhair;
    j["ClothFeet"] = clothfeet;
    j["ClothMask"] = clothmask;
    j["ClothAnces"] = clothances;
    j["effect"] = p->peffect;
    j["premwl"] = p->premwl;
    j["worldsowned"] = ((PlayerInfo*)(peer->data))->worldsowned;
    ofstream fs("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
    fs << j;
    fs.close();
}

string packPlayerMoving2(PlayerMoving* dataStruct) {
    string data;
    data.resize(56);
    STRINT(data, 0) = dataStruct->packetType;
    STRINT(data, 4) = dataStruct->netID;
    STRINT(data, 12) = dataStruct->characterState;
    STRINT(data, 20) = dataStruct->plantingTree;
    STRINT(data, 24) = *(int*)&dataStruct->x;
    STRINT(data, 28) = *(int*)&dataStruct->y;
    STRINT(data, 32) = *(int*)&dataStruct->XSpeed;
    STRINT(data, 36) = *(int*)&dataStruct->YSpeed;
    STRINT(data, 44) = dataStruct->punchX;
    STRINT(data, 48) = dataStruct->punchY;
    return data;
}

void sendWho(ENetPeer* peer) {
    ENetPeer* currentPeer;
    string name = "";
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer)) {
            if (((PlayerInfo*)(currentPeer->data))->isGhost)
                continue;

            gamepacket_t p;
            p.Insert("OnTalkBubble");
            p.Insert(((PlayerInfo*)(currentPeer->data))->netID);
            p.Insert(((PlayerInfo*)(currentPeer->data))->displayName);
            p.Insert(1);
            p.CreatePacket(peer);
        }
    }
}

bool SaveConvertedItem(int fItemid, int fQuantity, ENetPeer* peer) {
    int invsizee = ((PlayerInfo*)(peer->data))->currentInventorySize;

    bool doesItemInInventryAlready = false;

    for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
        if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == fItemid) {
            doesItemInInventryAlready = true;
            if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 101) return false;
            break;
        }
    }

    std::ifstream iffff("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");

    json jj;

    if (iffff.fail()) {
        iffff.close();
        cout << "SaveConvertedItem funkcijoje (ifstream dalyje) error: itemid - " << fItemid << ", kiekis - " << fQuantity << endl;

    }
    if (iffff.is_open()) {

    }

    iffff >> jj; //load

    std::ofstream oo("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
    if (!oo.is_open()) {
        cout << GetLastError() << " SaveConvertedItem funkcijoje (ofstream dalyje) error: itemid - " << fItemid << ", kiekis - " << fQuantity << endl;
        _getch();
    }
    int howManyHasNow = 0;

    for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
        int itemidFromJson = jj["items"][i]["itemid"];
        int quantityFromJson = jj["items"][i]["quantity"];;
        if (doesItemInInventryAlready) {
            if (itemidFromJson == fItemid) {
                howManyHasNow = jj["items"][i]["quantity"];
                howManyHasNow += fQuantity;
                jj["items"][i]["quantity"] = howManyHasNow;

                for (int k = 0; k < ((PlayerInfo*)(peer->data))->inventory.items.size(); k++) {
                    if (((PlayerInfo*)(peer->data))->inventory.items[k].itemID == fItemid) {
                        ((PlayerInfo*)(peer->data))->inventory.items[k].itemCount += (byte)fQuantity;
                        break;
                    }
                }
                sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);

                break;
            }
        }
        else if (itemidFromJson == 0 && quantityFromJson == 0) {
            jj["items"][i]["quantity"] = fQuantity;
            jj["items"][i]["itemid"] = fItemid;

            InventoryItem item;
            item.itemID = fItemid;
            item.itemCount = fQuantity;
            ((PlayerInfo*)(peer->data))->inventory.items.push_back(item);

            sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);

            break;
        }
    }

    oo << jj << std::endl;
    return true;
}
void RemoveInventoryItem(int fItemid, int fQuantity, ENetPeer* peer) {
    std::ifstream iffff("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");

    json jj;

    if (iffff.fail()) {
        iffff.close();

    }
    if (iffff.is_open()) {

    }

    iffff >> jj; //load

    std::ofstream oo("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
    if (!oo.is_open()) {
        cout << GetLastError() << " Gak bisa delete " << fItemid << " bruh" << fQuantity << endl;
        _getch();
    }

    //jj["items"][aposition]["aposition"] = aposition;

    for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
        int itemid = jj["items"][i]["itemid"];
        int quantity = jj["items"][i]["quantity"];
        if (itemid == fItemid) {
            if (quantity - fQuantity == 0) {
                jj["items"][i]["itemid"] = 0;
                jj["items"][i]["quantity"] = 0;
            }
            else {
                jj["items"][i]["itemid"] = itemid;
                jj["items"][i]["quantity"] = quantity - fQuantity;
            }

            break;
        }

    }
    oo << jj << std::endl;

    for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemid) {
            if ((unsigned int)((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount > fQuantity && (unsigned int)((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount != fQuantity) {
                ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount -= fQuantity;
            }
            else {
                ((PlayerInfo*)(peer->data))->inventory.items.erase(((PlayerInfo*)(peer->data))->inventory.items.begin() + i);
            }
            sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);
        }
    }

}
void SearchInventoryItem(ENetPeer* peer, int fItemid, int fQuantity, bool& iscontains) {
    iscontains = false;
    for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemid && ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount >= fQuantity) {

            iscontains = true;
            break;
        }
    }
}
void SaveFindsItem(int fItemid, int fQuantity, ENetPeer* peer) {

    std::ifstream iffff("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");

    json jj;

    if (iffff.fail()) {
        iffff.close();

    }
    if (iffff.is_open()) {

    }

    iffff >> jj; //load

    std::ofstream oo("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
    if (!oo.is_open()) {
        cout << GetLastError() << endl;
        _getch();
    }

    //jj["items"][aposition]["aposition"] = aposition;

    for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
        int itemid = jj["items"][i]["itemid"];
        int quantity = jj["items"][i]["quantity"];
        if (itemid == 0 && quantity == 0) {
            jj["items"][i]["itemid"] = fItemid;
            jj["items"][i]["quantity"] = fQuantity;
            break;
        }

    }
    oo << jj << std::endl;

    InventoryItem item;
    item.itemID = fItemid;
    item.itemCount = fQuantity;
    ((PlayerInfo*)(peer->data))->inventory.items.push_back(item);

    sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);
}
void SaveShopsItemMoreTimes(int fItemid, int fQuantity, ENetPeer* peer, bool& success) {
    size_t invsizee = ((PlayerInfo*)(peer->data))->currentInventorySize;
    bool invfull = false;
    bool alreadyhave = false;

    if (((PlayerInfo*)(peer->data))->inventory.items.size() == invsizee) {

        GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "add_label_with_icon|big|`4Whoops!|left|1432|\nadd_spacer|small|\nadd_textbox|`oSorry! Your inventory is full! You can purchase an inventory upgrade in the shop.|\nadd_spacer|small|\nadd_button|close|`5Close|0|0|"));
        ENetPacket* packet = enet_packet_create(ps.data,
            ps.len,
            ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        delete ps.data;

        alreadyhave = true;
    }

    bool isFullStock = false;
    bool isInInv = false;
    for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {

        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemid && ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount >= 200) {

            GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "add_label_with_icon|big|`4Whoops!|left|1432|\nadd_spacer|small|\nadd_textbox|`oSorry! You already have full stock of this item!|\nadd_spacer|small|\nadd_button|close|`5Close|0|0|"));
            ENetPacket* packet = enet_packet_create(ps.data,
                ps.len,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            delete ps.data;

            isFullStock = true;
        }

        if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemid && ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount < 200) isInInv = true;

    }

    if (isFullStock == true || alreadyhave == true) {
        success = false;
    }
    else {
        success = true;

        std::ifstream iffff("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");

        json jj;

        if (iffff.fail()) {
            iffff.close();

        }
        if (iffff.is_open()) {

        }

        iffff >> jj; //load

        std::ofstream oo("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
        if (!oo.is_open()) {
            cout << GetLastError() << endl;
            _getch();
        }

        //jj["items"][aposition]["aposition"] = aposition;

        if (isInInv == false) {

            for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
                int itemid = jj["items"][i]["itemid"];
                int quantity = jj["items"][i]["quantity"];

                if (itemid == 0 && quantity == 0) {
                    jj["items"][i]["itemid"] = fItemid;
                    jj["items"][i]["quantity"] = fQuantity;
                    break;
                }

            }
            oo << jj << std::endl;

            InventoryItem item;
            item.itemID = fItemid;
            item.itemCount = fQuantity;
            ((PlayerInfo*)(peer->data))->inventory.items.push_back(item);

            sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);
        }
        else {
            for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
                int itemid = jj["items"][i]["itemid"];
                int quantity = jj["items"][i]["quantity"];

                if (itemid == fItemid) {
                    jj["items"][i]["quantity"] = quantity + fQuantity;
                    break;
                }

            }
            oo << jj << std::endl;

            for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
                if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == fItemid) {
                    ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount += fQuantity;
                    sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);
                }
            }

        }
    }
}

int getAdminLevel(string username, string password) {
    for (int i = 0; i < admins.size(); i++) {
        Admin admin = admins[i];
        if (admin.username == username && admin.password == password) {
            return admin.level;
        }
    }
    return 0;
}

void sendPuncheffectpeer(ENetPeer* peer, int punch) {
    PlayerInfo* info = ((PlayerInfo*)(peer->data));
    int netID = info->netID;
    int state = getState(info);
    PlayerMoving data;
    float water = 125.0f;
    data.packetType = 0x14;
    data.characterState = ((PlayerInfo*)(peer->data))->characterState; // animation
    data.x = 1000;
    if (((PlayerInfo*)(peer->data))->cloth_hand == 366) {
        data.y = -400;
    }
    else {
        data.y = 400;
    }
    data.punchX = -1;
    data.punchY = -1;
    data.XSpeed = 300;
    if (((PlayerInfo*)(peer->data))->cloth_back == 9472) {
        data.YSpeed = 600;
    }
    else {
        data.YSpeed = 1150;
    }
    data.netID = netID;
    data.plantingTree = state;
    BYTE* raw = packPlayerMoving(&data);
    int
        var = punch;
    memcpy(raw + 1, &
        var, 3);
    memcpy(raw + 16, &water, 4);
    SendPacketRaw(4, raw, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

void addItemToInventory(ENetPeer* peer, int id) {
    PlayerInventory inventory = ((PlayerInfo*)(peer->data))->inventory;
    for (int i = 0; i < inventory.items.size(); i++) {
        if (inventory.items[i].itemID == id && inventory.items[i].itemCount < 200) {
            inventory.items[i].itemCount++;
            return;
        }
    }
    if (inventory.items.size() >= inventory.inventorySize)
        return;
    InventoryItem item;
    item.itemCount = 1;
    item.itemID = id;
    inventory.items.push_back(item);
}
int getSharedUID(ENetPeer* peer, int itemNetID) {
    auto v = ((PlayerInfo*)(peer->data))->item_uids;
    for (auto t = v.begin(); t != v.end(); ++t) {
        if (t->actual_uid == itemNetID) {
            return t->shared_uid;
        }
    }
    return 0;
}
int checkForUIDMatch(ENetPeer* peer, int itemNetID) {
    auto v = ((PlayerInfo*)(peer->data))->item_uids;
    for (auto t = v.begin(); t != v.end(); ++t) {
        if (t->shared_uid == itemNetID) {
            return t->actual_uid;
        }
    }
    return 0;
}

void sendAction(ENetPeer* peer, int netID, string action) {
    ENetPeer* currentPeer;
    string name = "";
    gamepacket_t p(0, netID);
    p.Insert("OnAction");
    p.Insert(action);
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (isHere(peer, currentPeer)) {
            p.CreatePacket(currentPeer);
        }
    }
}