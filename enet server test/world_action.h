#pragma once

WorldInfo generateWorld(string name, int width, int height)
{
	WorldInfo world;
	world.name = name;
	world.width = width;
	world.height = height;
	world.items = new WorldItem[world.width * world.height];
	for (int i = 0; i < world.width * world.height; i++)
	{
		if (i >= 3800 && i < 5400 && !(rand() % 50)) { world.items[i].foreground = 10; }
		else if (i >= 3700 && i < 5400) {
			if (i > 5000) {
				if (i % 7 == 0) { world.items[i].foreground = 4; }
				else { world.items[i].foreground = 2; }
			}
			else { world.items[i].foreground = 2; }
		}
		else if (i >= 5400) { world.items[i].foreground = 8; }
		if (i >= 3700)
			world.items[i].background = 14;
		if (i == 3650)
			world.items[i].foreground = 6;
		else if (i >= 3600 && i < 3700)
			world.items[i].foreground = 0; //fixed the grass in the world!
		if (i == 3750)
			world.items[i].foreground = 8;
	}
	return world;
}

bool isWorldOwner(ENetPeer* peer, WorldInfo* world) {
	return ((PlayerInfo*)(peer->data))->rawName == world->owner;
}
bool isWorldAdmin(ENetPeer* peer, WorldInfo* world) {
	const auto uid = ((PlayerInfo*)(peer->data))->rawName;
	for (const auto i = 0; world->acclist.size();) {
		const auto x = world->acclist.at(i);
		if (uid == x.substr(0, x.find("|"))) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

AWorld WorldDB::get2(string name) {
	if (worlds.size() > 200) {
		saveRedundant();
	}
	AWorld ret;
	name = getStrUpper(name);
	if (name.length() < 1) throw 1; // too short name
	for (char c : name) {
		if ((c < 'A' || c>'Z') && (c < '0' || c>'9'))
			throw 2; // wrong name
	}
	if (name == "EXIT") {
		throw 3;
	}
	if (name == "CON" || name == "PRN" || name == "AUX" || name == "NUL" || name == "COM1" || name == "COM2" || name == "COM3" || name == "COM4" || name == "COM5" || name == "COM6" || name == "COM7" || name == "COM8" || name == "COM9" || name == "LPT1" || name == "LPT2" || name == "LPT3" || name == "LPT4" || name == "LPT5" || name == "LPT6" || name == "LPT7" || name == "LPT8" || name == "LPT9") throw 3;
	for (int i = 0; i < worlds.size(); i++) {
		if (worlds.at(i).name == name)
		{
			ret.id = i;
			ret.info = worlds.at(i);
			ret.ptr = &worlds.at(i);
			return ret;
		}

	}
	std::ifstream ifs("worlds/" + name + ".json");
	if (ifs.is_open()) {

		json j;
		ifs >> j;
		WorldInfo info;
		info.name = j["name"].get<string>();
		info.width = j["width"];
		info.height = j["height"];
		info.owner = j["owner"].get<string>();
		info.weather = j["weather"].get<int>();
		info.isPublic = j["isPublic"];
		for (int i = 0; i < j["access"].size(); i++) {
			info.acclist.push_back(j["access"][i]);
		}
		info.stuffID = j["stuffID"].get<int>();
		info.stuff_gravity = j["stuff_gravity"].get<int>();
		info.stuff_invert = j["stuff_invert"].get<bool>();
		info.stuff_spin = j["stuff_spin"].get<bool>();
		json tiles = j["tiles"];
		int square = info.width * info.height;
		info.items = new WorldItem[square];
		for (int i = 0; i < square; i++) {
			info.items[i].foreground = tiles[i]["fg"];
			info.items[i].background = tiles[i]["bg"];
		}
		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds.at(worlds.size() - 1);
		return ret;
	}
	else {
		WorldInfo info = generateWorld(name, 100, 60);

		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds.at(worlds.size() - 1);
		return ret;
	}
	throw 1;
}

WorldInfo WorldDB::get(string name) {

	return this->get2(name).info;
}

int WorldDB::getworldStatus(string name) {
	name = getStrUpper(name);
	if (name.length() > 24) return -1;
	return 0;
}

void WorldDB::flush(WorldInfo info)
{
	std::ofstream o("worlds/" + info.name + ".json");
	if (!o.is_open()) {
		cout << GetLastError() << endl;
	}
	json j;
	j["name"] = info.name;
	j["width"] = info.width;
	j["height"] = info.height;
	j["owner"] = info.owner;
	j["isPublic"] = info.isPublic;
	j["weather"] = info.weather;
	j["stuffID"] = info.stuffID;
	j["stuff_gravity"] = info.stuff_gravity;
	j["stuff_invert"] = info.stuff_invert;
	j["stuff_spin"] = info.stuff_spin;
	j["access"] = info.acclist;
	json tiles = json::array();
	int square = info.width * info.height;

	for (int i = 0; i < square; i++)
	{
		json tile;
		tile["fg"] = info.items[i].foreground;
		tile["bg"] = info.items[i].background;
		tiles.push_back(tile);
	}
	j["tiles"] = tiles;
	o << j << std::endl;
}

void WorldDB::flush2(AWorld info)
{
	this->flush(info.info);
}

void WorldDB::save(AWorld info)
{
	flush2(info);
	delete info.info.items;
	worlds.erase(worlds.begin() + info.id);
}

void WorldDB::saveAll()
{
	for (int i = 0; i < worlds.size(); i++) {
		flush(worlds.at(i));
		delete[] worlds.at(i).items;
	}
	worlds.clear();
}

vector<WorldInfo> WorldDB::getRandomWorlds() {
	vector<WorldInfo> ret;
	for (int i = 0; i < ((worlds.size() < 10) ? worlds.size() : 10); i++)
	{ // load first four worlds, it is excepted that they are special
		ret.push_back(worlds.at(i));
	}
	// and lets get up to 6 random
	if (worlds.size() > 4) {
		for (int j = 0; j < 6; j++)
		{
			bool isPossible = true;
			WorldInfo world = worlds.at(rand() % (worlds.size() - 4));
			for (int i = 0; i < ret.size(); i++)
			{
				if (world.name == ret.at(i).name || world.name == "EXIT")
				{
					isPossible = false;
				}
			}
			if (isPossible)
				ret.push_back(world);
		}
	}
	return ret;
}

void WorldDB::saveRedundant()
{
	for (int i = 4; i < worlds.size(); i++) {
		bool canBeFree = true;
		ENetPeer* currentPeer;

		for (currentPeer = server->peers;
			currentPeer < &server->peers[server->peerCount];
			++currentPeer)
		{
			if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
				continue;
			if (((PlayerInfo*)(currentPeer->data))->currentWorld == worlds.at(i).name)
				canBeFree = false;
		}
		if (canBeFree)
		{
			flush(worlds.at(i));
			delete worlds.at(i).items;
			worlds.erase(worlds.begin() + i);
			i--;
		}
	}
}

WorldDB worldDB;

void saveAllWorlds() // atexit hack plz fix
{
	GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"),
		"`4Global system message`o: Saving all worlds `oin `p5 `wseconds`o, you will be timed out for a short amount of time`w! `oDon't punch anything or you may get disconnected!``"));
	ENetPacket* packet0 = enet_packet_create(p0.data,
		p0.len,
		ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet0);
	cout << "[!] Saving worlds..." << endl;
	worldDB.saveAll();
	cout << "[!] Worlds saved!" << endl;
	Sleep(1000);

	Sleep(200);
	GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`5 `4Global system message`o: `2Saved `oall worlds``"));
	ENetPacket* packet = enet_packet_create(p.data,
		p.len,
		ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
	delete p0.data;
	delete p.data;
}

void autosave()
{
	bool exist = std::experimental::filesystem::exists("save.txt");
	if (!exist)
	{
		ofstream save("save.txt");
		save << 0;
		save.close();
	}
	std::ifstream ok("save.txt");
	std::string limits((std::istreambuf_iterator<char>(ok)),
		(std::istreambuf_iterator<char>()));
	int a = atoi(limits.c_str());
	if (a == 0)
	{
		ofstream ok;
		ok.open("save.txt");
		ok << 50;
		ok.close();
		worldDB.saveAll();
		cout << "[!]Auto Saving Worlds" << endl;
	}
	else
	{
		int aa = a - 1;
		ofstream ss;
		ss.open("save.txt");
		ss << aa;
		ss.close();
		if (aa == 0)
		{
			ofstream ok;
			ok.open("save.txt");
			ok << 50;
			ok.close();
			worldDB.saveAll();
			cout << "[!]Auto Saving Worlds" << endl;
		}
	}
}

WorldInfo* getPlyersWorld(ENetPeer* peer)
{
	try {
		return worldDB.get2(((PlayerInfo*)(peer->data))->currentWorld).ptr;
	}
	catch (int e) {
		return NULL;
	}
}

BYTE* packBlockVisual222(TileExtra* dataStruct)
{

	BYTE* data = new BYTE[104]; // 96
	for (int i = 0; i < 100; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 8, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 16, &dataStruct->objectSpeedX, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	memcpy(data + 52, &dataStruct->charStat, 4);
	memcpy(data + 56, &dataStruct->blockid, 2);
	memcpy(data + 58, &dataStruct->backgroundid, 2);
	memcpy(data + 60, &dataStruct->visual, 4);
	memcpy(data + 64, &dataStruct->displayblock, 4);


	return data;
}
BYTE* packStuffVisual(TileExtra* dataStruct, int options, int gravity)
{
	BYTE* data = new BYTE[102];
	for (int i = 0; i < 102; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 8, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	memcpy(data + 52, &dataStruct->charStat, 4);
	memcpy(data + 56, &dataStruct->blockid, 2);
	memcpy(data + 58, &dataStruct->backgroundid, 2);
	memcpy(data + 60, &dataStruct->visual, 4);
	memcpy(data + 64, &dataStruct->displayblock, 4);
	memcpy(data + 68, &gravity, 4);
	memcpy(data + 70, &options, 4);

	return data;
}

void UpdateBlockState(ENetPeer* peer, int x, int y, bool forEveryone, WorldInfo* worldInfo) {

	if (!worldInfo) return;

	int i = y * worldInfo->width + x;

	int blockStateFlags = 0;


	if (worldInfo->items[i].flipped)
		blockStateFlags |= 0x00200000;
	if (worldInfo->items[i].water)
		blockStateFlags |= 0x04000000;
	if (worldInfo->items[i].glue)
		blockStateFlags |= 0x08000000;
	if (worldInfo->items[i].fire)
		blockStateFlags |= 0x10000000;
	if (worldInfo->items[i].red)
		blockStateFlags |= 0x20000000;
	if (worldInfo->items[i].green)
		blockStateFlags |= 0x40000000;
	if (worldInfo->items[i].blue)
		blockStateFlags |= 0x80000000;
	if (worldInfo->items[i].active)
		blockStateFlags |= 0x00400000;
	if (worldInfo->items[i].silenced)
		blockStateFlags |= 0x02400000;
}

void PlayAudioWorld(ENetPeer* peer, string audioFile) {
	string text = "action|play_sfx\nfile|" + audioFile + "\ndelayMS|0\n";
	BYTE* data = new BYTE[5 + text.length()];
	BYTE zero = 0;
	int type = 3;
	memcpy(data, &type, 4);
	memcpy(data + 4, text.c_str(), text.length());
	memcpy(data + 4 + text.length(), &zero, 1);
	for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
		if (isHere(peer, currentPeer)) {
			ENetPacket* packet2 = enet_packet_create(data,
				5 + text.length(),
				ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(currentPeer, 0, packet2);
		}
	}
	delete[] data;
}

void OnSetCurrentWeather(ENetPeer* peer, int weather) {
	GamePacket p2 = packetEnd(appendInt(appendString(createPacket(), "OnSetCurrentWeather"), weather));
	ENetPacket* packet2 = enet_packet_create(p2.data,
		p2.len,
		ENET_PACKET_FLAG_RELIABLE);
	for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
		if (isHere(peer, currentPeer)) {
			enet_peer_send(currentPeer, 0, packet2);
		}
	}
	delete p2.data;
}
inline void autoSaveWorlds()
{
	while (true)
	{
		Sleep(300000);
		ENetPeer* currentPeer;
		serverIsFrozen = true;
		for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer)
		{
			if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
		}
		GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"),
			"`2Saving all worlds."));
		ENetPacket* packet0 = enet_packet_create(p0.data,
			p0.len,
			ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, packet0);
		saveAllWorlds();

	}
	serverIsFrozen = false;

}

