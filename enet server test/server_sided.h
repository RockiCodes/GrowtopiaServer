#pragma once

void craftItemDescriptions() {
    int current = -1;
    std::ifstream infile("Descriptions.txt");
    for (std::string line; getline(infile, line);) {
        if (line.length() > 3 && line[0] != '/' && line[1] != '/') {
            vector < string > ex = explode("|", line);
            ItemDefinition def;
            if (atoi(ex[0].c_str()) + 1 < itemDefs.size()) {
                itemDefs.at(atoi(ex[0].c_str())).description = ex[1];
                if (!(atoi(ex[0].c_str()) % 2))
                    itemDefs.at(atoi(ex[0].c_str()) + 1).description = "This is a tree.";
            }
        }
    }
}

int itemdathash;
void buildItemsDatabase() {
    string secret = "PBG892FXX982ABC*";
    std::ifstream file("items.dat", std::ios::binary | std::ios::ate);
    int size = file.tellg();
    itemsDatSize = size;
    char* data = new char[size];
    file.seekg(0, std::ios::beg);

    if (file.read((char*)(data), size)) {
        itemsDat = new BYTE[60 + size];
        int MessageType = 0x4;
        int PacketType = 0x10;
        int NetID = -1;
        int CharState = 0x8;

        memset(itemsDat, 0, 60);
        memcpy(itemsDat, &MessageType, 4);
        memcpy(itemsDat + 4, &PacketType, 4);
        memcpy(itemsDat + 8, &NetID, 4);
        memcpy(itemsDat + 16, &CharState, 4);
        memcpy(itemsDat + 56, &size, 4);
        file.seekg(0, std::ios::beg);
        if (file.read((char*)(itemsDat + 60), size)) {
            uint8_t* pData;
            int size = 0;
            const char filename[] = "items.dat";
            size = filesize(filename);
            pData = getA((string)filename, &size, false, false);
            cout << "Updating items data success! Hash: " << HashString((unsigned char*)pData, size) << endl;
            itemdathash = HashString((unsigned char*)pData, size);
            file.close();
        }
    }
    else {
        cout << "Updating items data failed!" << endl;
        exit(0);
    }
    int itemCount;
    int memPos = 0;
    int16_t itemsdatVersion = 0;
    memcpy(&itemsdatVersion, data + memPos, 2);
    memPos += 2;
    memcpy(&itemCount, data + memPos, 4);
    memPos += 4;
    for (int i = 0; i < itemCount; i++) {
        ItemDefinition tile;

        {
            memcpy(&tile.id, data + memPos, 4);
            memPos += 4;
        } {
            tile.editableType = data[memPos];
            memPos += 1;
        } {
            tile.itemCategory = data[memPos];
            memPos += 1;
        } {
            tile.actionType = data[memPos];
            memPos += 1;
        } {
            tile.hitSoundType = data[memPos];
            memPos += 1;
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.name += data[memPos] ^ (secret[(j + tile.id) % secret.length()]);

                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.texture += data[memPos];
                memPos++;
            }
        }
        memcpy(&tile.textureHash, data + memPos, 4);
        memPos += 4;
        tile.itemKind = memPos[data];
        memPos += 1;
        memcpy(&tile.val1, data + memPos, 4);
        memPos += 4;
        tile.textureX = data[memPos];
        memPos += 1;
        tile.textureY = data[memPos];
        memPos += 1;
        tile.spreadType = data[memPos];
        memPos += 1;
        tile.isStripeyWallpaper = data[memPos];
        memPos += 1;
        tile.collisionType = data[memPos];
        memPos += 1;
        tile.breakHits = data[memPos] / 6;
        memPos += 1;
        memcpy(&tile.dropChance, data + memPos, 4);
        memPos += 4;
        tile.clothingType = data[memPos];
        memPos += 1;
        memcpy(&tile.rarity, data + memPos, 2);
        memPos += 2;
        tile.maxAmount = data[memPos];
        memPos += 1; {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.extraFile += data[memPos];
                memPos++;
            }
        }
        memcpy(&tile.extraFileHash, data + memPos, 4);
        memPos += 4;
        memcpy(&tile.audioVolume, data + memPos, 4);
        memPos += 4; {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.petName += data[memPos];
                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.petPrefix += data[memPos];
                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.petSuffix += data[memPos];
                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.petAbility += data[memPos];
                memPos++;
            }
        } {
            tile.seedBase = data[memPos];
            memPos += 1;
        } {
            tile.seedOverlay = data[memPos];
            memPos += 1;
        } {
            tile.treeBase = data[memPos];
            memPos += 1;
        } {
            tile.treeLeaves = data[memPos];
            memPos += 1;
        } {
            memcpy(&tile.seedColor, data + memPos, 4);
            memPos += 4;
        } {
            memcpy(&tile.seedOverlayColor, data + memPos, 4);
            memPos += 4;
        }
        memPos += 4; // deleted ingredients
        {
            memcpy(&tile.growTime, data + memPos, 4);
            memPos += 4;
        }
        memcpy(&tile.val2, data + memPos, 2);
        memPos += 2;
        memcpy(&tile.isRayman, data + memPos, 2);
        memPos += 2; {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.extraOptions += data[memPos];
                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.texture2 += data[memPos];
                memPos++;
            }
        } {
            int16_t strLen = *(int16_t*)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++) {
                tile.extraOptions2 += data[memPos];
                memPos++;
            }
        }
        memPos += 80;
        if (itemsdatVersion >= 11) {
            {
                int16_t strLen = *(int16_t*)&data[memPos];
                memPos += 2;
                for (int j = 0; j < strLen; j++) {
                    tile.punchOptions += data[memPos];
                    memPos++;
                }
            }
        }
        if (itemsdatVersion >= 12) memPos += 13;
        if (itemsdatVersion >= 13) memPos += 4;
        if (itemsdatVersion >= 14) memPos += 4;
        if (i != tile.id)
            cout << "Item are unordered!" << i << "/" << tile.id << endl;

        switch (tile.actionType) {
        case 0:
            tile.blockType = BlockTypes::FIST;
            break;
        case 1:
            // wrench tool
            break;
        case 2:
            tile.blockType = BlockTypes::DOOR;
            break;
        case 3:
            tile.blockType = BlockTypes::LOCK;
            break;
        case 4:
            tile.blockType = BlockTypes::GEM;
            break;
        case 8:
            tile.blockType = BlockTypes::CONSUMABLE;
            break;
        case 9:
            tile.blockType = BlockTypes::GATEWAY;
            break;
        case 10:
            tile.blockType = BlockTypes::SIGN;
            break;
        case 13:
            tile.blockType = BlockTypes::MAIN_DOOR;
            break;
        case 15:
            tile.blockType = BlockTypes::BEDROCK;
            break;
        case 17:
            tile.blockType = BlockTypes::FOREGROUND;
            break;
        case 18:
            tile.blockType = BlockTypes::BACKGROUND;
            break;
        case 19:
            tile.blockType = BlockTypes::SEED;
            break;
        case 20:
            tile.blockType = BlockTypes::CLOTHING;
            switch (tile.clothingType) {
            case 0:
                tile.clothType = ClothTypes::HAIR;
                break;
            case 1:
                tile.clothType = ClothTypes::SHIRT;
                break;
            case 2:
                tile.clothType = ClothTypes::PANTS;
                break;
            case 3:
                tile.clothType = ClothTypes::FEET;
                break;
            case 4:
                tile.clothType = ClothTypes::FACE;
                break;
            case 5:
                tile.clothType = ClothTypes::HAND;
                break;
            case 6:
                tile.clothType = ClothTypes::BACK;
                break;
            case 7:
                tile.clothType = ClothTypes::MASK;
                break;
            case 8:
                tile.clothType = ClothTypes::NECKLACE;
                break;

            }

            break;
        case 26: // portal
            tile.blockType = BlockTypes::DOOR;
            break;
        case 27:
            tile.blockType = BlockTypes::CHECKPOINT;
            break;
        case 28: // piano note
            tile.blockType = BlockTypes::BACKGROUND;
            break;
        case 41:
            tile.blockType = BlockTypes::WEATHER_MACHINE;
            break;
        case 34: // bulletin boardd
            tile.blockType = BlockTypes::BOARD;
            break;
        case 107: // ances
            tile.blockType = BlockTypes::CLOTHING;
            tile.clothType = ClothTypes::ANCES;
            break;
        default:
            break;

        }

        // -----------------
        itemDefs.push_back(tile);
    }
    craftItemDescriptions();
}

