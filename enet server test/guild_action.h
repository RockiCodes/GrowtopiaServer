#pragma once

void updateGuild(ENetPeer* peer) {
	string guildname = PlayerDB::getProperName(((PlayerInfo*)(peer->data))->guild);
	if (guildname != "") {
		std::ifstream ifff("guilds/" + guildname + ".json");
		if (ifff.fail()) {
			ifff.close();
			cout << "Failed to loading guilds/" + guildname + ".json! From " + ((PlayerInfo*)(peer->data))->displayName + "." << endl;
			((PlayerInfo*)(peer->data))->guild = "";
			updateGuild;
		}
		json j;
		ifff >> j;

		int gfbg, gffg;

		string gstatement, gleader;

		vector<string> gmembers;

		gfbg = j["backgroundflag"];
		gffg = j["foregroundflag"];
		gstatement = j["GuildStatement"].get<string>();
		gleader = j["Leader"].get<string>();
		for (int i = 0; i < j["Member"].size(); i++) {
			gmembers.push_back(j["Member"][i]);
		}

		if (find(gmembers.begin(), gmembers.end(), ((PlayerInfo*)(peer->data))->rawName) == gmembers.end()) {
			((PlayerInfo*)(peer->data))->guild = "";
		}
		else {
			((PlayerInfo*)(peer->data))->guildBg = gfbg;
			((PlayerInfo*)(peer->data))->guildFg = gffg;
			((PlayerInfo*)(peer->data))->guildStatement = gstatement;
			((PlayerInfo*)(peer->data))->guildLeader = gleader;
			((PlayerInfo*)(peer->data))->guildMembers = gmembers;
		}

		ifff.close();
	}
}