BYTE* packBlockVisual(BlockVisual* dataStruct)
{
	BYTE* data = new BYTE[72];
	for (int i = 0; i < 72; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 8, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	memcpy(data + 52, &dataStruct->charStat, 4);
	memcpy(data + 56, &dataStruct->blockid, 4);
	memcpy(data + 60, &dataStruct->visual, 4);
	memcpy(data + 64, &dataStruct->displayblock, 4);


	return data;
}

void SendDropSingle(ENetPeer* peer, int netID, int x, int y, int item, int count, BYTE specialEffect)
{
	if (item >= maxItems) return;
	if (item < 0) return;

	PlayerMoving data;
	data.packetType = 14;
	data.x = x;
	data.y = y;
	data.netID = netID;
	data.plantingTree = item;
	float val = count; // item count
	BYTE val2 = specialEffect;

	BYTE* raw = packPlayerMoving(&data);
	memcpy(raw + 16, &val, 4);
	memcpy(raw + 1, &val2, 1);

	SendPacketRaw(4, raw, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}
void updateDoor(ENetPeer* peer, int foreground, int x, int y, string text)
{
	PlayerMoving sign;
	sign.packetType = 0x3;
	sign.characterState = 0x0;
	sign.x = x;
	sign.y = y;
	sign.punchX = x;
	sign.punchY = y;
	sign.XSpeed = 0;
	sign.YSpeed = 0;
	sign.netID = -1;
	sign.plantingTree = foreground;
	SendPacketRaw(4, packPlayerMoving(&sign), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
	int hmm = 8;
	int text_len = text.length();
	int lol = 0;
	int wut = 5;
	int yeh = hmm + 3 + 1;
	int idk = 15 + text_len;
	int is_locked = 0;
	int bubble_type = 1;
	int ok = 52 + idk;
	int kek = ok + 4;
	int yup = ok - 8 - idk;
	int four = 4;
	int magic = 56;
	int wew = ok + 5 + 4;
	int wow = magic + 4 + 5;

	BYTE* data = new BYTE[kek];
	ENetPacket* p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
	for (int i = 0; i < kek; i++) data[i] = 0;
	memcpy(data, &wut, four); //4
	memcpy(data + yeh, &hmm, four); //8
	memcpy(data + yup, &x, 4); //12
	memcpy(data + yup + 4, &y, 4); //16
	memcpy(data + 4 + yup + 4, &idk, four); //20
	memcpy(data + magic, &foreground, 2); //22
	memcpy(data + four + magic, &lol, four); //26
	memcpy(data + magic + 4 + four, &bubble_type, 1); //27
	memcpy(data + wow, &text_len, 2); //data + wow = text_len, pos 29
	memcpy(data + 2 + wow, text.c_str(), text_len); //data + text_len_len + text_len_offs = text, pos 94
	memcpy(data + ok, &is_locked, four); //98
	memcpy(p->data, &four, four); //4
	memcpy((char*)p->data + four, data, kek); //kek = data_len
	ENetPeer* currentPeer;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer)) {
			enet_peer_send(currentPeer, 0, p);
		}
	}
	delete data;
}
void updateEntrance(ENetPeer* peer, int foreground, int x, int y, bool open, int bg) {
	BYTE* data = new BYTE[69];// memset(data, 0, 69);
	for (int i = 0; i < 69; i++) data[i] = 0;
	int four = 4; int five = 5; int eight = 8;
	int huhed = (65536 * bg) + foreground; int loled = 128;

	memcpy(data, &four, 4);
	memcpy(data + 4, &five, 4);
	memcpy(data + 16, &eight, 4);
	memcpy(data + 48, &x, 4);
	memcpy(data + 52, &y, 4);
	memcpy(data + 56, &eight, 4);
	memcpy(data + 60, &foreground, 4);
	memcpy(data + 62, &bg, 4);

	if (open) {
		int state = 0;
		memcpy(data + 66, &loled, 4);
		memcpy(data + 68, &state, 4);
	}
	else {
		int state = 100;
		int yeetus = 25600;
		memcpy(data + 67, &yeetus, 5);
		memcpy(data + 68, &state, 4);
	}
	ENetPacket* p = enet_packet_create(data, 69, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, p);
}
void updateSign(ENetPeer* peer, int foreground, int x, int y, string text, int background)
{
	PlayerMoving sign;
	sign.packetType = 0x3;
	sign.characterState = 0x0;
	sign.x = x;
	sign.y = y;
	sign.punchX = x;
	sign.punchY = y;
	sign.XSpeed = 0;
	sign.YSpeed = 0;
	sign.netID = -1;
	sign.plantingTree = foreground;
	SendPacketRaw(4, packPlayerMoving(&sign), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
	int hmm = 8, wot = text.length(), lol = 0, wut = 5;
	int yeh = hmm + 3 + 1, idk = 15 + wot, lmao = -1, yey = 2; //idk = text_len + 15, wut = type(?), wot = text_len, yey = len of text_len
	int ok = 52 + idk;
	int kek = ok + 4, yup = ok - 8 - idk;
	int thonk = 4, magic = 56, wew = ok + 5 + 4;
	int wow = magic + 4 + 5;
	BYTE* data = new BYTE[kek];
	ENetPacket* p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
	for (int i = 0; i < kek; i++) data[i] = 0;
	memcpy(data, &wut, thonk);
	memcpy(data + yeh, &hmm, thonk); //read discord
	memcpy(data + yup, &x, 4);
	memcpy(data + yup + 4, &y, 4);
	memcpy(data + 4 + yup + 4, &idk, thonk);
	memcpy(data + magic, &foreground, yey);
	memcpy(data + magic + 2, &background, yey); //p100 fix by the one and only lapada
	memcpy(data + thonk + magic, &lol, thonk);
	memcpy(data + magic + 4 + thonk, &yey, 1);
	memcpy(data + wow, &wot, yey); //data + wow = text_len
	memcpy(data + yey + wow, text.c_str(), wot); //data + text_len_len + text_len_offs = text
	memcpy(data + ok, &lmao, thonk); //end ?
	memcpy(p->data, &thonk, thonk);
	memcpy((char*)p->data + thonk, data, kek); //kek = data_len
	ENetPeer* currentPeer;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer)) {
			enet_peer_send(currentPeer, 0, p);
		}
	}
	delete data;
}
void updateSignSound(ENetPeer* peer, int foreground, int x, int y, string text, int background)
{
	int hmm = 8, wot = text.length(), lol = 0, wut = 5;
	int yeh = hmm + 3 + 1, idk = 15 + wot, lmao = -1, yey = 2; //idk = text_len + 15, wut = type(?), wot = text_len, yey = len of text_len
	int ok = 52 + idk;
	int kek = ok + 4, yup = ok - 8 - idk;
	int thonk = 4, magic = 56, wew = ok + 5 + 4;
	int wow = magic + 4 + 5;
	BYTE* data = new BYTE[kek];
	ENetPacket* p = enet_packet_create(0, wew, ENET_PACKET_FLAG_RELIABLE);
	for (int i = 0; i < kek; i++) data[i] = 0;
	memcpy(data, &wut, thonk);
	memcpy(data + yeh, &hmm, thonk); //read discord
	memcpy(data + yup, &x, 4);
	memcpy(data + yup + 4, &y, 4);
	memcpy(data + 4 + yup + 4, &idk, thonk);
	memcpy(data + magic, &foreground, yey);
	memcpy(data + magic + 2, &background, yey); //p100 fix by the one and only lapada
	memcpy(data + thonk + magic, &lol, thonk);
	memcpy(data + magic + 4 + thonk, &yey, 1);
	memcpy(data + wow, &wot, yey); //data + wow = text_len
	memcpy(data + yey + wow, text.c_str(), wot); //data + text_len_len + text_len_offs = text
	memcpy(data + ok, &lmao, thonk); //end ?
	memcpy(p->data, &thonk, thonk);
	memcpy((char*)p->data + thonk, data, kek); //kek = data_len
	ENetPeer* currentPeer;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer)) {
			enet_peer_send(currentPeer, 0, p);
		}
	}
	delete data;
}
void sendStuffweather(ENetPeer* peer, int x, int y, uint16_t itemid, uint16_t gravity, bool spin, bool invert) {
	PlayerMoving pmov;
	pmov.packetType = 5;
	pmov.characterState = 8;
	pmov.punchX = x;
	pmov.punchY = y;
	pmov.netID = -1;
	uint8_t* pmovpacked = packPlayerMoving(&pmov);
	*(uint32_t*)(pmovpacked + 52) = 10 + 8;
	uint8_t* packet = new uint8_t[4 + 56 + 10 + 8];
	memset(packet, 0, 4 + 56 + 10 + 8);
	packet[0] = 4;
	memcpy(packet + 4, pmovpacked, 56);
	*(uint16_t*)(packet + 4 + 56) = 3832; // WEATHER MACHINE - STUFF
	*(uint16_t*)(packet + 4 + 56 + 6) = 1;
	*(uint8_t*)(packet + 4 + 56 + 8) = 0x31;
	*(uint32_t*)(packet + 4 + 56 + 9) = itemid;
	*(uint32_t*)(packet + 4 + 56 + 13) = gravity;
	*(uint8_t*)(packet + 4 + 56 + 17) = spin | (invert << 1);
	ENetPacket* epacket = enet_packet_create(packet, 4 + 56 + 8 + 10, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, epacket);
	delete pmovpacked;
	delete[] packet;
}
void sendBackground(ENetPeer* peer, int x, int y, uint16_t itemid) {
	PlayerMoving pmov;
	pmov.packetType = 5;
	pmov.characterState = 8;
	pmov.punchX = x;
	pmov.punchY = y;
	pmov.netID = -1;
	uint8_t* pmovpacked = packPlayerMoving(&pmov);
	*(uint32_t*)(pmovpacked + 52) = 5 + 8;
	uint8_t* packet = new uint8_t[4 + 56 + 5 + 8];
	memset(packet, 0, 4 + 56 + 5 + 8);
	packet[0] = 4;
	memcpy(packet + 4, pmovpacked, 56);
	*(uint16_t*)(packet + 4 + 56) = 5000; // WEATHER MACHINE - BACKGROUND
	*(uint16_t*)(packet + 4 + 56 + 6) = 1;
	*(uint8_t*)(packet + 4 + 56 + 8) = 0x28;
	*(uint16_t*)(packet + 4 + 56 + 9) = itemid;
	ENetPacket* epacket = enet_packet_create(packet, 4 + 56 + 8 + 5, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, epacket);
	delete pmovpacked;
	delete[] packet;
}