void showWrong(ENetPeer* peer, string listFull, string itemFind) {
    packet::dialog(peer, "add_label_with_icon|big|`wFind item: " + itemFind + "``|left|3802|\nadd_spacer|small|\n" + listFull + "add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n");
}
void PathFindingCore(ENetPeer* peer, int xs, int ys) {

    try {
        int Square = xs + (ys * 100); // 100 = Width World lu
        string gayname = ((PlayerInfo*)(peer->data))->rawName;
        WorldInfo* world = getPlyersWorld(peer);
        if (world->items[Square].PosFind == Square) {
            packet::consolemessage(peer, "`4[CHEAT DETECTED] " + gayname + " STOP CHEATING!");
            enet_peer_disconnect_later(peer, 0);
        }

    }
    catch (const std::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }
}

void addAdmin(string username, string password, int level) {
    Admin admin;
    admin.username = username;
    admin.password = password;
    admin.level = level;
    admins.push_back(admin);
}

int adminlevel(string name) {
    std::ifstream ifff("players/" + PlayerDB::getProperName(name) + ".json");
    json j;
    ifff >> j;

    int adminlevel;
    adminlevel = j["adminLevel"];

    ifff.close();
    if (adminlevel == 0) {
        return 0;
    }
    else {
        return adminlevel;
    }

}
int level(string name) {
    std::ifstream ifff("players/" + PlayerDB::getProperName(name) + ".json");
    json j;
    ifff >> j;

    int level;
    level = j["level"];

    ifff.close();
    return level;

}

string getRankText(string name) {
    int lvl = 0;
    lvl = adminlevel(name);
    if (lvl == 0) {
        return "`wPlayer";
    }
    if (lvl == 444) {
        return "`w[`1VIP`w]";
    }
    else if (lvl == 666) {
        return "`#Moderator";
    }
    else if (lvl == 777) {
        return "`4Administrator";
    }
    else if (lvl == 999) {
        return "`4CO-Creator";
    }
    else if (lvl == 1337) {
        return "`cServer-Creator";
    }
}
string getRankId(string name) {
    int lvl = 0;
    lvl = adminlevel(name);
    if (lvl == 0) {
        return "18";
    }
    if (lvl == 444) {
        return "274";
    }
    else if (lvl == 666) {
        return "278";
    }
    else if (lvl == 777) {
        return "276";
    }
    else if (lvl == 999) {
        return "1956";
    }
    else if (lvl == 1337) {
        return "2376";
    }
}
string getRankTexts(string name) {
    int lvl = 0;
    lvl = level(name);
    if (lvl <= 10) {
        return "`2Newbie";
    }
    if (lvl >= 11) {
        return "`1Advance";
    }
    if (lvl >= 50) {
        return "`cPro";
    }
    if (lvl >= 100) {
        return "`eMaster";
    }
    if (lvl >= 150) {
        return "`9Expert";
    }
    if (lvl >= 200) {
        return "`5A`4C`qE";
    }
}
string getRankIds(string name) {
    int lvl = 0;
    lvl = level(name);
    if (lvl <= 10) {
        return "3900";
    }
    if (lvl >= 11) {
        return "3192";
    }
    if (lvl >= 50) {
        return "7832";
    }
    if (lvl >= 100) {
        return "7586";
    }
    if (lvl >= 150) {
        return "6312";
    }
    if (lvl >= 200) {
        return "1956";
    }
}

void sendGazette(ENetPeer* peer) {
    std::ifstream news("news.txt");
    std::stringstream buffer;
    buffer << news.rdbuf();
    std::string newsString(buffer.str());
    packet::dialog(peer, newsString);
}

void removeAcc(ENetPeer* peer, string name) {
    string resetacc;
    std::ifstream ifff("players/" + PlayerDB::getProperName(name) + ".json");
    json j;
    ifff >> j;
}
void sendGrowmojis(ENetPeer* peer) {
    std::ifstream emoji("growmoji.txt");
    std::stringstream buffer;
    buffer << emoji.rdbuf();
    std::string newsString(buffer.str());
    packet::dialog(peer, newsString);
}