int getPlayersCountInWorld(string name)
{
	int count = 0;
	ENetPeer* currentPeer;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (((PlayerInfo*)(currentPeer->data))->currentWorld == name)
			count++;
	}
	return count;
}

void sendRoulete(ENetPeer* peer, int x, int y)
{
	ENetPeer* currentPeer;
	int val = rand() % 37;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer))
		{
			gamepacket_t p(2500);
			p.Insert("OnTalkBubble");
			p.Insert(((PlayerInfo*)(peer->data))->netID);
			p.Insert("`w[" + ((PlayerInfo*)(peer->data))->displayName + " `wspun the wheel and got `4" + std::to_string(val) + "`w!]");
			p.Insert(0);
			p.CreatePacket(currentPeer);
		}
	}
}

void sendTileData(ENetPeer* peer, int x, int y, int visual, uint16_t fgblock, uint16_t bgblock, string tiledata) {
	PlayerMoving pmov;
	pmov.packetType = 5;
	pmov.characterState = 0;
	pmov.x = 0;
	pmov.y = 0;
	pmov.XSpeed = 0;
	pmov.YSpeed = 0;
	pmov.plantingTree = 0;
	pmov.punchX = x;
	pmov.punchY = y;
	pmov.netID = 0;

	string packetstr;
	packetstr.resize(4);
	packetstr[0] = 4;
	packetstr += packPlayerMoving2(&pmov);
	packetstr[16] = 8;
	packetstr.resize(packetstr.size() + 4);
	STRINT(packetstr, 52 + 4) = tiledata.size() + 4;
	STR16(packetstr, 56 + 4) = fgblock;
	STR16(packetstr, 58 + 4) = bgblock;
	packetstr += tiledata;

	ENetPacket* packet = enet_packet_create(&packetstr[0],
		packetstr.length(),
		ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, 0, packet);
}

ItemDefinition getItemDef(int id)
{
	if (id < itemDefs.size() && id > -1)
		return itemDefs.at(id);
	throw 0;
	return itemDefs.at(0);
}

string lockTileDatas(int visual, uint32_t owner, uint32_t adminLength, uint32_t* admins, bool isPublic = false, uint8_t bpm = 0) {
	string data;
	data.resize(4 + 2 + 4 + 4 + adminLength * 4 + 8);
	if (bpm) data.resize(data.length() + 4);
	data[2] = 0x01;
	if (isPublic) data[2] |= 0x80;
	data[4] = 3;
	data[5] = visual; // or 0x02
	STRINT(data, 6) = owner;
	//data[14] = 1;
	STRINT(data, 10) = adminLength;
	for (uint32_t i = 0; i < adminLength; i++) {
		STRINT(data, 14 + i * 4) = admins[i];
	}

	if (bpm) {
		STRINT(data, 10)++;
		STRINT(data, 14 + adminLength * 4) = -bpm;
	}
	return data;
}