void onPeerConnect(ENetPeer* peer) {
    ENetPeer* currentPeer;

    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (peer != currentPeer) {
            if (isHere(peer, currentPeer)) {
                string netIdS = std::to_string(((PlayerInfo*)(currentPeer->data))->netID);
                packet::onspawn(peer, "spawn|avatar\nnetID|" + netIdS + "\nuserID|" + netIdS + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(((PlayerInfo*)(currentPeer->data))->x) + "|" + std::to_string(((PlayerInfo*)(currentPeer->data))->y) + "\nname|``" + ((PlayerInfo*)(currentPeer->data))->displayName + "``\ncountry|" + ((PlayerInfo*)(currentPeer->data))->country + "\ninvis|0\nmstate|0\nsmstate|0\n"); // ((PlayerInfo*)(server->peers[i].data))->tankIDName
                string netIdS2 = std::to_string(((PlayerInfo*)(peer->data))->netID);
                packet::onspawn(currentPeer, "spawn|avatar\nnetID|" + netIdS2 + "\nuserID|" + netIdS2 + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(((PlayerInfo*)(peer->data))->x) + "|" + std::to_string(((PlayerInfo*)(peer->data))->y) + "\nname|``" + ((PlayerInfo*)(peer->data))->displayName + "``\ncountry|" + ((PlayerInfo*)(peer->data))->country + "\ninvis|0\nmstate|0\nsmstate|0\n"); // ((PlayerInfo*)(server->peers[i].data))->tankIDName
            }
        }
    }

}

void sendEmoji(ENetPeer* peer, string emoji) {
    GamePacket p2ssw = packetEnd(appendString(appendInt(appendString(createPacket(), "OnEmoticonDataChanged"), 0), u8"(wl)|ā|1&(yes)|Ă|1&(no)|ă|1&(love)|Ą|1&(oops)|ą|1&(shy)|Ć|1&(wink)|ć|1&(tongue)|Ĉ|1&(agree)|ĉ|1&(sleep)|Ċ|1&(punch)|ċ|1&(music)|Č|1&(build)|č|1&(megaphone)|Ď|1&(sigh)|ď|1&(mad)|Đ|1&(wow)|đ|1&(dance)|Ē|1&(see-no-evil)|ē|1&(bheart)|Ĕ|1&(heart)|ĕ|1&(grow)|Ė|1&(gems)|ė|1&(kiss)|Ę|1&(gtoken)|ę|1&(lol)|Ě|1&(smile)|Ā|1&(cool)|Ĝ|1&(cry)|ĝ|1&(vend)|Ğ|1&(bunny)|ě|1&(cactus)|ğ|1&(pine)|Ĥ|1&(peace)|ģ|1&(terror)|ġ|1&(troll)|Ġ|1&(evil)|Ģ|1&(fireworks)|Ħ|1&(football)|ĥ|1&(alien)|ħ|1&(party)|Ĩ|1&(pizza)|ĩ|1&(clap)|Ī|1&(song)|ī|1&(ghost)|Ĭ|1&(nuke)|ĭ|1&(halo)|Į|1&(turkey)|į|1&(gift)|İ|1&(cake)|ı|1&(heartarrow)|Ĳ|1&(lucky)|ĳ|1&(shamrock)|Ĵ|1&(grin)|ĵ|1&(ill)|Ķ|1&"));
    ENetPacket* packet2ssw = enet_packet_create(p2ssw.data, p2ssw.len, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet2ssw);
    delete p2ssw.data;
}

void craftItemText() {
    int current = -1;
    std::ifstream infile("effect.txt");
    for (std::string line; getline(infile, line);) {
        if (line.length() > 5 && line[0] != '/' && line[1] != '/') {
            vector < string > ex = explode("|", line);
            ItemDefinition def;
            itemDefs.at(atoi(ex[0].c_str())).effect = ex[3] + " `$(`o" + ex[1] + " `omod removed)";
            itemDefs.at(atoi(ex[0].c_str())).effects = ex[2] + " `$(`o" + ex[1] + " `omod added)";
        }
    }
}
void getAutoEffect(ENetPeer* peer) {
    PlayerInfo* info = ((PlayerInfo*)(peer->data));
    if (info->cloth_hand == 5480) {
        info->peffect = 8421456;
        craftItemText();
    }
    else if (info->cloth_necklace == 11232) {
        info->peffect = 8421376 + 224;
        craftItemText();
    }
    else if (info->cloth_hand == 11440) {
        info->peffect = 8421376 + 111;
        craftItemText();
    }
    else if (info->cloth_hand == 5276) {
        info->peffect = 8421376 + 47;
        craftItemText();
    }
    else if (info->cloth_hair == 4746) {
        info->peffect = 8421376 + 75;
        craftItemText();
    }
    else if (info->cloth_hair == 4748) {
        info->peffect = 8421376 + 75;
        craftItemText();
    }
    else if (info->cloth_hair == 4750) {
        info->peffect = 8421376 + 75;
        craftItemText();
    }
    else if (info->cloth_hand == 10652) {
        info->peffect = 8421376 + 188;
        craftItemText();
    }
    else if (info->cloth_hand == 9716) {
        info->peffect = 8421529;
        craftItemText();
    }
    else if (info->cloth_shirt == 1780) {
        info->peffect = 8421396;
        craftItemText();
    }
    else if (info->cloth_shirt == 10426) {
        info->peffect = 8421559;
        craftItemText();
    }
    else if (info->cloth_face == 1204) {
        info->peffect = 8421386;
        craftItemText();
    }
    else if (info->cloth_face == 10128) {
        info->peffect = 8421376 + 683;
        craftItemText();
    }
    else if (info->cloth_feet == 10618) {
        info->peffect = 8421376 + 699;
        craftItemText();
    }
    else if (info->cloth_face == 138) {
        info->peffect = 8421377;
        craftItemText();
    }
    else if (info->cloth_face == 2476) {
        info->peffect = 8421415;
        craftItemText();
    }
    else if (info->cloth_hand == 366 || info->cloth_hand == 1464) {
        info->peffect = 8421378;
        craftItemText();
    }
    else if (info->cloth_hand == 472) {
        info->peffect = 8421379;
        craftItemText();
    }
    else if (info->cloth_hand == 7912) {
        info->peffect = 8421487;
        craftItemText();
    }
    else if (info->cloth_hand == 594) {
        info->peffect = 8421380;
        craftItemText();
    }
    else if (info->cloth_hand == 768) {
        info->peffect = 8421381;
        craftItemText();
    }
    else if (info->cloth_hand == 900) {
        info->peffect = 8421382;
        craftItemText();
    }
    else if (info->cloth_hand == 910) {
        info->peffect = 8421383;
        craftItemText();
    }
    else if (info->cloth_hand == 930) {
        info->peffect = 8421384;
        craftItemText();
    }
    else if (info->cloth_hand == 1016) {
        info->peffect = 8421385;
        craftItemText();
    }
    else if (info->cloth_hand == 1378) {
        info->peffect = 8421387;
        craftItemText();
    }
    else if (info->cloth_hand == 1484) {
        info->peffect = 8421389;
        craftItemText();
    }
    else if (info->cloth_hand == 1512) {
        info->peffect = 8421390;
        craftItemText();
    }
    else if (info->cloth_hand == 1542) {
        info->peffect = 8421391;
        craftItemText();
    }
    else if (info->cloth_hand == 1576) {
        info->peffect = 8421392;
        craftItemText();
    }
    else if (info->cloth_hand == 1676) {
        info->peffect = 8421393;
        craftItemText();
    }
    else if (info->cloth_hand == 1710) {
        info->peffect = 8421394;
        craftItemText();
    }
    else if (info->cloth_hand == 1748) {
        info->peffect = 8421395;
        craftItemText();
    }
    else if (info->cloth_hand == 1782) {
        info->peffect = 8421397;
        craftItemText();
    }
    else if (info->cloth_hand == 1804) {
        info->peffect = 8421398;
        craftItemText();
    }
    else if (info->cloth_hand == 1868) {
        info->peffect = 8421399;
        craftItemText();
    }
    else if (info->cloth_hand == 1874) {
        info->peffect = 8421400;
        craftItemText();
    }
    else if (info->cloth_hand == 1946) {
        info->peffect = 8421401;
        craftItemText();
    }
    else if (info->cloth_hand == 1948) {
        info->peffect = 8421402;
        craftItemText();
    }
    else if (info->cloth_hand == 1956) {
        info->peffect = 8421403;
        craftItemText();
    }
    else if (info->cloth_hand == 2908) {
        info->peffect = 8421405;
        craftItemText();
    }
    else if (info->cloth_hand == 2952) {
        info->peffect = 8421405;
        craftItemText();
    }
    else if (info->cloth_hand == 6312) {
        info->peffect = 8421405;
        craftItemText();
    }
    else if (info->cloth_hand == 1980) {
        info->peffect = 8421406;
        craftItemText();
    }
    else if (info->cloth_hand == 2066) {
        info->peffect = 8421407;
        craftItemText();
    }
    else if (info->cloth_hand == 2212) {
        info->peffect = 8421408;
        craftItemText();
    }
    else if (info->cloth_hand == 2218) {
        info->peffect = 8421409;
        craftItemText();
    }
    else if (info->cloth_feet == 2220) {
        info->peffect = 8421410;
        craftItemText();
    }
    else if (info->cloth_hand == 2266) {
        info->peffect = 8421411;
        craftItemText();
    }
    else if (info->cloth_hand == 2386) {
        info->peffect = 8421412;
        craftItemText();
    }
    else if (info->cloth_hand == 2388) {
        info->peffect = 8421413;
        craftItemText();
    }
    else if (info->cloth_hand == 2450) {
        info->peffect = 8421414;
        craftItemText();
    }
    else if (info->cloth_hand == 2512) {
        info->peffect = 8421417;
        craftItemText();
    }
    else if (info->cloth_hand == 2572) {
        info->peffect = 8421418;
        craftItemText();
    }
    else if (info->cloth_hand == 2592) {
        info->peffect = 8421419;
        craftItemText();
    }
    else if (info->cloth_hand == 2720) {
        info->peffect = 8421420;
        craftItemText();
    }
    else if (info->cloth_hand == 2752) {
        info->peffect = 8421421;
        craftItemText();
    }
    else if (info->cloth_hand == 2754) {
        info->peffect = 8421422;
        craftItemText();
    }
    else if (info->cloth_hand == 2756) {
        info->peffect = 8421423;
        craftItemText();
    }
    else if (info->cloth_hand == 2802) {
        info->peffect = 8421425;
        craftItemText();
    }
    else if (info->cloth_hand == 2866) {
        info->peffect = 8421426;
        craftItemText();
    }
    else if (info->cloth_hand == 2876) {
        info->peffect = 8421427;
        craftItemText();
    }
    else if (info->cloth_hand == 2886) {
        info->peffect = 8421430;
        craftItemText();
    }
    else if (info->cloth_hand == 2890) {
        info->peffect = 8421431;
        craftItemText();
    }
    else if (info->cloth_hand == 3066) {
        info->peffect = 8421433;
        craftItemText();
    }
    else if (info->cloth_hand == 3124) {
        info->peffect = 8421434;
        craftItemText();
    }
    else if (info->cloth_hand == 3168) {
        info->peffect = 8421435;
        craftItemText();
    }
    else if (info->cloth_hand == 3214) {
        info->peffect = 8421436;
        craftItemText();
    }
    else if (info->cloth_hand == 3300) {
        info->peffect = 8421440;
        craftItemText();
    }
    else if (info->cloth_hand == 3418) {
        info->peffect = 8421441;
        craftItemText();
    }
    else if (info->cloth_hand == 3476) {
        info->peffect = 8421442;
        craftItemText();
    }
    else if (info->cloth_hand == 3686) {
        info->peffect = 8421444;
        craftItemText();
    }
    else if (info->cloth_hand == 3716) {
        info->peffect = 8421445;
        craftItemText();
    }
    else if (info->cloth_hand == 4290) {
        info->peffect = 8421447;
        craftItemText();
    }
    else if (info->cloth_hand == 4474) {
        info->peffect = 8421448;
        craftItemText();
    }
    else if (info->cloth_hand == 4464) {
        info->peffect = 8421449;
        craftItemText();
    }
    else if (info->cloth_hand == 1576) {
        info->peffect = 8421450;
        craftItemText();
    }
    else if (info->cloth_hand == 4778 || info->cloth_hand == 6026) {
        info->peffect = 8421452;
        craftItemText();
    }
    else if (info->cloth_hand == 4996) {
        info->peffect = 8421453;
        craftItemText();
    }
    else if (info->cloth_hand == 4840) {
        info->peffect = 8421454;
        craftItemText();
    }
    else if (info->cloth_hand == 5480) {
        info->peffect = 8421456;
        craftItemText();
    }
    else if (info->cloth_hand == 6110) {
        info->peffect = 8421457;
        craftItemText();
    }
    else if (info->cloth_hand == 6308) {
        info->peffect = 8421458;
        craftItemText();
    }
    else if (info->cloth_hand == 6310) {
        info->peffect = 8421459;
        craftItemText();
    }
    else if (info->cloth_hand == 6298) {
        info->peffect = 8421460;
        craftItemText();
    }
    else if (info->cloth_hand == 6756) {
        info->peffect = 8421461;
        craftItemText();
    }
    else if (info->cloth_hand == 7044) {
        info->peffect = 8421462;
        craftItemText();
    }
    else if (info->cloth_shirt == 6892) {
        info->peffect = 8421463;
        craftItemText();
    }
    else if (info->cloth_hand == 7088) {
        info->peffect = 8421465;
        craftItemText();
    }
    else if (info->cloth_hand == 7098) {
        info->peffect = 8421466;
        craftItemText();
    }
    else if (info->cloth_shirt == 7192) {
        info->peffect = 8421467;
        craftItemText();
    }
    else if (info->cloth_shirt == 7136) {
        info->peffect = 8421468;
        craftItemText();
    }
    else if (info->cloth_mask == 7216) {
        info->peffect = 8421470;
        craftItemText();
    }
    else if (info->cloth_back == 7196) {
        info->peffect = 8421471;
        craftItemText();
    }
    else if (info->cloth_back == 7392) {
        info->peffect = 8421472;
        craftItemText();
    }
    else if (info->cloth_hand == 7488) {
        info->peffect = 8421479;
        craftItemText();
    }
    else if (info->cloth_hand == 7586) {
        info->peffect = 8421480;
        craftItemText();
    }
    else if (info->cloth_hand == 7650) {
        info->peffect = 8421481;
        craftItemText();
    }
    else if (info->cloth_feet == 7950) {
        info->peffect = 8421489;
        craftItemText();
    }
    else if (info->cloth_hand == 8036) {
        info->peffect = 8421494;
        craftItemText();
    }
    else if (info->cloth_hand == 8910) {
        info->peffect = 8421505;
        craftItemText();
    }
    else if (info->cloth_hand == 8942) {
        info->peffect = 8421506;
        craftItemText();
    }
    else if (info->cloth_hand == 8948) {
        info->peffect = 8421507;
        craftItemText();
    }
    else if (info->cloth_hand == 8946) {
        info->peffect = 8421509;
        craftItemText();
    }
    else if (info->cloth_back == 9006) {
        info->peffect = 8421511;
        craftItemText();
    }
    else if (info->cloth_hand == 9116 || info->cloth_hand == 9118 || info->cloth_hand == 9120 || info->cloth_hand == 9122) {
        info->peffect = 8421376 + 111;
        craftItemText();
    }
    else {
        info->peffect = 8421376;
        craftItemText();
    }
}