void sendNothingHappened(ENetPeer* peer, int x, int y) {
	PlayerMoving data;
	data.netID = ((PlayerInfo*)(peer->data))->netID;
	data.packetType = 0x8;
	data.plantingTree = 0;
	data.netID = -1;
	data.x = x;
	data.y = y;
	data.punchX = x;
	data.punchY = y;
	SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

void sendTileUpdate(int x, int y, int tile, int causedBy, ENetPeer* peer)
{
	if (tile > itemDefs.size()) {
		return;
	}
	bool isLock = false;
	PlayerMoving data;
	data.packetType = 0x3;
	data.characterState = 0x0; // animation
	data.x = x;
	data.y = y;
	data.punchX = x;
	data.punchY = y;
	data.XSpeed = 0;
	data.YSpeed = 0;
	data.netID = causedBy;
	data.plantingTree = tile;

	WorldInfo* world = getPlyersWorld(peer);

	string gay = world->items[x + (y * world->width)].text;
	int gay2 = world->items[x + (y * world->width)].foreground;
	if (getItemDef(world->items[x + (y * world->width)].foreground).blockType == BlockTypes::SIGN || world->items[x + (y * world->width)].foreground == 1420 || world->items[x + (y * world->width)].foreground == 6214)
	{
		if (world->owner != "") {
			if (((PlayerInfo*)(peer->data))->rawName == world->owner) {
				if (tile == 32) {
					((PlayerInfo*)(peer->data))->wrenchx = x;
					((PlayerInfo*)(peer->data))->wrenchy = y;
					GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "``|left|" + to_string(gay2) + "|\n\nadd_textbox|`oWhat would you like to write on this sign?|\nadd_text_input|ch3||" + gay + "|100|\nembed_data|tilex|" + std::to_string(((PlayerInfo*)(peer->data))->wrenchx) + "\nembed_data|tiley|" + std::to_string(((PlayerInfo*)(peer->data))->wrenchy) + "\nend_dialog|sign_edit|Cancel|OK|"));
					ENetPacket* packet = enet_packet_create(p.data,
						p.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, packet);
					delete p.data;
				}
			}
		}
	}
	if (getItemDef(world->items[x + (y * world->width)].foreground).blockType == BlockTypes::GATEWAY) {
		if (tile == 32) {
			if (((PlayerInfo*)(peer->data))->rawName == world->owner) {
				((PlayerInfo*)(peer->data))->wrenchx = x;
				((PlayerInfo*)(peer->data))->wrenchy = y;
				int id = getItemDef(world->items[x + (y * world->width)].foreground).id;
				if (world->items[x + (y * world->width)].isOpened == false) {
					packet::dialog(peer, "add_label_with_icon|big|Edit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "|left|" + to_string(id) + "|\nadd_checkbox|opentopublic|`ois Open to Public?|0|\nend_dialog|entrance|Cancel|OK|");
				}
				else {
					packet::dialog(peer, "add_label_with_icon|big|Edit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "|left|" + to_string(id) + "|\nadd_checkbox|opentopublic|`ois Open to Public?|1|\nend_dialog|entrance|Cancel|OK|");
				}
			}
		}
	}
	if (getItemDef(tile).blockType == BlockTypes::CONSUMABLE) {
		if (tile == 11398) {
			for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer->data == NULL) continue;
				if (isHere(peer, currentPeer)) {
					if (x == ((PlayerInfo*)(peer->data))->x / 32 && y == ((PlayerInfo*)(peer->data))->y / 32) {
						RemoveInventoryItem(11398, 1, peer);
						vector<int> alien_pod{ 10990, 11000, 11410, 11426, 10952, 10956, 10954, 10958, 10960, 10996, 11408, 11412, 11414, 10998, 11422, 10994 };
						int rand_item = alien_pod[rand() % alien_pod.size()];
						int count = 1;
						if (rand_item == 10990) count = 1;
						if (rand_item == 11000) count = 1;
						if (rand_item == 11410) count = 1;
						if (rand_item == 11426) count = 1;
						if (rand_item == 10952) count = 1;
						if (rand_item == 10956) count = 1;
						if (rand_item == 10954) count = 1;
						if (rand_item == 10958) count = 1;
						if (rand_item == 10960) count = 1;
						if (rand_item == 10958) count = 1;
						if (rand_item == 10996) count = 1;
						if (rand_item == 11408) count = 1;
						if (rand_item == 11412) count = 1;
						if (rand_item == 11414) count = 1;
						if (rand_item == 10998) count = 1;
						if (rand_item == 11422) count = 1;
						if (rand_item == 10994) count = 1;
						if (rand_item == 10960 || rand_item == 10956 || rand_item == 10958 || rand_item == 10954) {
							int target = 5;
							if (tile == 9286) target = 10;
							if ((rand() % 1000) <= target) {}
							else rand_item = 11422;
						}
						packet::SendTalkSelf(peer, "You received `2" + to_string(count) + " " + getItemDef(rand_item).name + "`` from the Alien Landing Pod.");
						packet::consolemessage(peer, "You received `2" + to_string(count) + " " + getItemDef(rand_item).name + "`` from the Alien Landing Pod.");
						bool success = true;
						SaveShopsItemMoreTimes(rand_item, count, peer, success);
						return;
					}
					else if (x == ((PlayerInfo*)(currentPeer->data))->x / 32 && y == ((PlayerInfo*)(currentPeer->data))->y / 32) {
						packet::SendTalkSelf(peer, "You can only use that on yourself.");
						return;
					}
					else {
						packet::SendTalkSelf(peer, "Must be used on a person.");
						return;
					}
				}
			}
			if (tile == 10456) {
				if (x == ((PlayerInfo*)(peer->data))->x / 32 && y == ((PlayerInfo*)(peer->data))->y / 32) {
					if (((PlayerInfo*)(peer->data))->cloth_back != 10456) {
						((PlayerInfo*)(peer->data))->cloth_back = 10456;
						((PlayerInfo*)(peer->data))->peffect = 8421559;
						sendPuncheffectpeer(peer, ((PlayerInfo*)(peer->data))->peffect);
						sendClothes(peer);
					}
					else {
						((PlayerInfo*)(peer->data))->cloth_back = 10426;
						((PlayerInfo*)(peer->data))->peffect = 8421559;
						sendPuncheffectpeer(peer, ((PlayerInfo*)(peer->data))->peffect);
						sendClothes(peer);
					}
				}
				else {
					packet::SendTalkSelf(peer, "Must be used on a person.");
				}
			}
		}
		return;
	}

	if (world == NULL) return;
	if (x<0 || y<0 || x>world->width - 1 || y>world->height - 1 || tile > itemDefs.size()) return; // needs - 1
	sendNothingHappened(peer, x, y);
	if (((PlayerInfo*)(peer->data))->adminLevel < 665)
	{
		if (world->items[x + (y * world->width)].foreground == 6 || world->items[x + (y * world->width)].foreground == 8 || world->items[x + (y * world->width)].foreground == 3760)
			return;
		if (tile == 6 || tile == 8 || tile == 3760 || tile == 6864)
			return;
	}
	if (world->name == "ADMIN" && !getAdminLevel(((PlayerInfo*)(peer->data))->rawName, ((PlayerInfo*)(peer->data))->tankIDPass))
	{
		if (world->items[x + (y * world->width)].foreground == 758)
			sendRoulete(peer, x, y);
		return;
	}
	if (world->name != "ADMIN") {
		if (world->owner != "") {

			string name = ((PlayerInfo*)(peer->data))->rawName;
			if (((PlayerInfo*)(peer->data))->rawName == world->owner || (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end() || ((PlayerInfo*)(peer->data))->adminLevel == 1337)) {
				if (((PlayerInfo*)(peer->data))->rawName == "") return;
				// WE ARE GOOD TO GO

				if (world->items[x + (y * world->width)].foreground == 242 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 1796 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 4428 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 2408 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 7188 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 5980 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 2950 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()) || world->items[x + (y * world->width)].foreground == 5638 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end()))
				{
					return;
				}

				if (tile == 32 && (find(world->acclist.begin(), world->acclist.end(), name) != world->acclist.end())) {
					return;
				}
				string offlinelist = "";
				string offname = "";
				int ischecked;
				int ischecked1;
				int ischecked2;
				for (std::vector<string>::const_iterator i = world->acclist.begin(); i != world->acclist.end(); ++i) {
					offname = *i;
					offlinelist += "\nadd_checkbox|isAccessed|" + offname + "|0|\n";

				}


				if (world->isPublic == true) {
					ischecked = 1;
				}
				else {
					ischecked = 0;
				}
				int noclap = 0;
				bool casin = world->isCasino;
				if (casin == true) {
					noclap = 1;
				}
				else {
					noclap = 0;
				}
				if (tile == 32) {
					if (world->items[x + (y * world->width)].foreground == 242 || world->items[x + (y * world->width)].foreground == 5814 || world->items[x + (y * world->width)].foreground == 2408 || world->items[x + (y * world->width)].foreground == 1796 || world->items[x + (y * world->width)].foreground == 4428 || world->items[x + (y * world->width)].foreground == 7188) {

						GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wEdit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "``|left|" + to_string(world->items[x + (y * world->width)].foreground) + "|\nadd_textbox|`wAccess list:|left|\nadd_spacer|small|" + offlinelist + "add_spacer|small|\nadd_player_picker|netid|`wAdd|\nadd_spacer|small|\nadd_checkbox|isWorldPublic|Allow anyone to build|" + std::to_string(ischecked) + "|\nadd_checkbox|allowNoclip|Disable noclip|" + std::to_string(noclap) + "|\nend_dialog|wlmenu|Cancel|OK|"));
						ENetPacket* packet = enet_packet_create(p.data,
							p.len,
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
						delete p.data;
					}
				}
			}

			else if (find(world->acclist.begin(), world->acclist.end(), ((PlayerInfo*)(peer->data))->rawName) != world->acclist.end())
			{
				if (world->items[x + (y * world->width)].foreground == 242 || world->items[x + (y * world->width)].foreground == 5814 || world->items[x + (y * world->width)].foreground == 2408 || world->items[x + (y * world->width)].foreground == 1796 || world->items[x + (y * world->width)].foreground == 4428 || world->items[x + (y * world->width)].foreground == 7188 || world->items[x + (y * world->width)].foreground == 4802 || world->items[x + (y * world->width)].foreground == 10410)
				{


					string ownername = world->owner;
					GamePacket p2 = packetEnd(appendIntx(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`0" + ownername + "'s `$World Lock`0. (`2Access Granted`w)"), 0), 1));


					ENetPacket* packet2 = enet_packet_create(p2.data,
						p2.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, packet2);
					delete p2.data;
					string text = "action|play_sfx\nfile|audio/punch_locked.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length()); // change memcpy here
					memcpy(data + 4 + text.length(), &zero, 1); // change memcpy here, revert to 4

					ENetPacket* packetsou = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);

					enet_peer_send(peer, 0, packetsou);


					return;
				}

			}
			else if (world->isPublic)
			{
				if (world->items[x + (y * world->width)].foreground == 242 || world->items[x + (y * world->width)].foreground == 5814 || world->items[x + (y * world->width)].foreground == 2408 || world->items[x + (y * world->width)].foreground == 1796 || world->items[x + (y * world->width)].foreground == 4428 || world->items[x + (y * world->width)].foreground == 7188)
				{
					string ownername = world->owner;
					GamePacket p2 = packetEnd(appendIntx(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`0" + ownername + "'s `$World Lock`0. (`2Access Granted`w)"), 0), 1));


					ENetPacket* packet2 = enet_packet_create(p2.data,
						p2.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, packet2);
					delete p2.data;
					string text = "action|play_sfx\nfile|audio/punch_locked.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length()); // change memcpy here
					memcpy(data + 4 + text.length(), &zero, 1); // change memcpy here, revert to 4

					ENetPacket* packetsou = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);

					enet_peer_send(peer, 0, packetsou);


					return;
				}

			}
			else {
				ItemDefinition pro;
				pro = getItemDef(world->items[x + (y * world->width)].foreground);
				if (world->items[x + (y * world->width)].foreground == 242 || world->items[x + (y * world->width)].foreground == 5814 || world->items[x + (y * world->width)].foreground == 2408 || world->items[x + (y * world->width)].foreground == 1796 || world->items[x + (y * world->width)].foreground == 4428 || world->items[x + (y * world->width)].foreground == 7188) {
					string ownername = world->owner;
					GamePacket p2 = packetEnd(appendIntx(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`0" + ownername + "'s `$World Lock`0. (`4No access`w)"), 0), 1));


					ENetPacket* packet2 = enet_packet_create(p2.data,
						p2.len,
						ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, packet2);
					delete p2.data;
					string text = "action|play_sfx\nfile|audio/punch_locked.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length()); // change memcpy here
					memcpy(data + 4 + text.length(), &zero, 1); // change memcpy here, revert to 4

					ENetPacket* packetsou = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);

					enet_peer_send(peer, 0, packetsou);


					return;
				}
				else
				{
					string text = "action|play_sfx\nfile|audio/punch_locked.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length()); // change memcpy here
					memcpy(data + 4 + text.length(), &zero, 1); // change memcpy here, revert to 4

					ENetPacket* packetsou = enet_packet_create(data,
						5 + text.length(),
						ENET_PACKET_FLAG_RELIABLE);

					enet_peer_send(peer, 0, packetsou);


					return;
				}

			} /*lockeds*/
			if (tile == 242 || tile == 2408 || tile == 1796 || tile == 4428 || tile == 7188) {



				GamePacket p3 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`0Only one `$World Lock`0 can be placed in a world!"), 0));


				ENetPacket* packet3 = enet_packet_create(p3.data,
					p3.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet3);
				delete p3.data;
				return;
			}
		}
	}

	if (world->items[x + (y * world->width)].foreground == 5958) {
		if (tile == 32) {
			packet::dialog(peer, "add_label_with_icon|big|`wEpoch Machine|left|5958|\nadd_spacer|small|\nadd_textbox|`oSelect Your Doom:|\nadd_spacer|small|\nadd_checkbox|epochice|Ice Age|" + to_string(world->ice) + "|\nadd_checkbox|epochvol|Volcano|" + to_string(world->volcano) + "|\nadd_checkbox|epochland|Floating Islands|" + to_string(world->land) + "|\nadd_text_input|timedilation|Cycle time (minutes): |0|3|\nend_dialog|epochweather|Cancel|Okay|");
		}
	}
	if (world->items[x + (y * world->width)].foreground == 5818) {
		if (tile == 32) {
			string leader = ((PlayerInfo*)(peer->data))->guildLeader;
			string rawname = ((PlayerInfo*)(peer->data))->rawName;
			((PlayerInfo*)(peer->data))->wrenchx = x;
			((PlayerInfo*)(peer->data))->wrenchy = y;
			int id = getItemDef(world->items[x + (y * world->width)].foreground).id;
			if (rawname == leader || isWorldOwner(peer, world) || isWorldAdmin(peer, world)) {
				if (world->items[x + (y * world->width)].isOpened == false) {
					packet::dialog(peer, "add_label_with_icon|big|Edit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "|left|" + to_string(id) + "|\nadd_checkbox|gopentopublic|`ois Open to Public?|0|\nend_dialog|gentrance|Cancel|OK|");
				}
				else {
					packet::dialog(peer, "add_label_with_icon|big|Edit " + getItemDef(world->items[x + (y * world->width)].foreground).name + "|left|" + to_string(id) + "|\nadd_checkbox|gopentopublic|`ois Open to Public?|1|\nend_dialog|gentrance|Cancel|OK|");
				}
			}
		}
	}
	if (world->items[x + (y * world->width)].foreground == 5000) {
		if (tile == 32) {
			if (world->owner == "" || isWorldOwner(peer, world) || isWorldAdmin(peer, world) || getPlyersWorld(peer)->isPublic) {
				string fyg = "set_default_color|`o\nadd_label_with_icon|big|`wWeather Machine - Background``|left|5000|\nadd_spacer|small|\nadd_textbox|`oYou can scan any Background Block to set it up in your weather machine.|\nadd_item_picker|bg_pick|Item: `2" + getItemDef(world->bgID).name + "|Select any Background Block|\nadd_quick_exit|\nend_dialog|bg_weather|Cancel|Okay|";
				packet::dialog(peer, fyg);
				((PlayerInfo*)(peer->data))->wrenchedBlockLocation = x + (y * world->width);
			}
			else {
				packet::SendTalkSelf(peer, "That area is owned by " + world->owner + "");
				return;
			}
		}
	}
	if (tile == 32) {
		if (world->items[x + (y * world->width)].foreground == 3832) {
			if (tile == 32) {
				if (world->owner == "" || isWorldOwner(peer, world) || isWorldAdmin(peer, world) || getPlyersWorld(peer)->isPublic) {
					string gaekid = "set_default_color|`o\nadd_label_with_icon|big|`wWeather Machine - Stuff``|left|3832|\nadd_spacer|small|\nadd_item_picker|stuff_pick|Item: `2" + getItemDef(world->stuffID).name + "|Select any item to rain down|\nadd_text_input|stuff_gravity|Gravity:|" + to_string(world->stuff_gravity) + "|3|\nadd_checkbox|stuff_spin|Spin Items|" + to_string(world->stuff_spin) + "|\nadd_checkbox|stuff_invert|Invert Sky Colors|" + to_string(world->stuff_invert) + "|\nadd_textbox|`oSelect item and click okay to continue!|\nend_dialog|stuff_weather|Cancel|Okay|";
					packet::dialog(peer, gaekid);
					((PlayerInfo*)(peer->data))->wrenchedBlockLocation = x + (y * world->width);
				}
				else {
					packet::SendTalkSelf(peer, "That area is owned by " + world->owner + "");
					return;
				}
			}
		}
	}

	if (tile == 18) {
		if (world->owner == "" || isWorldOwner(peer, world) || isWorldAdmin(peer, world) || getPlyersWorld(peer)->isPublic) {
			if (world->items[x + (y * world->width)].foreground == 340) {
				world->items[x + (y * world->width)].foreground = 0;
				int valgems = 0;
				valgems = rand() % 1000;
				int valgem = rand() % 1000;
				valgem = valgems + 1;
				std::ifstream ifs("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
				std::string content((std::istreambuf_iterator<char>(ifs)),
					(std::istreambuf_iterator<char>()));


				int gembux = atoi(content.c_str());
				int fingembux = gembux + valgem;
				ofstream myfile;
				myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
				myfile << fingembux;
				myfile.close();

				int gemcalc = gembux + valgem;
				GamePacket pp = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), gemcalc));
				ENetPacket* packetpp = enet_packet_create(pp.data,
					pp.len,
					ENET_PACKET_FLAG_RELIABLE);

				enet_peer_send(peer, 0, packetpp);
				delete pp.data;
			}
			if (world->items[x + (y * world->width)].foreground == 1210) {
				if (world->weather == 8) {
					world->weather = 0;
				}
				else {
					world->weather = 8;
				}
				OnSetCurrentWeather(peer, world->weather);
			}
			if (world->items[x + (y * world->width)].foreground == 5958) {
				if (world->weather == 38) {
					world->weather = 0;
				}
				else {
					world->weather = 38;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 3832) {
				if (world->weather == 29) {
					world->weather = 0;
				}
				else {
					world->weather = 29;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 984) {
				if (world->weather == 5) {
					world->weather = 0;
				}
				else {
					world->weather = 5;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 946) {
				if (world->weather == 3) {
					world->weather = 0;
				}
				else {
					world->weather = 3;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 934) {
				if (world->weather == 2) {
					world->weather = 0;
				}
				else {
					world->weather = 2;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 3252) {
				if (world->weather == 20) {
					world->weather = 0;
				}
				else {
					world->weather = 20;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 7644) {
				if (world->weather == 44) {
					world->weather = 0;
				}
				else {
					world->weather = 44;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 6854) {
				if (world->weather == 42) {
					world->weather = 0;
				}
				else {
					world->weather = 42;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 5716) {
				if (world->weather == 37) {
					world->weather = 0;
				}
				else {
					world->weather = 37;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 5654) {
				if (world->weather == 36) {
					world->weather = 0;
				}
				else {
					world->weather = 36;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 8896) {
				if (world->weather == 47) {
					world->weather = 0;
				}
				else {
					world->weather = 47;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 8836) {
				if (world->weather == 48) {
					world->weather = 0;
				}
				else {
					world->weather = 48;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 5112) {
				if (world->weather == 35) {
					world->weather = 0;
				}
				else {
					world->weather = 35;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 5000) {
				if (world->weather == 34) {
					world->weather = 0;
				}
				else {
					world->weather = 34;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 4892) {
				if (world->weather == 33) {
					world->weather = 0;
				}
				else {
					world->weather = 33;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 4776) {
				if (world->weather == 32) {
					world->weather = 0;
				}
				else {
					world->weather = 32;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 4486) {
				if (world->weather == 31) {
					world->weather = 0;
				}
				else {
					world->weather = 31;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 4242) {
				if (world->weather == 30) {
					world->weather = 0;
				}
				else {
					world->weather = 30;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 1490) {
				if (world->weather == 10) {
					world->weather = 0;
				}
				else {
					world->weather = 10;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 3694) {
				if (world->weather == 28) {
					world->weather = 0;
				}
				else {
					world->weather = 28;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 3534) {
				if (world->weather == 22) {
					world->weather = 0;
				}
				else {
					world->weather = 22;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 3446) {
				if (world->weather == 21) {
					world->weather = 0;
				}
				else {
					world->weather = 21;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 2284) {
				if (world->weather == 18) {
					world->weather = 0;
				}
				else {
					world->weather = 18;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 2046) {
				if (world->weather == 17) {
					world->weather = 0;
				}
				else {
					world->weather = 17;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 1750) {
				if (world->weather == 15) {
					world->weather = 0;
				}
				else {
					world->weather = 15;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 1364) {
				if (world->weather == 11) {
					world->weather = 0;
				}
				else {
					world->weather = 11;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 932) {
				if (world->weather == 4) {
					world->weather = 0;
				}
				else {
					world->weather = 4;
				}
				OnSetCurrentWeather(peer, world->weather);
			}

			if (world->items[x + (y * world->width)].foreground == 10286) {
				if (world->weather == 51) {
					world->weather = 0;
				}
				else {
					world->weather = 51;
				}
				OnSetCurrentWeather(peer, world->weather);
			}
		}
	}

	if (tile == 822) {
		world->items[x + (y * world->width)].water = !world->items[x + (y * world->width)].water;
		return;
	}
	if (tile == 1136) {
		packet::dialog(peer, "set_default_color|`w\n\nadd_label_with_icon|big|`wMars Blast|left|1136|\nadd_textbox|`oThis item creates a new world! Enter a unique name for it.|left|\nadd_text_input|marstext|`oNew World Name||30|\nend_dialog|sendmb|Cancel|`5Create!|\n");
		return;
	}
	if (tile == 3062)
	{
		world->items[x + (y * world->width)].fire = !world->items[x + (y * world->width)].fire;
		return;
	}
	if (tile == 1866)
	{
		world->items[x + (y * world->width)].glue = !world->items[x + (y * world->width)].glue;
		return;
	}
	ItemDefinition def;
	try {
		def = getItemDef(tile);
		if (def.blockType == BlockTypes::CLOTHING) return;
	}
	catch (int e) {
		def.breakHits = 4;
		def.blockType = BlockTypes::UNKNOWN;
#ifdef TOTAL_LOG
		cout << "Ugh, unsupported item " << tile << endl;
#endif
	}

	if (tile == 18) {
		if (world->items[x + (y * world->width)].background == 6864 && world->items[x + (y * world->width)].foreground == 0) return;
		if (world->items[x + (y * world->width)].background == 0 && world->items[x + (y * world->width)].foreground == 0) return;
		//data.netID = -1;
		int tool = ((PlayerInfo*)(peer->data))->cloth_hand;
		data.packetType = 0x8;
		data.plantingTree = (tool == 98 || tool == 1438 || tool == 4956) ? 8 : 6;
		int block = world->items[x + (y * world->width)].foreground > 0 ? world->items[x + (y * world->width)].foreground : world->items[x + (y * world->width)].background;
		//if (world->items[x + (y*world->width)].foreground == 0) return;
		using namespace std::chrono;
		if ((duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items[x + (y * world->width)].breakTime >= 4000)
		{
			world->items[x + (y * world->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			world->items[x + (y * world->width)].breakLevel = 0; // TODO
			if (world->items[x + (y * world->width)].foreground == 758)
				sendRoulete(peer, x, y);
		}
		if (y < world->height)
		{
			world->items[x + (y * world->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			world->items[x + (y * world->width)].breakLevel += (int)((tool == 98 || tool == 1438 || tool == 4956) ? 8 : 6); // TODO
		}


		if (y < world->height && world->items[x + (y * world->width)].breakLevel >= getItemDef(block).breakHits * 6) { // TODO
			data.packetType = 0x3;// 0xC; // 0xF // World::HandlePacketTileChangeRequest
			data.plantingTree = 18;
			world->items[x + (y * world->width)].breakLevel = 0;
			if (world->items[x + (y * world->width)].foreground != 0)
			{
				if (world->items[x + (y * world->width)].foreground == 242)
				{
					world->owner = "";
					world->isPublic = false;
				}
				world->items[x + (y * world->width)].foreground = 0;
			}
			else {
				world->items[x + (y * world->width)].background = 6864;
			}

		}


	}
	else {
		for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
		{
			if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == tile)
			{
				if ((unsigned int)((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount > 1)
				{
					((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount--;
				}
				else {
					((PlayerInfo*)(peer->data))->inventory.items.erase(((PlayerInfo*)(peer->data))->inventory.items.begin() + i);

				}
			}
		}
		if (def.blockType == BlockTypes::BACKGROUND)
		{
			world->items[x + (y * world->width)].background = tile;
		}
		else {
			if (world->items[x + (y * world->width)].foreground != 0)return;
			world->items[x + (y * world->width)].foreground = tile;
			ItemDefinition pro;
			pro = getItemDef(tile);
			if (tile == 242 || tile == 2408 || tile == 1796 || tile == 4428 || tile == 7188 || tile == 10410) {
				if (((PlayerInfo*)(peer->data))->rawName == world->owner) return;
				world->owner = ((PlayerInfo*)(peer->data))->rawName;
				world->ownerID = ((PlayerInfo*)(peer->data))->netID;
				world->isPublic = false;
				isLock = true;
				ENetPeer* currentPeer;

				for (currentPeer = server->peers;
					currentPeer < &server->peers[server->peerCount];
					++currentPeer)
				{
					if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
						continue;
					if (isHere(peer, currentPeer)) {
						packet::consolemessage(peer, "`5[`w" + world->name + " `ohas been World Locked by " + ((PlayerInfo*)(peer->data))->displayName + "`5]");
						packet::SendTalkSelf(peer, "`5[`w" + world->name + " `ohas been World Locked by " + ((PlayerInfo*)(peer->data))->displayName + "`5]");
						packet::PlayAudio(peer, "audio/use_lock.wav", 0);
					}
				}
			}

		}

		world->items[x + (y * world->width)].breakLevel = 0;
	}

	ENetPeer* currentPeer;

	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer))
			SendPacketRaw(4, packPlayerMoving(&data), 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
	}
	string name = ((PlayerInfo*)(peer->data))->rawName;
	if (isLock) {
		ENetPeer* currentPeer;

		for (currentPeer = server->peers;
			currentPeer < &server->peers[server->peerCount];
			++currentPeer)
		{
			if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
				continue;
			if (isHere(peer, currentPeer)) {
				sendTileData(currentPeer, x, y, 0x10, tile, world->items[x + (y * world->width)].background, lockTileDatas(0x20, ((PlayerInfo*)(peer->data))->userID, 0, 0, false, 100));
			}
		}
	}
}

void sendCollect(ENetPeer* peer, int netID, int itemNetID) {
	ENetPeer* currentPeer;
	PlayerMoving data;
	data.packetType = 14;
	data.netID = netID;
	data.plantingTree = itemNetID;
	data.characterState = 0;
	cout << "Request collect: " << std::to_string(itemNetID) << endl;
	WorldInfo* world = getPlyersWorld(peer);
	for (auto m_item = world->droppedItems.begin(); m_item != world->droppedItems.end(); ++m_item) {
		if ((checkForUIDMatch(peer, itemNetID)) == m_item->uid) {
			cout << "Success!" << endl;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (isHere(peer, currentPeer)) {
					data.plantingTree = getSharedUID(currentPeer, m_item->uid);
					BYTE* raw = packPlayerMoving(&data);
					SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
				}
			}
			world->droppedItems.erase(m_item);
			m_item--;
			return;
		}
	}
}
void sendWorld(ENetPeer* peer, WorldInfo* worldInfo)
{
	int zero = 0;
	((PlayerInfo*)(peer->data))->droppeditemcount = 0;
	((PlayerInfo*)(peer->data))->joinClothesUpdated = false;
	string asdf = "0400000004A7379237BB2509E8E0EC04F8720B050000000000000000FBBB0000010000007D920100FDFDFDFD04000000040000000000000000000000070000000000";
	string worldName = worldInfo->name;
	int xSize = worldInfo->width;
	int ySize = worldInfo->height;
	int square = xSize * ySize;
	__int16 nameLen = (__int16)worldName.length();
	int payloadLen = asdf.length() / 2;
	int dataLen = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 100;
	int offsetData = dataLen - 100;
	int allocMem = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 16000 + 100 + (worldInfo->droppedCount * 20);
	BYTE* data = new BYTE[allocMem];
	memset(data, 0, allocMem);
	for (int i = 0; i < asdf.length(); i += 2)
	{
		char x = ch2n(asdf[i]);
		x = x << 4;
		x += ch2n(asdf[i + 1]);
		memcpy(data + (i / 2), &x, 1);
	}

	__int16 item = 0;
	int smth = 0;
	for (int i = 0; i < square * 8; i += 4) memcpy(data + payloadLen + i + 14 + nameLen, &zero, 4);
	for (int i = 0; i < square * 8; i += 8) memcpy(data + payloadLen + i + 14 + nameLen, &item, 2);
	memcpy(data + payloadLen, &nameLen, 2);
	memcpy(data + payloadLen + 2, worldName.c_str(), nameLen);
	memcpy(data + payloadLen + 2 + nameLen, &xSize, 4);
	memcpy(data + payloadLen + 6 + nameLen, &ySize, 4);
	memcpy(data + payloadLen + 10 + nameLen, &square, 4);
	BYTE* blockPtr = data + payloadLen + 14 + nameLen;

	int sizeofblockstruct = 8;


	for (int i = 0; i < square; i++) {

		int tile = worldInfo->items[i].foreground;
		sizeofblockstruct = 8;


		//if (world->items[x + (y*world->width)].foreground == 242 or world->items[x + (y*world->width)].foreground == 2408 or world->items[x + (y*world->width)].foreground == 5980 or world->items[x + (y*world->width)].foreground == 2950 or world->items[x + (y*world->width)].foreground == 5814 or world->items[x + (y*world->width)].foreground == 4428 or world->items[x + (y*world->width)].foreground == 1796 or world->items[x + (y*world->width)].foreground == 4802 or world->items[x + (y*world->width)].foreground == 4994 or world->items[x + (y*world->width)].foreground == 5260 or world->items[x + (y*world->width)].foreground == 7188)
		if (tile == 6) {
			int type = 0x00010000;
			memcpy(blockPtr, &tile, 2);
			memcpy(blockPtr + 4, &type, 4);
			BYTE btype = 1;
			memcpy(blockPtr + 8, &btype, 1);

			string doorText = "EXIT";
			const char* doorTextChars = doorText.c_str();
			short length = (short)doorText.size();
			memcpy(blockPtr + 9, &length, 2);
			memcpy(blockPtr + 11, doorTextChars, length);
			sizeofblockstruct += 4 + length;
			dataLen += 4 + length; // it's already 8.

		}
		else if (getItemDef(tile).blockType == BlockTypes::SIGN || tile == 1420 || tile == 6124) {
			int type = 0x00010000;
			memcpy(blockPtr, &worldInfo->items[i].foreground, 2);
			memcpy(blockPtr + 4, &type, 4);
			BYTE btype = 2;
			memcpy(blockPtr + 8, &btype, 1);
			string signText = worldInfo->items[i].text;
			const char* signTextChars = signText.c_str();
			short length = (short)signText.size();
			memcpy(blockPtr + 9, &length, 2);
			memcpy(blockPtr + 11, signTextChars, length);
			int minus1 = -1;
			memcpy(blockPtr + 11 + length, &minus1, 4);
			sizeofblockstruct += 3 + length + 4;
			dataLen += 3 + length + 4; // it's already 8.
		}
		else if (tile == 3832) {
			int type = 0x00010000;
			memcpy(blockPtr, &worldInfo->items[i].foreground, 2);
			memcpy(blockPtr + 4, &type, 4);
			BYTE btype = 0x31;
			memcpy(blockPtr + 8, &btype, 1);


			short flags = 0;
			int item = worldInfo->items[i].displayBlock;
			int gravity = worldInfo->items[i].gravity;
			flags = 3;

			memcpy(blockPtr + 9, &item, 4);
			memcpy(blockPtr + 13, &gravity, 4);
			memcpy(blockPtr + 17, &flags, 2);
			sizeofblockstruct += 10;
			dataLen += 10;
		}
		else if ((worldInfo->items[i].foreground == 0) || (worldInfo->items[i].foreground == 2) || (worldInfo->items[i].foreground == 8) || (worldInfo->items[i].foreground == 100) || (worldInfo->items[i].foreground == 4))
		{

			memcpy(blockPtr, &worldInfo->items[i].foreground, 2);
			int type = 0x00000000;

			// type 1 = locked
			if (worldInfo->items[i].activated)
				type |= 0x00200000;
			if (worldInfo->items[i].water)
				type |= 0x04000000;
			if (worldInfo->items[i].glue)
				type |= 0x08000000;
			if (worldInfo->items[i].fire)
				type |= 0x10000000;
			if (worldInfo->items[i].red)
				type |= 0x20000000;
			if (worldInfo->items[i].green)
				type |= 0x40000000;
			if (worldInfo->items[i].blue)
				type |= 0x80000000;

			// int type = 0x04000000; = water
			// int type = 0x08000000 = glue
			// int type = 0x10000000; = fire
			// int type = 0x20000000; = red color
			// int type = 0x40000000; = green color
			// int type = 0x80000000; = blue color


			memcpy(blockPtr + 4, &type, 4);
			/*if (worldInfo->items[i].foreground % 2)
			{
				blockPtr += 6;
			}*/
		}
		else
		{
			memcpy(blockPtr, &zero, 2);
		}
		memcpy(blockPtr + 2, &worldInfo->items[i].background, 2);
		blockPtr += sizeofblockstruct;


	}

	/*int increase = 20;
//TODO

	int inc = 20;
	memcpy(blockPtr, &worldInfo->droppedCount, 4);
	memcpy(blockPtr + 4, &worldInfo->droppedCount, 4);

	for (int i = 0; i < worldInfo->droppedCount; i++) {

		memcpy(blockPtr + inc - 12, &worldInfo->droppedItems.at(i).id, 2);
		memcpy(blockPtr + inc - 10, &worldInfo->droppedItems.at(i).x, 4);
		memcpy(blockPtr + inc - 6, &worldInfo->droppedItems.at(i).y, 4);
		memcpy(blockPtr + inc - 2, &worldInfo->droppedItems.at(i).count, 2);
		memcpy(blockPtr + inc, &i, 4);
		inc += 16;

	}
	blockPtr += inc;
	dataLen += inc;*/

	//((PlayerInfo*)(peer->data))->droppeditemcount = worldInfo->droppedCount;
	offsetData = dataLen - 100;

	//              0       1       2       3       4       5       6       7       8       9      10     11      12      13      14
	string asdf2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	BYTE* data2 = new BYTE[101];
	memcpy(data2 + 0, &zero, 4);
	for (int i = 0; i < asdf2.length(); i += 2)
	{
		char x = ch2n(asdf2[i]);
		x = x << 4;
		x += ch2n(asdf2[i + 1]);
		memcpy(data2 + (i / 2), &x, 1);
	}
	int weather = worldInfo->weather;
	memcpy(data2 + 4, &weather, 4);

	memcpy(data + offsetData, data2, 100);


	//cout << dataLen << " <- dataLen allocMem -> " << allocMem << endl;
	memcpy(data + dataLen - 4, &smth, 4);
	ENetPacket* packet2 = enet_packet_create(data,
		dataLen,
		ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet2);
	//enet_host_flush(server);
	for (int i = 0; i < square; i++) {
		ItemDefinition pro;
		pro = getItemDef(worldInfo->items[i].foreground);
		if ((worldInfo->items[i].foreground == 0) || (getItemDef(worldInfo->items[i].foreground).blockType) == BlockTypes::SIGN || worldInfo->items[i].foreground == 1420 || worldInfo->items[i].foreground == 6214 || (worldInfo->items[i].foreground == 3832) || (worldInfo->items[i].foreground == 2946) || (worldInfo->items[i].foreground == 6) || (worldInfo->items[i].foreground == 4) || (worldInfo->items[i].foreground == 2) || (worldInfo->items[i].foreground == 8) || (worldInfo->items[i].foreground == 100))
			; // nothing
		else if (worldInfo->items[i].foreground == 242 || worldInfo->items[i].foreground == 2408 || worldInfo->items[i].foreground == 1796 || worldInfo->items[i].foreground == 4428 || worldInfo->items[i].foreground == 7188)
		{
			ENetPeer* currentPeer;

			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (isHere(peer, currentPeer)) {
					int x = i % xSize, y = i / xSize;
					sendTileData(currentPeer, x, y, 0x10, worldInfo->items[x + (y * worldInfo->width)].foreground, worldInfo->items[x + (y * worldInfo->width)].background, lockTileDatas(0x20, worldInfo->ownerID, 0, 0, false, 100));
				}
			}
		}
		else
		{
			PlayerMoving data;
			//data.packetType = 0x14;
			data.packetType = 0x3;

			//data.characterState = 0x924; // animation
			data.characterState = 0x0; // animation
			data.x = i % worldInfo->width;
			data.y = i / worldInfo->height;
			data.punchX = i % worldInfo->width;
			data.punchY = i / worldInfo->width;
			data.XSpeed = 0;
			data.YSpeed = 0;
			data.netID = -1;
			data.plantingTree = worldInfo->items[i].foreground;
			SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
			int x = i % xSize, y = i / xSize;
			UpdateBlockState(peer, x, y, true, worldInfo);
		}
	}
	int idx = 0;
	for (int i = 0; i < worldInfo->droppedItemUid; i++)
	{
		bool found = false;
		for (int j = 0; j < worldInfo->droppedItems.size(); j++)
		{
			if (worldInfo->droppedItems.at(j).uid == i)
			{
				SendDropSingle(peer, -1, worldInfo->droppedItems.at(j).x, worldInfo->droppedItems.at(j).y, worldInfo->droppedItems.at(j).id, worldInfo->droppedItems.at(j).count, 0);
				found = true;
				break;
			}
		}
		// temporary fix
		if (!found) SendDropSingle(peer, -1, -1000, -1000, 0, 1, 0);
	}
	((PlayerInfo*)(peer->data))->currentWorld = worldInfo->name;
	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			int squaresign = i + (j * 100);

			bool displaysss = std::experimental::filesystem::exists("display/" + worldInfo->name + "X" + std::to_string(squaresign) + ".txt");

			if (displaysss) {
				if (worldInfo->items[squaresign].foreground == 2946)
				{

					int x = squaresign % worldInfo->width;
					int y = squaresign / worldInfo->width;
					//cout << "[!] foundzzzzzzzzzzzzzz!";
					WorldInfo* world = getPlyersWorld(peer);
					ENetPeer* currentPeer;
					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;
						if (isHere(peer, currentPeer)) {
							BlockVisual data;
							data.packetType = 0x5;
							data.characterState = 8;
							data.punchX = x;
							data.punchY = y;
							data.charStat = 13; // 13y
							data.blockid = 2946; // 2946 3794 = display shelf
							//data.netID = ((PlayerInfo*)(peer->data))->netID;
							data.backgroundid = 6864;
							data.visual = 0x00010000; //0x00210000

							std::ifstream ifs("display/" + worldInfo->name + "X" + std::to_string(squaresign) + ".txt");
							std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
							int id = atoi(content.c_str());

							world->items[x + (y * world->width)].displayblock = id;

							int n = id;
							string hex = "";
							{
								std::stringstream ss;
								ss << std::hex << n; // int decimal_value
								std::string res(ss.str());

								hex = res + "17";
							}

							if (hex == "2017") {
								continue;
							}


							int xx;
							std::stringstream ss;
							ss << std::hex << hex;
							ss >> xx;
							data.displayblock = xx;


							SendPacketRaw(192, packBlockVisual(&data), 69, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);

						}
					}
				}
			}
		}
	}

	int otherpeople = 0;
	int count = 0;
	ENetPeer* currentPeer;
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		count++;
		if (isHere(peer, currentPeer))
			otherpeople++;
	}
	string gstatement, gleader;
	((PlayerInfo*)(peer->data))->guildLeader = gleader;
	if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
		((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + "`w(`eGL`w)";
	}
	int otherpeoples = otherpeople - 1;
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 0) {
				((PlayerInfo*)(peer->data))->displayName = "`2" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
			else if (((PlayerInfo*)(peer->data))->legend == true) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + " of Legend";
			}
			else if (((PlayerInfo*)(peer->data))->level >= 125) {
				((PlayerInfo*)(peer->data))->displayName >= "`4Dr. " + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 0) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName;
			}
			else if (((PlayerInfo*)(peer->data))->legend == true) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + " of Legend";
			}
			else if (((PlayerInfo*)(peer->data))->level >= 125) {
				((PlayerInfo*)(peer->data))->displayName >= "`4Dr. " + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 0) {
				((PlayerInfo*)(peer->data))->displayName = "`2" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
			else if (((PlayerInfo*)(peer->data))->legend == true) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + " of Legend";
			}
			else if (((PlayerInfo*)(peer->data))->level >= 125) {
				((PlayerInfo*)(peer->data))->displayName >= "`4Dr. " + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 0) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName;
			}
			else if (((PlayerInfo*)(peer->data))->legend == true) {
				((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + " of Legend";
			}
			else if (((PlayerInfo*)(peer->data))->level >= 125) {
				((PlayerInfo*)(peer->data))->displayName >= "`4Dr. " + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`#@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`#@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`#@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`#@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
				((PlayerInfo*)(peer->data))->displayName = "`c@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
				((PlayerInfo*)(peer->data))->displayName = "`c@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
				((PlayerInfo*)(peer->data))->displayName = "`c@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
				((PlayerInfo*)(peer->data))->displayName = "`cc@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
				((PlayerInfo*)(peer->data))->displayName = "`4@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	if (((PlayerInfo*)(peer->data))->haveGrowId) {
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
				((PlayerInfo*)(peer->data))->displayName = "`6@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
				((PlayerInfo*)(peer->data))->displayName = "`6@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}
	else
	{
		if (((PlayerInfo*)(peer->data))->rawName == worldInfo->owner)
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
				((PlayerInfo*)(peer->data))->displayName = "`6@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}

		else
			string gstatement, gleader;
		((PlayerInfo*)(peer->data))->guildLeader = gleader;
		if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
			((PlayerInfo*)(peer->data))->displayName = ((PlayerInfo*)(peer->data))->tankIDName + "`w(`eGL`w)";
		}
		else
		{
			if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
				((PlayerInfo*)(peer->data))->displayName = "`6@" + ((PlayerInfo*)(peer->data))->tankIDName;
			}
		}
	}

	string act = ((PlayerInfo*)(peer->data))->currentWorld;
	sendState(peer);
	if (worldInfo->weather == 29) {
		WorldInfo* world = getPlyersWorld(peer);
		int x = 0;
		int y = 0;
		int stuffGra = world->stuff_gravity;
		bool stuff_spin = world->stuff_spin;
		int stuffID = world->stuffID;
		bool stuff_invert = world->stuff_invert;
		world->stuff_invert = stuff_invert;
		world->stuff_spin = stuff_spin;
		world->stuff_gravity = stuffGra;
		world->stuffID = stuffID;
		sendStuffweather(currentPeer, x, y, world->stuffID, stuffGra, stuff_spin, stuff_invert);
	}
	else {
		GamePacket p7 = packetEnd(appendInt(appendString(createPacket(), "OnSetCurrentWeather"), worldInfo->weather));
		ENetPacket* packet7 = enet_packet_create(p7.data,
			p7.len,
			ENET_PACKET_FLAG_RELIABLE);

		enet_peer_send(peer, 0, packet7);
		delete p7.data;
	}
	string nameworld = worldInfo->name;
	string ownerworld = worldInfo->owner;
	string accessname = "";
	for (std::vector<string>::const_iterator i = worldInfo->acclist.begin(); i != worldInfo->acclist.end(); ++i) {
		accessname = *i;
	}
	if (worldInfo->owner == ((PlayerInfo*)(peer->data))->rawName)
	{

		GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`5 `5[`0" + nameworld + " `$World Locked `oby " + ownerworld + " ``(`2ACCESS GRANTED``)`5]"));
		ENetPacket* packet3 = enet_packet_create(p3.data,
			p3.len,
			ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet3);
		delete p3.data;





	}
	else if (((PlayerInfo*)(peer->data))->rawName == accessname)
	{

		GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`5 `5[`0" + nameworld + " `$World Locked `oby " + ownerworld + " ``(`2ACCESS GRANTED``)`5]"));
		ENetPacket* packet3 = enet_packet_create(p3.data,
			p3.len,
			ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet3);
		delete p3.data;





	}
	else if (((PlayerInfo*)(peer->data))->adminLevel == 1337)
	{
		if (ownerworld != "") {
			GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`5 `5[`0" + nameworld + " `$World Locked `oby " + ownerworld + " ``(`2ACCESS GRANTED``)`5]"));
			ENetPacket* packet3 = enet_packet_create(p3.data,
				p3.len,
				ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet3);
			delete p3.data;

		}





	}

	else
	{

		if (ownerworld != "") {
			GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`5 `5[`0" + nameworld + " `$World Locked `oby " + ownerworld + "`5]"));
			ENetPacket* packet3 = enet_packet_create(p3.data,
				p3.len,
				ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet3);
			delete p3.data;
		}
	}
	delete data;
}

void sendPlayerLeave(ENetPeer* peer, PlayerInfo* player)
{
	ENetPeer* currentPeer;
	gamepacket_t p;
	p.Insert("OnRemove");
	p.Insert("netID|" + std::to_string(player->netID) + "\n");
	gamepacket_t p2;
	p2.Insert("OnConsoleMessage");
	p2.Insert("`5<`w" + player->displayName + "`` `5left, `w" + std::to_string(getPlayersCountInWorld(player->currentWorld) - 1) + "`` `5others here>``");
	for (currentPeer = server->peers;
		currentPeer < &server->peers[server->peerCount];
		++currentPeer)
	{
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
			continue;
		if (isHere(peer, currentPeer)) {
			{
				p.CreatePacket(peer);

				{
					p.CreatePacket(currentPeer);
				}

			}
			{
				p2.CreatePacket(currentPeer);
			}
		}
	}
}

void sendDrop(ENetPeer* peer, int netID, int x, int y, int item, int count, BYTE specialEffect, bool onlyForPeer)
{
	if (item >= 7068) return;
	if (item < 0) return;
	if (onlyForPeer) {
		PlayerMoving data;
		data.packetType = 14;
		data.x = x;
		data.y = y;
		data.netID = netID;
		data.plantingTree = item;
		float val = count; // item count
		BYTE val2 = specialEffect;

		BYTE* raw = packPlayerMoving(&data);
		memcpy(raw + 16, &val, 4);
		memcpy(raw + 1, &val2, 1);

		SendPacketRaw(4, raw, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
	}
	else {
		DroppedItem dropItem;
		dropItem.x = x;
		dropItem.y = y;
		dropItem.count = count;
		dropItem.id = item;
		dropItem.uid = worldDB.get2(((PlayerInfo*)(peer->data))->currentWorld).ptr->currentItemUID++;
		worldDB.get2(((PlayerInfo*)(peer->data))->currentWorld).ptr->droppedItems.push_back(dropItem);
		ENetPeer* currentPeer;
		for (currentPeer = server->peers;
			currentPeer < &server->peers[server->peerCount];
			++currentPeer)
		{
			if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
				continue;
			if (isHere(peer, currentPeer)) {

				ItemSharedUID m_uid;
				m_uid.actual_uid = dropItem.uid;
				m_uid.shared_uid = (((PlayerInfo*)(currentPeer->data)))->last_uid++;
				(((PlayerInfo*)(currentPeer->data)))->item_uids.push_back(m_uid);
				PlayerMoving data;
				data.packetType = 14;
				data.x = x;
				data.y = y;
				data.netID = netID;
				data.plantingTree = item;
				float val = count; // item count
				BYTE val2 = specialEffect;

				BYTE* raw = packPlayerMoving(&data);
				memcpy(raw + 16, &val, 4);
				memcpy(raw + 1, &val2, 1);

				SendPacketRaw(4, raw, 56, 0, currentPeer, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}