void sendPData(ENetPeer* peer, PlayerMoving* data) {
    ENetPeer* currentPeer;

    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount];
        ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
            continue;
        if (peer != currentPeer) {
            if (isHere(peer, currentPeer)) {
                data->netID = ((PlayerInfo*)(peer->data))->netID;

                SendPacketRaw(4, packPlayerMoving(data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }
}

void sendPuncheffect(ENetPeer* peer) {
    PlayerInfo* info = ((PlayerInfo*)(peer->data));
    int netID = info->netID;
    int state = getState(info);
    int pro = getState(info);
    int statey = 0;
    if (info->cloth_hand == 6028) statey = 1024;
    if (info->cloth_hand == 6262) statey = 8192;
    if (info->haveGrowId == false) statey = 50000;
    for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
        if (isHere(peer, currentPeer)) {
            PlayerMoving data;
            data.packetType = 0x14;
            data.characterState = statey;
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
                var = info->peffect;
            memcpy(raw + 1, &
                var, 3);
            SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
        }
    }
}
void sendNotification(ENetPeer* peer, string song, string flag, string message) {
    GamePacket p = packetEnd(appendInt(appendString(appendString(appendString(appendString(createPacket(), "OnAddNotification"), song), message), flag), 0));
    ENetPacket* packet = enet_packet_create(p.data,
        p.len,
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    delete p.data;
}

void loadnews() {
    std::ifstream ifs("news.txt");
    std::string content((std::istreambuf_iterator < char >(ifs)),
        (std::istreambuf_iterator < char >()));

    string target = "\r";
    string news = "";
    int found = -1;
    do {
        found = content.find(target, found + 1);
        if (found != -1) {
            news = content.substr(0, found) + content.substr(found + target.length());
        }
        else {
            news = content;
        }
    } while (found != -1);
    if (news != "") {
        newslist = news;
    }
}

void resetacc(ENetPeer* peer) {
    for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        string name2 = PlayerDB::getProperName(((PlayerInfo*)(currentPeer->data))->rawName);
        remove(("players/" + name2 + ".json").c_str());
        enet_peer_disconnect_later(currentPeer, 0);
    }
}

inline void sendWrongCmdLog(ENetPeer* peer) {
    packet::consolemessage(peer, "`4Unknown command. `oEnter `$/help `ofor a list of valid commands.");
}

void removeExtraSpaces(string& str) {
    int n = str.length();
    int i = 0, j = -1;
    bool spaceFound = false;
    while (++j < n && str[j] == ' ');

    while (j < n) {
        if (str[j] != ' ') {
            if ((str[j] == '.' || str[j] == ',' ||
                str[j] == '?') && i - 1 >= 0 &&
                str[i - 1] == ' ')
                str[i - 1] = str[j++];
            else
                str[i++] = str[j++];

            spaceFound = false;
        }
        else if (str[j++] == ' ') {
            if (!spaceFound) {
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }
    if (i <= 1)
        str.erase(str.begin() + i, str.end());
    else
        str.erase(str.begin() + i, str.end());
}

void sendChatMessage(ENetPeer* peer, int netID, string message) {
    if (message.length() == 0) return;
    for (char c : message)
        if (c < 0x18 || std::all_of(message.begin(), message.end(), isspace)) {
            return;
        }
    string name = "";
    ENetPeer* currentPeer;
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
        if (((PlayerInfo*)(currentPeer->data))->netID == netID)
            name = ((PlayerInfo*)(currentPeer->data))->displayName;
    }
    GamePacket p;
    GamePacket p2;
    if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `5" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`5" + message), 0));
    }
    else if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `c" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`c" + message), 0));
    }
    else if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `2" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`2" + message), 0));
    }
    else if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `^" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`^" + message), 0));
    }
    else if (((PlayerInfo*)(peer->data))->adminLevel == 444) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `1" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`1" + message), 0));
    }
    else if (((PlayerInfo*)(peer->data))->adminLevel == 111) {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> `9" + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), "`9" + message), 0));
    }
    else {
        p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "CP:_PL:0_OID:_CT:[W]_ `o<`w" + name + "`o> " + message));
        p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), netID), message), 0));
    }
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
        if (isHere(peer, currentPeer)) {
            ENetPacket* packet = enet_packet_create(p.data,
                p.len,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet);
            ENetPacket* packet2 = enet_packet_create(p2.data,
                p2.len,
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet2);
        }
    }
    delete p.data;
    delete p2.data;
}

void sendWorldOffers(ENetPeer* peer) {
    if (!((PlayerInfo*)(peer->data))->isIn) return;
    vector < WorldInfo > worlds = worldDB.getRandomWorlds();
    string worldOffers = "default|";
    if (worlds.size() > 0) {
        worldOffers += worlds[0].name;
    }

    worldOffers += "\nadd_button|Showing: `wWorlds``|_catselect_|0.6|3529161471|\n";
    for (int i = 0; i < worlds.size(); i++) {
        worldOffers += "add_floater|" + worlds[i].name + "|" + std::to_string(getPlayersCountInWorld(worlds[i].name)) + "|0.55|3529161471\n";
    }
    packet::requestworldselectmenu(peer, worldOffers);
}

void exitHandler(int s) {
    saveAllWorlds();
    exit(0);

}

void loadConfig() {
    std::ifstream ifs("config.json");
    if (ifs.is_open()) {
        json j;
        ifs >> j;
        ifs.close();
        try {
            configPort = j["port"].get < int >();
            configCDN = j["cdn"].get < string >();

            cout << "Config loaded." << endl;
        }
        catch (...) {
            cout << "Invalid Config, Fixing..." << endl;
            string config_contents = "{ \"port\": 17091, \"cdn\": \"0098/CDNContent77/cache/\" }";

            ofstream myfile1;
            myfile1.open("config.json");
            myfile1 << config_contents;
            myfile1.close();
            cout << "Config Has Been Fixed! Reloading..." << endl;
            std::ifstream ifs("config.json");
            json j;
            ifs >> j;
            ifs.close();
            configPort = j["port"].get < int >();
            configCDN = j["cdn"].get < string >();

            cout << "Config loaded." << endl;
        }
    }
    else {
        cout << "Config not found, Creating..." << endl;
        string config_contents = "{ \"port\": 17091, \"cdn\": \"0098/CDNContent77/cache/\" }";

        ofstream myfile1;
        myfile1.open("config.json");
        myfile1 << config_contents;
        myfile1.close();
        cout << "Config Has Been Created! Reloading..." << endl;
        std::ifstream ifs("config.json");
        json j;
        ifs >> j;
        ifs.close();
        configPort = j["port"].get < int >();
        configCDN = j["cdn"].get < string >();

        cout << "Config loaded." << endl;
    }
}

string randomDuctTapeMessage(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "f"
            "m";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}