/**********************************************************************************
    Great Open Sourced Growtopia Private Server Made With ENet
    Copyright (C) 2023  Rocki/JenZip

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************************/

#include "stdafx.h"
#include "back_end.h"
#include "game_packet.h"
#include "discord_webhook.h"
#include "player_base.h"
#include "player_packet.h"
#include "player_action.h"
#include "world_base.h"
#include "guild_action.h"
#include "world_action.h"
#include "server_sided.h"

int main()
{
	cout << "Growtopia Private Server (c) Rocki, Credit : GrowtopiaNoobs" << endl;
	system("Color 0A");
		
	cout << "Loading config from config.json" << endl;
	loadConfig();
		
	enet_initialize();
	ENetAddress address;
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	enet_address_set_host (&address, "0.0.0.0");
	//address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = configPort;
	server = enet_host_create(&address /* the address to bind the server host to */,
		1024      /* allow up to 32 clients and/or outgoing connections */,
		10      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet server host.\n");
		while (1);
		exit(EXIT_FAILURE);
	}
	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);
	cout << "Building items database..." << endl;
	ifstream myFile("items.dat");
	if (myFile.fail()) {
		std::cout << "Items.dat not found!" << endl;
		std::cout << "Please put items.dat in this folder:" << endl;
                system("cd");
		std::cout << "If you dont have items.dat, you can get it from Growtopia cache folder. Please exit." << endl;
                while (true);
	}
	buildItemsDatabase();
	cout << "Database is built!" << endl;
	loadnews();

	thread AutoSaveWorlds(autoSaveWorlds);
	if (AutoSaveWorlds.joinable()) AutoSaveWorlds.detach();

	worldDB.get("START");
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (true)
	while (enet_host_service(server, &event, 1000) > 0)
	{
		ENetPeer* peer = event.peer;
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
#ifdef TOTAL_LOG
			printf("A new client connected.\n");
#endif
			
			/* Store any relevant client information here. */
			//event.peer->data = "Client information";
			ENetPeer * currentPeer;
			int count = 0;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;
				if (currentPeer->address.host == peer->address.host)
					count++;
			}

			event.peer->data = new PlayerInfo;
			/* Get the string ip from peer */
			char clientConnection[16];
			enet_address_get_host_ip(&peer->address, clientConnection, 16);
			((PlayerInfo*)(peer->data))->charIP = clientConnection;
			if (count > 3)
			{
				packet::consolemessage(peer, "`rToo many accounts are logged on from this IP. Log off one account before playing please.``");
				enet_peer_disconnect_later(peer, 0);
			}
			else {
				sendData(peer, 1, 0, 0);
			}


			continue;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			if (((PlayerInfo*)(peer->data))->isUpdating)
			{
				cout << "packet drop" << endl;
				continue;
			}

			int messageType = GetMessageTypeFromPacket(event.packet);

			WorldInfo* world = getPlyersWorld(peer);
			switch (messageType) {
			case 2:
			{
				//cout << GetTextPointerFromPacket(event.packet) << endl;
				string cch = GetTextPointerFromPacket(event.packet);
				if (cch.find("©") != std::string::npos) enet_peer_reset(peer);
				string str = cch.substr(cch.find("text|") + 5, cch.length() - cch.find("text|") - 1);
				if (cch.find("action|wrench") == 0) {
					vector<string> ex = explode("|", cch);


					stringstream ss;


					ss << ex[3];


					string temp;
					int found;
					while (!ss.eof()) {


						ss >> temp;


						if (stringstream(temp) >> found)
							//cout << found;
							((PlayerInfo*)(peer->data))->wrenchsession = found;


						temp = "";
					}
					string worldsowned;
					string rolex;
					string rolexx;
					if (((PlayerInfo*)(peer->data))->haveGrowId == false) {
						continue;
					}
					ENetPeer* currentPeer;
					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;


						if (isHere(peer, currentPeer)) {
							if (((PlayerInfo*)(currentPeer->data))->haveGrowId == false) {
								continue;
							}
							if (((PlayerInfo*)(currentPeer->data))->netID == ((PlayerInfo*)(peer->data))->wrenchsession) {
								std::ifstream ifs("gemdb/" + ((PlayerInfo*)(currentPeer->data))->rawName + ".txt");
								std::string content2x((std::istreambuf_iterator<char>(ifs)),
									(std::istreambuf_iterator<char>()));

								((PlayerInfo*)(peer->data))->lastInfo = ((PlayerInfo*)(currentPeer->data))->rawName;
								((PlayerInfo*)(peer->data))->lastInfoname = ((PlayerInfo*)(currentPeer->data))->tankIDName;

								string name = ((PlayerInfo*)(currentPeer->data))->displayName;
								string rawnam = ((PlayerInfo*)(peer->data))->rawName;
								string rawnamofwrench = ((PlayerInfo*)(currentPeer->data))->rawName;
								string guildleader = ((PlayerInfo*)(peer->data))->guildLeader;
								if (rawnamofwrench != rawnam)
								{

									if (rawnamofwrench != "")
									{
										if (find(((PlayerInfo*)(peer->data))->guildMembers.begin(), ((PlayerInfo*)(peer->data))->guildMembers.end(), name) != ((PlayerInfo*)(peer->data))->guildMembers.end()) {
											if (world->owner == ((PlayerInfo*)(peer->data))->rawName && ((PlayerInfo*)(peer->data))->haveGrowId || ((PlayerInfo*)(peer->data))->adminLevel >= 777)
											{
												if (((PlayerInfo*)(peer->data))->adminLevel >= 999 || ((PlayerInfo*)(peer->data))->rawName == "saku" || ((PlayerInfo*)(peer->data))->rawName == "nabz")
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small||\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_button|infobutton|`!Punish/View|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
												else
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|18|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|giveownership|`wGive ownership to this player!|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
											}
											else
											{
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet);
												delete p.data;
											}
										}
										else if (((PlayerInfo*)(peer->data))->rawName == guildleader) {
											if (world->owner == ((PlayerInfo*)(peer->data))->rawName && ((PlayerInfo*)(peer->data))->haveGrowId || ((PlayerInfo*)(peer->data))->adminLevel >= 777)
											{
												if (((PlayerInfo*)(peer->data))->adminLevel >= 999 || ((PlayerInfo*)(peer->data))->rawName == "mindpin")
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_label|small|\nadd_button|trades|`9Trade|0|0|\nadd_button|infobutton|`!Punish/View|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|inviteguildbutton|`2Invite to guild``|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
												else
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|inviteguildbutton|`2Invite to guild``|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
											}
											else
											{
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_button|inviteguildbutton|`2Invite to guild``|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet);
												delete p.data;
											}
										}
										else {
											if (world->owner == ((PlayerInfo*)(peer->data))->rawName && ((PlayerInfo*)(peer->data))->haveGrowId || ((PlayerInfo*)(peer->data))->adminLevel >= 777)
											{
												if (((PlayerInfo*)(peer->data))->adminLevel >= 999 || ((PlayerInfo*)(peer->data))->rawName == "mindpin")
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`w" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`9Trade|0|0|\nadd_button|infobutton|`!Punish/View|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|giveownership|`wGive ownership to this player!|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
												else
												{
													GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_button|pull|`5Pull|0|0|\nadd_button|kick|`4Kick|0|0|\nadd_button|wban|`4World Ban|0|0|\nadd_button|giveownership|`wGive ownership to this player!|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
													ENetPacket* packet = enet_packet_create(p.data,
														p.len,
														ENET_PACKET_FLAG_RELIABLE);
													enet_peer_send(peer, 0, packet);
													delete p.data;
												}
											}
											else
											{
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_label|small|\nadd_label|small|`oGems : `6" + content2x + "$|left|4|\nadd_button|trades|`wTrade|0|0|\nadd_spacer|small|\nadd_button||Continue|0|0|\nadd_quick_exit"));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet);
												delete p.data;
											}
										}
									}
									else
									{
										GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\nadd_label_with_icon|big|`0" + name + " `w(`2" + to_string(((PlayerInfo*)(currentPeer->data))->level) + "`w)``|left|" + getRankId(((PlayerInfo*)(peer->data))->lastInfoname) + "|\nadd_spacer|small|\nadd_button|chc0|Close|noflags|0|0|\n\nadd_quick_exit|\nnend_dialog|gazette||OK|"));
										ENetPacket* packet = enet_packet_create(p.data,
											p.len,
											ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packet);
										delete p.data;
									}
								}
								else
								{
									if (((PlayerInfo*)(peer->data))->haveGrowId == true)
									{
										std::ostringstream oss;
										if (!((PlayerInfo*)(peer->data))->worldsowned.empty())
										{
											std::copy(((PlayerInfo*)(peer->data))->worldsowned.begin(), ((PlayerInfo*)(peer->data))->worldsowned.end() - 1,
												std::ostream_iterator<string>(oss, " "));

											// Now add the last element with no delimiter
											oss << ((PlayerInfo*)(peer->data))->worldsowned.back();
										}
										else {
											string oss = "You dont have any worlds!";
										}
										int levels = ((PlayerInfo*)(peer->data))->level;
										int xp = ((PlayerInfo*)(peer->data))->xp;
										string currentworld = ((PlayerInfo*)(peer->data))->currentWorld;
										int yy = ((PlayerInfo*)(peer->data))->posX / 32;
										int xx = ((PlayerInfo*)(peer->data))->posY / 32;

										if (((PlayerInfo*)(peer->data))->isinvited == true)
										{
											GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_player_info|" + name + " | " + std::to_string(levels) + " | " + std::to_string(xp) + " | " + to_string(levels * 1500) + "|\nadd_spacer|small|\nadd_button|\nadd_spacer|small|\nadd_button|growmojis|`$Growmojis|\nadd_spacer|small|\nadd_button|ingamerole|`6Purchase Rank||\nadd_spacer|small|\nadd_button|chc0|Close|noflags|0|0|\n\nadd_quick_exit|\nnend_dialog|gazette||OK|"));
											ENetPacket* packet = enet_packet_create(p.data,
												p.len,
												ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(peer, 0, packet);
											delete p.data;
										}
										else
										{
											GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_player_info|" + name + " | " + std::to_string(levels) + " | " + std::to_string(xp) + " | " + to_string(levels * 1500) + "|\nadd_spacer|small|\nadd_button|\nadd_spacer|small|\nadd_button|growmojis|`$Growmojis|\nadd_spacer|small|\nadd_button|ingamerole|`6Purchase Rank||\nadd_spacer|small|\nadd_button|chc0|Close|noflags|0|0|\n\nadd_quick_exit|\nnend_dialog|gazette||OK|"));
											ENetPacket* packet = enet_packet_create(p.data,
												p.len,
												ENET_PACKET_FLAG_RELIABLE);
											enet_peer_send(peer, 0, packet);
											delete p.data;
										}
									}
									else
									{
									}
								}

							}


						}


					}
				}

				if (cch.find("action|friends\n") == 0) {
					if (((PlayerInfo*)(peer->data))->haveGrowId == false) {
						packet::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\n\nadd_spacer|small|\nadd_textbox|A `wGrowID `wmeans `oyou can use a name and password to logon from any device.|\nadd_spacer|small|\nadd_textbox|This `wname `owill be reserved for you and `wshown to other players`o, so choose carefully!|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nadd_text_input|passwordverify|Password Verify||100|\nadd_textbox|Your `wemail address `owill only be used for account verification purposes and won't be spammed or shared. If you use a fake email, you'll never be able to recover or change your password.|\nadd_text_input|email|Email||100|\nadd_textbox|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag`o. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o.|\nadd_text_input|discord|Discord||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
						enet_host_flush(server);
					}
					if (((PlayerInfo*)(peer->data))->joinguild == true) {
						string properson = "set_default_color|`w\n\nadd_label_with_icon|big|Social Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|backonlinelist|Show Friends``|0|0|\nadd_button|showguild|Show Guild Members``|0|0|\nend_dialog||OK||\nadd_quick_exit|";
						packet::dialog(peer, properson);
					}
					else {
						string gayfriend = "set_default_color|`w\n\nadd_label_with_icon|big|Social Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|backonlinelist|Show Friends``|0|0|\nadd_button|createguildinfo|Create Guild``|0|0|\nend_dialog||OK||\nadd_quick_exit|";
						packet::dialog(peer, gayfriend);
					}
				}

				if (cch.find("action|setSkin") == 0) {
					if (!world) continue;
					std::stringstream ss(cch);
					std::string to;
					int id = -1;
					string color;
					while (std::getline(ss, to, '\n')) {
						vector<string> infoDat = explode("|", to);
						if (infoDat[0] == "color") color = infoDat[1];
						if (has_only_digits(color) == false) continue;
						id = atoi(color.c_str());
						if (color == "2190853119") {
							id = -2104114177;
						}
						else if (color == "2527912447") {
							id = -1767054849;
						}
						else if (color == "2864971775") {
							id = -1429995521;
						}
						else if (color == "3033464831") {
							id = -1261502465;
						}
						else if (color == "3370516479") {
							id = -924450817;
						}

					}
					((PlayerInfo*)(peer->data))->skinColor = id;
					sendClothes(peer);
				}

				if (cch.find("action|buy\nitem|store") == 0)
				{
					string text1 = "set_description_text|Welcome to the `2Growtopia Store``! Select the item you'd like more info on.`o `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||1|0|0|0||||-1|-1||||";;
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|1|";
					string text10 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons/store_buttons16.rttex|September 2021:`` `9Ouroboros Charm``!<CR><CR>`2You Get:`` 1 `9Ouroboros Charm``.<CR><CR>`5Description: ``The endless loop of life and death, personified and celebrated. Is it a charm or is it a curse?|0|3|350000|0||interface/large/gui_store_button_overlays1.rttex|0|0||-1|-1||1|||||||";
					string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1||||||";
					string text12 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|2|";
					string text13 = "|\nadd_button|gems_glory|Road To Glory|interface/large/store_buttons/store_buttons30.rttex|rt_grope_loyalty_bundle01|0|0|5|0||interface/large/gui_store_button_overlays1.rttex|0|0|/interface/large/gui_shop_buybanner.rttex|1|0|`2You Get:`` Road To Glory and 100k Gems Instantly.<CR>`5Description:`` Earn Gem rewards when you level up. Every 10 levels you will get additional Gem rewards up to Level 50! Claim all rewards instantly if you are over level 50!! 1.6M Gems in total!! |1|||||||";
					string text14 = "|\nadd_button|gems_rain|It's Rainin' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_rain|1|5|0|0|||-1|-1||-1|-1|`2You Get:`` 200,000 Gems, 2 Growtoken, and 1 Megaphone.<CR><CR>`5Description:`` All the gems you could ever want and more plus 2 Growtokens and a Megaphone to tell the whole world about it.|1|||||||";
					string text15 = "|\nadd_button|gems_fountain|Gem Fountain|interface/large/store_buttons/store_buttons2.rttex|rt_grope_gem_fountain|0|2|0|0|||-1|-1||-1|-1|`2You Get:`` 90,000 Gems and 1 Growtoken.<CR><CR>`5Description:`` Get a pile of gems to shop to your hearts desire and 1 Growtoken.|1|||||||";
					string text16 = "|\nadd_button|gems_chest|Chest o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_chest|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 30,000 Gems.<CR><CR>`5Description:`` Get a chest containing gems.|1|||||||";
					string text17 = "|\nadd_button|gems_bag|Bag o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_bag|1|0|0|0|||-1|-1||-1|-1|`2You Get:`` 14,000 Gems.<CR><CR>`5Description:`` Get a small bag of gems.|1|||||||";
					string text18 = "|\nadd_button|tapjoy|Earn Free Gems|interface/large/store_buttons/store_buttons.rttex||1|2|0|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|3|";
					string text20 = "|\nadd_button|365d|`o1-Year Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle02|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 1-Year Subscription Token and 25 Growtokens.<CR><CR>`5Description:`` One full year of special treatment AND 25 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
					string text21 = "|\nadd_button|30d|`o30-Day Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle01|0|4|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 30-Day Free Subscription Token and 2 Growtokens.<CR><CR>`5Description:`` 30 full days of special treatment AND 2 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
					string text22 = "|\nadd_button|video_tapjoy|Watch Videos For Gems|interface/large/store_buttons/store_buttons29.rttex||0|1|0|0|1/5 VIDEOS WATCHED||-1|-1||-1|-1||1||||||";

					/*
					string text1 = "set_description_text|Welcome to the `2Growtopia Store``!  Tap the item you'd like more info on.`o  `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
					string text2 = "|\nadd_button|iapp_menu|Buy Gems|interface/large/store_buttons5.rttex||0|2|0|0|";
					string text3 = "|\nadd_button|subs_menu|Subscriptions|interface/large/store_buttons22.rttex||0|1|0|0|";
					string text4 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons16.rttex|`2September 2018:`` `9Sorcerer's Tunic of Mystery!`` Capable of reflecting the true colors of the world around it, this rare tunic is made of captured starlight and aether. If you think knitting with thread is hard, just try doing it with moonbeams and magic! The result is worth it though, as these clothes won't just make you look amazing - you'll be able to channel their inherent power into blasts of cosmic energy!``|0|3|200000|0|";
					string text5 = "|\nadd_button|locks_menu|Locks And Stuff|interface/large/store_buttons3.rttex||0|4|0|0|";
					string text6 = "|\nadd_button|itempack_menu|Item Packs|interface/large/store_buttons3.rttex||0|3|0|0|";
					string text7 = "|\nadd_button|bigitems_menu|Awesome Items|interface/large/store_buttons4.rttex||0|6|0|0|";
					string text8 = "|\nadd_button|weather_menu|Weather Machines|interface/large/store_buttons5.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|4|0|0||";
					string text9 = "|\nadd_button|token_menu|Growtoken Items|interface/large/store_buttons9.rttex||0|0|0|0|";

					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22);
				}*/
					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22);
				}
				if (cch.find("action|buy\nitem|locks") == 0) {
					string text1 = "set_description_text|`2Locks And Stuff!``  Select the item you'd like more info on, or BACK to go back.";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|store_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||1|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_button|upgrade_backpack|`0Upgrade Backpack`` (`w10 Slots``)|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 10 Additional Backpack Slots.<CR><CR>`5Description:`` Sewing an extra pocket onto your backpack will allow you to store `$10`` additional item types.  How else are you going to fit all those toilets and doors?|0|1|3700|0|||-1|-1||-1|-1||1|||||||";
					string text10 = "|\nadd_button|rename|`oBirth Certificate``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Birth Certificate.<CR><CR>`5Description:`` Tired of being who you are? By forging a new birth certificate, you can change your GrowID! The Birth Certificate will be consumed when used. This item only works if you have a GrowID, and you can only use one every 60 days, so you're not confusing everybody.|0|6|20000|0|||-1|-1||-1|-1||1|||||||";
					string text11 = "|\nadd_button|clothes|`oClothes Pack``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 3 Randomly Wearble Items.<CR><CR>`5Description:`` Why not look the part? Some may even have special powers...|0|0|50|0|||-1|-1||-1|-1||1|||||||";
					string text12 = "|\nadd_button|rare_clothes|`oRare Clothes Pack``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 3 Randomly Chosen Wearbale Items.<CR><CR>`5Description:`` Enjoy the garb of kings! Some may even have special powers...|0|1|500|0|||-1|-1||-1|-1||1|||||||";
					string text13 = "|\nadd_button|transmutation_device|`oTransmutabooth``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Transmutabooth.<CR><CR>`5Description:`` Behold! A wondrous technological achievement from the innovative minds at GrowTech, the Transmutabooth allows you to merge clothing items, transferring the visual appearance of one onto another in the same slot! If you've ever wanted your Cyclopean Visor to look like Shades (while keeping its mod), now you can!|0|7|25000|0|||-1|-1||-1|-1||1|||||||";
					string text14 = "|\nadd_button|contact_lenses|`oContact Lens Pack``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 10 Random Contact Lens Colors.<CR><CR>`5Description:`` Need a colorful new look? This pack includes 10 random Contact Lens colors (and may include Contact Lens Cleaning Solution, to return to your natural eye color)!|0|7|15000|0|||-1|-1||-1|-1||1|||||||";
					string text15 = "|\nadd_button|eye_drops|`oEye Drop Pack``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 `#Rare Bathroom Mirror`` and 10 random Eye Drop Colors.<CR><CR>`5Description:`` Need a fresh new look?  This pack includes a 10 random Eye Drop Colors (may include Eye Cleaning Solution, to leave your eyes sparkly clean)!|0|6|30000|0|||-1|-1||-1|-1||1|||||||";
					string text16 = "|\nadd_button|nyan_hat|`oTurtle Hat``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 1 Turtle Hat.<CR><CR>`5Description:`` It's the greatest hat ever. It bloops out bubbles as you run! `4Not available any other way!``|0|2|25000|0|||-1|-1||-1|-1||1|||||||";
					string text17 = "|\nadd_button|tiny_horsie|`oTiny Horsie``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 1 Tiny Horsie.<CR><CR>`5Description:`` Tired of wearing shoes? Wear a Tiny Horsie instead! Or possibly a large dachshund, we're not sure. Regardless, it lets you run around faster than normal, plus you're on a horse! `4Not available any other way!``|0|5|25000|0|||-1|-1||-1|-1||1|||||||";
					string text18 = "|\nadd_button|star_ship|`oPleiadian Star Ship``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 1 Pleiadian Star Ship.<CR><CR>`5Description:`` Float on, my brother. It's all groovy. This star ship can't fly, but you can still zoom around in it, leaving a trail of energy rings and moving at enhanced speed. Sponsored by Pleiadian. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_button|dragon_hand|`oDragon Hand``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Dragon Hand.<CR><CR>`5Description:`` Call forth the dragons of legend!  With the Dragon Hand, you will command your own pet dragon. Instead of punching blocks or players, you can order your dragon to incinerate them! In addition to just being awesome, this also does increased damage, and pushes other players farther. `4Not available any other way!``|0|1|50000|0|||-1|-1||-1|-1||1|||||||";
					string text20 = "|\nadd_button|corvette|`oLittle Red Corvette``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Little Red Corvette.<CR><CR>`5Description:`` Cruise around the neighborhood in style with this sweet convertible. It moves at enhanced speed and leaves other Growtopians in your dust. `4Not available any other way!``|0|1|25000|0|||-1|-1||-1|-1||1|||||||";
					string text21 = "|\nadd_button|stick_horse|`oStick Horse``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Stick Horse.<CR><CR>`5Description:`` Nobody looks cooler than a person bouncing along on a stick with a fake horse head attached. NOBODY. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1|||||||";
					string text22 = "|\nadd_button|ambulance|`oAmbulance``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Ambulance.<CR><CR>`5Description:`` Rush to the scene of an accident while lawyers chase you in this speedy rescue vehicle. `4Not available any other way!``|0|3|25000|0|||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nadd_button|raptor|`oRiding Raptor``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Riding Raptor.<CR><CR>`5Description:`` Long thought to be extinct, it turns out that these dinosaurs are actually alive and easily tamed. And riding one lets you run around faster than normal! `4Not available any other way!``|0|7|25000|0|||-1|-1||-1|-1||1|||||||";
					string text24 = "|\nadd_button|owl|`oMid-Pacific Owl``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Mid-Pacific Owl.<CR><CR>`5Description:`` This owl is a bit lazy - if you stop moving around, he'll land on your head and fall asleep. Dedicated to the students of the Mid-Pacific Institute. `4Not available any other way!``|0|1|30000|0|||-1|-1||-1|-1||1|||||||";
					string text25 = "|\nadd_button|unicorn|`oUnicorn Garland``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Unicorn Garland.<CR><CR>`5Description:`` Prance about in the fields with your very own pet unicorn! It shoots `1R`2A`3I`4N`5B`6O`7W`8S``. `4Not available any other way!``|0|4|50000|0|||-1|-1||-1|-1||1|||||||";
					string text26 = "|\nadd_button|starboard|`oStarBoard``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 StarBoard.<CR><CR>`5Description:`` Hoverboards are here at last! Zoom around Growtopia on this brand new model, which is powered by fusion energy (that means stars spit out of the bottom). Moves faster than walking. Sponsored by Miwsky, Chudy, and Dawid. `4Not available any other way!``|0|1|30000|0|||-1|-1||-1|-1||1|||||||";
					string text27 = "|\nadd_button|motorcycle|`oGrowley Motorcycle``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Growley Motorcycle.<CR><CR>`5Description:`` The coolest motorcycles available are Growley Dennisons. Get a sporty blue one today! It even moves faster than walking, which is pretty good for a motorcycle. `4Not available any other way!``|0|6|50000|0|||-1|-1||-1|-1||1|||||||";
					string text28 = "|\nadd_button|monkey_on_back|`oMonkey On Your Back``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Monkey On Your Back.<CR><CR>`5Description:`` Most people work really hard to get rid of these, but hey, if you want one, it's available! `4But not available any other way!`` Sponsored by SweGamerHD's subscribers, Kizashi, and Inforced. `#Note: This is a neck item, not a back item. He's grabbing your neck!``|0|2|50000|0|||-1|-1||-1|-1||1|||||||";
					string text29 = "|\nadd_button|carrot_sword|`oCarrot Sword``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Carrot Sword.<CR><CR>`5Description:`` Razor sharp, yet oddly tasty. This can carve bunny symbols into your foes! `4Not available any other way!`` Sponsored by MrMehMeh.|0|3|15000|0|||-1|-1||-1|-1||1|||||||";
					string text30 = "|\nadd_button|red_bicycle|`oRed Bicycle``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Red Bicycle.<CR><CR>`5Description:`` It's the environmentally friendly way to get around! Ride this bicycle at high speed hither and zither throughout Growtopia. `4Not available any other way!``|0|5|30000|0|||-1|-1||-1|-1||1|||||||";
					string text31 = "|\nadd_button|fire_truck|`oFire Truck``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Fire Truck.<CR><CR>`5Description:`` Race to the scene of the fire in this speedy vehicle! `4Not available any other way!``|0|2|50000|0|||-1|-1||-1|-1||1|||||||";
					string text32 = "|\nadd_button|pet_slime|`oPet Slime``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Pet Slime.<CR><CR>`5Description:`` What could be better than a blob of greasy slime that follows you around? How about a blob of greasy slime that follows you around and spits corrosive acid, melting blocks more quickly than a normal punch? `4Not available any other way!``|0|4|100000|0|||-1|-1||-1|-1||1|||||||";
					string text33 = "|\nadd_button|dabstep_shoes|`oDabstep Low Top Sneakers``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Dabstep Low Top Sneakers.<CR><CR>`5Description:`` Light up every footfall and move to a better beat with these dabulous shoes! When you're wearing these, the world is your dance floor! `4Not available any other way!``|0|2|30000|0|||-1|-1||-1|-1||1|||||||";


					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23 + text24 + text25 + text26 + text27 + text28 + text29 + text30 + text31 + text32 + text33);

				}
				if (cch.find("action|buy\nitem|itempack") == 0) {
					string text1 = "set_description_text|`2Item Packs!``  Select the item you'd like more info on, or BACK to go back.";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||1|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_button|world_lock|`oWorld Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 World Lock.<CR><CR>`5Description:`` Become the undisputed ruler of your domain with one of these babies.  It works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.``  `wRecycles for 200 Gems.``|0|7|2000|0|||-1|-1||-1|-1||1|||||||";
					string text10 = "|\nadd_button|10_wl|`oWorld Lock Pack``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 10 World Locks.<CR><CR>`5Description:`` 10-pack of World Locks. Become the undisputed ruler of up to TEN kingdoms with these babies. Each works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.`` `wEach recycles for 200 Gems.``|0|3|20000|0|||-1|-1||-1|-1||1|||||||";
					string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1|||||||";
					string text12 = "|\nadd_button|small_lock|`oSmall Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Small Lock.<CR><CR>`5Description:`` Protect up to `$10`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|1|3|50|0|||-1|-1||-1|-1||1|||||||";
					string text13 = "|\nadd_button|big_lock|`oBig Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Big Lock.<CR><CR>`5Description:`` Protect up to `$48`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|1|1|200|0|||-1|-1||-1|-1||1|||||||";
					string text14 = "|\nadd_button|huge_lock|`oHuge Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Huge Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|0|4|500|0|||-1|-1||-1|-1||1|||||||";
					string text15 = "|\nadd_button|door_pack|`oDoor And Sign Hello Pack``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Door and 1 Sign.<CR><CR>`5Description:`` Own your very own door and sign! This pack comes with one of each. Leave cryptic messages and create a door that can open to, well, anywhere.|0|3|15|0|||-1|-1||-1|-1||1|||||||";
					string text16 = "|\nadd_button|door_mover|`oDoor Mover``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Door Mover.<CR><CR>`5Description:`` Unsatisfied with your world's layout?  This one-use device can be used to move the White Door to any new location in your world, provided there are 2 empty spaces for it to fit in. Disappears when used. `2Only usable on a world you have World Locked.``|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text17 = "|\nadd_button|vending_machine|`oVending Machine``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Vending Machine.<CR><CR>`5Description:`` Tired of interacting with human beings? Try a Vending Machine! You can put a stack of items inside it, set a price in World Locks, and people can buy from the machine while you sit back and rake in the profits! `5It's a perma-item, is never lost when destroyed, and it is not available any other way.``|0|6|8000|0|||-1|-1||-1|-1||1|||||||";
					string text18 = "|\nadd_button|digi_vend|`oDigiVend Machine``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 DigiVend Machine.<CR><CR>`5Description:`` Get with the times and go digital! This wired vending machine can connect its contents to Vending Hubs AND the multiversal economy, providing a unified shopping experience along with price checks to help you sell your goods! All that, and still no human-related hassle! Use your wrench on this to stock it with an item and set a price in World Locks. Other players will be able to buy from it! Only works in World-Locked worlds.|0|2|12000|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_button|checkout_counter|`oVending Hub - Checkout Counter``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 Vending Hub.<CR><CR>`5Description:`` Your one-stop shop! This vending hub will collect and display (and let shoppers buy) the contents of ALL DigiVends in its row or column (wrench it to set which the direction)! Wow! Now that's a shopping experience we can all enjoy! Note: Only works in World-Locked worlds.|0|3|50000|0|||-1|-1||-1|-1||1|||||||";
					string text20 = "|\nadd_button|change_addr|`oChange of Address``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Change of Address.<CR><CR>`5Description:`` Don't like the name of your world? You can use up one of these to trade your world's name with the name of any other world that you own. You must have a `2World Lock`` in both worlds. Go lock up that empty world with the new name you want and swap away!|0|6|20000|0|||-1|-1||-1|-1||1|||||||";
					string text21 = "|\nadd_button|signal_jammer|`oSignal Jammer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Signal Jammer.<CR><CR>`5Description:`` Get off the grid! Install a `$Signal Jammer``! A single punch will cause it to whir to life, tireless hiding your world and its population from pesky snoopers - only those who know the world name will be able to enter. `5It's a perma-item, is never lost when destroyed.``|1|6|2000|0|||-1|-1||-1|-1||1|||||||";
					string text22 = "|\nadd_button|punch_jammer|`oPunch Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Punch Jammer.<CR><CR>`5Description:`` Tired of getting bashed around? Set up a Punch Jammer in your world, and people won't be able to punch each other! Can be turned on and off as needed. `5It's a perma-item, is never lost when destroyed.``|0|4|15000|0|||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nadd_button|zombie_jammer|`oZombie Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Zombie Jammer.<CR><CR>`5Description:`` Got a parkour or race that you don't want slowed down? Turn this on and nobody can be infected by zombie bites in your world. It does not prevent direct infection by the g-Virus itself though. `5It's a perma-item, is never lost when destroyed.``|0|5|15000|0|||-1|-1||-1|-1||1|||||||";
					string text24 = "|\nadd_button|starship_blast|`oImperial Starship Blast``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Imperial Starship Blast.<CR><CR>`5Description:`` Command your very own Starship and explore the cosmos! This blast contains one of 3 possible Imperial ship types - which will you get? Note: Each Starship comes with a full tank of gas, an Imperial Helm - Mk. I, Imperial Reactor - Mk. I and an Imperial Viewscreen - Mk. I, so you'll be all set for your adventure among the stars! Note: A Starship also comes with an assortment of space-age blocks!|0|1|20000|0|||-1|-1||-1|-1||1|||||||";
					string text25 = "|\nadd_button|surg_blast|`oSurgWorld Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 SurgWorld Blast and 1 Caduceaxe.<CR><CR>`5Description:`` Your gateway to a world of medical wonders! SurgWorld is a place of care and healing, with all kinds of interesting blocks, top tips on how to treat people with surgery, and an increased chance of getting maladies while you work! Also comes with 1 Caduceaxe to extract Vaccine Drops from blocks. `6Warning:`` May break when extracting vaccine.|0|2|10000|0|||-1|-1||-1|-1||1|||||||";
					string text26 = "|\nadd_button|bountiful_blast|`oBountiful Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Bountiful Blast.<CR><CR>`5Description:`` Enter a world of fertile soil, cheerful sunshine and lush green hills, and bountiful new trees! This blast is your ticket to a different kind of farming!|0|3|5000|0|||-1|-1||-1|-1||1|||||||";
					string text27 = "|\nadd_button|thermo_blast|`oThermonuclear Blast``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Thermonuclear Blast.<CR><CR>`5Description:`` This supervillainous device will blast you to a new world that has been scoured completely empty - it contains nothing but Bedrock and a White Door. Remember: When using this, you are creating a NEW world by typing in a new name. It would be irresponsible to let you blow up an entire existing world.|0|5|15000|0|||-1|-1||-1|-1||1|||||||";
					string text28 = "|\nadd_button|antigravity_generator|`oAntigravity Generator``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Antigravity Generator.<CR><CR>`5Description:`` Disables gravity in your world when activated! Well, it reduces gravity, and lets everybody jump as much as they want! `5It's a perma-item - never lost when destroyed! `4Not available any other way!````|0|3|450000|0|||-1|-1||-1|-1||1|||||||";
					string text29 = "|\nadd_button|building_blocks_machine|`oBuilding Blocks Machine``|interface/large/store_buttons/store_buttons26.rttex|`2You Get:`` 1 Building Blocks Machine.<CR><CR>`5Description:`` Eager to add some new building materials to your construction stockpile? Tired of collecting them from random worlds and weirdos? Well, pop this beauty in your world and it'll start cranking out awesome blocks in no time! Contains the `5RARE Creepy Baby Block and Digital Dirt`` amongst a heap of other new blocks! Careful, though - blocks don't just come from nothing, and this machine will eventually run out of power once it makes a bunch!|0|3|8000|0|||-1|-1||-1|-1||1|||||||";
					string text30 = "|\nadd_button|builders_lock|`oBuilder's Lock``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Builders Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles. Wrench the lock to limit it - it can either only allow building, or only allow breaking! `5It's a perma-item, is never lost when destroyed.``|0|2|50000|0|||-1|-1||-1|-1||1|||||||";
					string text31 = "|\nadd_button|weather_sunny|`oWeather Machine - Sunny``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Sunny.<CR><CR>`5Description:`` You probably don't need this one... but if you ever have a desire to turn a sunset or desert world back to normal, grab a Sunny Weather Machine to restore the default Growtopia sky! `5It's a perma-item, is never lost when destroyed.``|0|5|1000|0|||-1|-1||-1|-1||1|||||||";
					string text32 = "|\nadd_button|weather_night|`oWeather Machine - Night``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Night.<CR><CR>`5Description:`` You might not call it weather, but we do! This will turn the background of your world into a lovely night scene with stars and moon. `5It's a perma-item, is never lost when destroyed.``|0|6|10000|0|||-1|-1||-1|-1||1|||||||";
					string text33 = "|\nadd_button|weather_arid|`oWeather Machine - Arid``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Arid.<CR><CR>`5Description:`` Want your world to look like a cartoon desert? This will turn the background of your world into a desert scene with all the trimmings. `5It's a perma-item, is never lost when destroyed.``|0|7|10000|0|||-1|-1||-1|-1||1|||||||";
					string text34 = "|\nadd_button|weather_rainy|`oWeather Machine - Rainy City``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Weather Machine - Rainy City.<CR><CR>`5Description:`` This will turn the background of your world into a dark, rainy city scene complete with sound effects. `5It's a perma-item, is never lost when destroyed.``|0|5|10000|0|||-1|-1||-1|-1||1|||||||";
					string text35 = "|\nadd_button|weather_warp|`oWeather Machine - Warp Speed``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Weather Machine - Warp Speed.<CR><CR>`5Description:`` This Weather Machine will launch your world through space at relativistic speeds, which will cause you to age more slowly, as well as see stars flying by rapidly in the background. `5It's a perma-item, is never lost when destroyed.``|0|3|10000|0|||-1|-1||-1|-1||1|||||||";
					string text36 = "|\nadd_button|mars_blast|`oMars Blast``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Mars Blast.<CR><CR>`5Description:`` Blast off to Mars!  This powerful rocket ship will launch you to a new world set up like the surface of Mars, with a special martian sky background, and unique terrain not found elsewhere in the solar system. Mars even has lower gravity than Growtopia normally does! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to Mars, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1|||||||";
					string text37 = "|\nadd_button|undersea_blast|`oUndersea Blast``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Undersea Blast.<CR><CR>`5Description:`` Explore the ocean!  This advanced device will terraform a new world set up like the bottom of the ocean, with a special ocean background, and special blocks like Seaweed, Coral, Jellyfish, Sharks, and maybe a special surprise... Remember, by using this you are creating a NEW world by typing in a new name. You can't convert an existing world to an ocean, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1|||||||";
					string text38 = "|\nadd_button|cave_blast|`oCave Blast``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Cave Blast.<CR><CR>`5Description:`` This explosive device will punch a hole in the ground, giving you a dark cavern to explore. There are even rumors of treasure and the entrance to ancient mines, hidden deep in the caves... but make sure you bring a World Lock. The blasted world is not locked when it's created, so lock it before somebody shows up! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to a cave, that would be dangerous.|0|2|30000|0|||-1|-1||-1|-1||1|||||||";
					string text39 = "|\nadd_button|weather_stuff|`oWeather Machine - Stuff``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Weather Machine - Stuff.<CR><CR>`5Description:`` This is the most fun weather imaginable - Choose any item from your inventory, adjust some settings, and watch it rain down from the sky! Or up, if you prefer reversing the gravity. `5It's a perma-item, is never lost when destroyed.``|0|6|50000|0|||-1|-1||-1|-1||1|||||||";
					string text40 = "|\nadd_button|weather_jungle|`oWeather Machine - Jungle``|interface/large/store_buttons/store_buttons16.rttex|`2You Get:`` 1 Weather Machine - Jungle.<CR><CR>`5Description:`` This weather machine will turn the background of your world into a steamy jungle. `5It's a perma-item, is never lost when destroyed.``|0|5|20000|0|||-1|-1||-1|-1||1|||||||";
					string text41 = "|\nadd_button|weather_backgd|`oWeather Machine - Background``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Weather Machine - Background.<CR><CR>`5Description:`` This amazing device can scan any Background Block, and will make your entire world look like it's been filled with that block. Also handy for hiding music notes! `5It's a perma-item, is never lost when destroyed.``|0|1|150000|0|||-1|-1||-1|-1||1|||||||";
					string text42 = "|\nadd_button|digital_rain_weather|`oWeather Machine - Digital Rain``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 1 Weather Machine - Digital Rain.<CR><CR>`5Description:`` Take the grow pill, and we'll show you how deep the rabbit hole goes! Splash the scrolling code of creation across the skies of your worlds. They say you learn to understand it after a while... Note: You can only have one of these per world. `5It's a perma-item, is never lost when destroyed.``|0|6|30000|0|||-1|-1||-1|-1||1|||||||";
					string text43 = "|\nadd_button|treasure_blast|`oTreasure Blast``|interface/large/store_buttons/store_buttons26.rttex|`2You Get:`` 1 Treasure Blast.<CR><CR>`5Description:`` Enter a world of snow-capped peaks and long-forgotten mysteries! Riddles and secrets - and a ton of treasure - await those who brave this blast's blocks! Remember, when you use this, it'll create a new world by typing in a new name! No sense in searching for clues to great treasures in well-trod worlds, is there?|0|6|10000|0|||-1|-1||-1|-1||1|||||||";


					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23 + text24 + text25 + text26 + text27 + text28 + text29 + text30 + text31 + text32 + text33 + text34 + text35 + text36 + text37 + text38 + text39 + text40 + text41 + text42 + text43);

				}
				if (cch.find("action|buy\nitem|bigitems") == 0) {
					string text1 = "set_description_text|`2Item Packs!``  Select the item you'd like more info on, or BACK to go back.";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||1|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_button|world_lock|`oWorld Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 World Lock.<CR><CR>`5Description:`` Become the undisputed ruler of your domain with one of these babies.  It works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.``  `wRecycles for 200 Gems.``|0|7|2000|0|||-1|-1||-1|-1||1|||||||";
					string text10 = "|\nadd_button|10_wl|`oWorld Lock Pack``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 10 World Locks.<CR><CR>`5Description:`` 10-pack of World Locks. Become the undisputed ruler of up to TEN kingdoms with these babies. Each works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.`` `wEach recycles for 200 Gems.``|0|3|20000|0|||-1|-1||-1|-1||1|||||||";
					string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1|||||||";
					string text12 = "|\nadd_button|small_lock|`oSmall Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Small Lock.<CR><CR>`5Description:`` Protect up to `$10`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|1|3|50|0|||-1|-1||-1|-1||1|||||||";
					string text13 = "|\nadd_button|big_lock|`oBig Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Big Lock.<CR><CR>`5Description:`` Protect up to `$48`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|1|1|200|0|||-1|-1||-1|-1||1|||||||";
					string text14 = "|\nadd_button|huge_lock|`oHuge Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Huge Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``|0|4|500|0|||-1|-1||-1|-1||1|||||||";
					string text15 = "|\nadd_button|door_pack|`oDoor And Sign Hello Pack``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Door and 1 Sign.<CR><CR>`5Description:`` Own your very own door and sign! This pack comes with one of each. Leave cryptic messages and create a door that can open to, well, anywhere.|0|3|15|0|||-1|-1||-1|-1||1|||||||";
					string text16 = "|\nadd_button|door_mover|`oDoor Mover``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Door Mover.<CR><CR>`5Description:`` Unsatisfied with your world's layout?  This one-use device can be used to move the White Door to any new location in your world, provided there are 2 empty spaces for it to fit in. Disappears when used. `2Only usable on a world you have World Locked.``|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text17 = "|\nadd_button|vending_machine|`oVending Machine``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Vending Machine.<CR><CR>`5Description:`` Tired of interacting with human beings? Try a Vending Machine! You can put a stack of items inside it, set a price in World Locks, and people can buy from the machine while you sit back and rake in the profits! `5It's a perma-item, is never lost when destroyed, and it is not available any other way.``|0|6|8000|0|||-1|-1||-1|-1||1|||||||";
					string text18 = "|\nadd_button|digi_vend|`oDigiVend Machine``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 DigiVend Machine.<CR><CR>`5Description:`` Get with the times and go digital! This wired vending machine can connect its contents to Vending Hubs AND the multiversal economy, providing a unified shopping experience along with price checks to help you sell your goods! All that, and still no human-related hassle! Use your wrench on this to stock it with an item and set a price in World Locks. Other players will be able to buy from it! Only works in World-Locked worlds.|0|2|12000|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_button|checkout_counter|`oVending Hub - Checkout Counter``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 Vending Hub.<CR><CR>`5Description:`` Your one-stop shop! This vending hub will collect and display (and let shoppers buy) the contents of ALL DigiVends in its row or column (wrench it to set which the direction)! Wow! Now that's a shopping experience we can all enjoy! Note: Only works in World-Locked worlds.|0|3|50000|0|||-1|-1||-1|-1||1|||||||";
					string text20 = "|\nadd_button|change_addr|`oChange of Address``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Change of Address.<CR><CR>`5Description:`` Don't like the name of your world? You can use up one of these to trade your world's name with the name of any other world that you own. You must have a `2World Lock`` in both worlds. Go lock up that empty world with the new name you want and swap away!|0|6|20000|0|||-1|-1||-1|-1||1|||||||";
					string text21 = "|\nadd_button|signal_jammer|`oSignal Jammer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Signal Jammer.<CR><CR>`5Description:`` Get off the grid! Install a `$Signal Jammer``! A single punch will cause it to whir to life, tireless hiding your world and its population from pesky snoopers - only those who know the world name will be able to enter. `5It's a perma-item, is never lost when destroyed.``|1|6|2000|0|||-1|-1||-1|-1||1|||||||";
					string text22 = "|\nadd_button|punch_jammer|`oPunch Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Punch Jammer.<CR><CR>`5Description:`` Tired of getting bashed around? Set up a Punch Jammer in your world, and people won't be able to punch each other! Can be turned on and off as needed. `5It's a perma-item, is never lost when destroyed.``|0|4|15000|0|||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nadd_button|zombie_jammer|`oZombie Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Zombie Jammer.<CR><CR>`5Description:`` Got a parkour or race that you don't want slowed down? Turn this on and nobody can be infected by zombie bites in your world. It does not prevent direct infection by the g-Virus itself though. `5It's a perma-item, is never lost when destroyed.``|0|5|15000|0|||-1|-1||-1|-1||1|||||||";
					string text24 = "|\nadd_button|starship_blast|`oImperial Starship Blast``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Imperial Starship Blast.<CR><CR>`5Description:`` Command your very own Starship and explore the cosmos! This blast contains one of 3 possible Imperial ship types - which will you get? Note: Each Starship comes with a full tank of gas, an Imperial Helm - Mk. I, Imperial Reactor - Mk. I and an Imperial Viewscreen - Mk. I, so you'll be all set for your adventure among the stars! Note: A Starship also comes with an assortment of space-age blocks!|0|1|20000|0|||-1|-1||-1|-1||1|||||||";
					string text25 = "|\nadd_button|surg_blast|`oSurgWorld Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 SurgWorld Blast and 1 Caduceaxe.<CR><CR>`5Description:`` Your gateway to a world of medical wonders! SurgWorld is a place of care and healing, with all kinds of interesting blocks, top tips on how to treat people with surgery, and an increased chance of getting maladies while you work! Also comes with 1 Caduceaxe to extract Vaccine Drops from blocks. `6Warning:`` May break when extracting vaccine.|0|2|10000|0|||-1|-1||-1|-1||1|||||||";
					string text26 = "|\nadd_button|bountiful_blast|`oBountiful Blast``|interface/large/store_buttons/store_buttons27.rttex|`2You Get:`` 1 Bountiful Blast.<CR><CR>`5Description:`` Enter a world of fertile soil, cheerful sunshine and lush green hills, and bountiful new trees! This blast is your ticket to a different kind of farming!|0|3|5000|0|||-1|-1||-1|-1||1|||||||";
					string text27 = "|\nadd_button|thermo_blast|`oThermonuclear Blast``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Thermonuclear Blast.<CR><CR>`5Description:`` This supervillainous device will blast you to a new world that has been scoured completely empty - it contains nothing but Bedrock and a White Door. Remember: When using this, you are creating a NEW world by typing in a new name. It would be irresponsible to let you blow up an entire existing world.|0|5|15000|0|||-1|-1||-1|-1||1|||||||";
					string text28 = "|\nadd_button|antigravity_generator|`oAntigravity Generator``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Antigravity Generator.<CR><CR>`5Description:`` Disables gravity in your world when activated! Well, it reduces gravity, and lets everybody jump as much as they want! `5It's a perma-item - never lost when destroyed! `4Not available any other way!````|0|3|450000|0|||-1|-1||-1|-1||1|||||||";
					string text29 = "|\nadd_button|building_blocks_machine|`oBuilding Blocks Machine``|interface/large/store_buttons/store_buttons26.rttex|`2You Get:`` 1 Building Blocks Machine.<CR><CR>`5Description:`` Eager to add some new building materials to your construction stockpile? Tired of collecting them from random worlds and weirdos? Well, pop this beauty in your world and it'll start cranking out awesome blocks in no time! Contains the `5RARE Creepy Baby Block and Digital Dirt`` amongst a heap of other new blocks! Careful, though - blocks don't just come from nothing, and this machine will eventually run out of power once it makes a bunch!|0|3|8000|0|||-1|-1||-1|-1||1|||||||";
					string text30 = "|\nadd_button|builders_lock|`oBuilder's Lock``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Builders Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles. Wrench the lock to limit it - it can either only allow building, or only allow breaking! `5It's a perma-item, is never lost when destroyed.``|0|2|50000|0|||-1|-1||-1|-1||1|||||||";
					string text31 = "|\nadd_button|weather_sunny|`oWeather Machine - Sunny``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Sunny.<CR><CR>`5Description:`` You probably don't need this one... but if you ever have a desire to turn a sunset or desert world back to normal, grab a Sunny Weather Machine to restore the default Growtopia sky! `5It's a perma-item, is never lost when destroyed.``|0|5|1000|0|||-1|-1||-1|-1||1|||||||";
					string text32 = "|\nadd_button|weather_night|`oWeather Machine - Night``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Night.<CR><CR>`5Description:`` You might not call it weather, but we do! This will turn the background of your world into a lovely night scene with stars and moon. `5It's a perma-item, is never lost when destroyed.``|0|6|10000|0|||-1|-1||-1|-1||1|||||||";
					string text33 = "|\nadd_button|weather_arid|`oWeather Machine - Arid``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Weather Machine - Arid.<CR><CR>`5Description:`` Want your world to look like a cartoon desert? This will turn the background of your world into a desert scene with all the trimmings. `5It's a perma-item, is never lost when destroyed.``|0|7|10000|0|||-1|-1||-1|-1||1|||||||";
					string text34 = "|\nadd_button|weather_rainy|`oWeather Machine - Rainy City``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Weather Machine - Rainy City.<CR><CR>`5Description:`` This will turn the background of your world into a dark, rainy city scene complete with sound effects. `5It's a perma-item, is never lost when destroyed.``|0|5|10000|0|||-1|-1||-1|-1||1|||||||";
					string text35 = "|\nadd_button|weather_warp|`oWeather Machine - Warp Speed``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Weather Machine - Warp Speed.<CR><CR>`5Description:`` This Weather Machine will launch your world through space at relativistic speeds, which will cause you to age more slowly, as well as see stars flying by rapidly in the background. `5It's a perma-item, is never lost when destroyed.``|0|3|10000|0|||-1|-1||-1|-1||1|||||||";
					string text36 = "|\nadd_button|mars_blast|`oMars Blast``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Mars Blast.<CR><CR>`5Description:`` Blast off to Mars!  This powerful rocket ship will launch you to a new world set up like the surface of Mars, with a special martian sky background, and unique terrain not found elsewhere in the solar system. Mars even has lower gravity than Growtopia normally does! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to Mars, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1|||||||";
					string text37 = "|\nadd_button|undersea_blast|`oUndersea Blast``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Undersea Blast.<CR><CR>`5Description:`` Explore the ocean!  This advanced device will terraform a new world set up like the bottom of the ocean, with a special ocean background, and special blocks like Seaweed, Coral, Jellyfish, Sharks, and maybe a special surprise... Remember, by using this you are creating a NEW world by typing in a new name. You can't convert an existing world to an ocean, that would be dangerous.|0|7|15000|0|||-1|-1||-1|-1||1|||||||";
					string text38 = "|\nadd_button|cave_blast|`oCave Blast``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Cave Blast.<CR><CR>`5Description:`` This explosive device will punch a hole in the ground, giving you a dark cavern to explore. There are even rumors of treasure and the entrance to ancient mines, hidden deep in the caves... but make sure you bring a World Lock. The blasted world is not locked when it's created, so lock it before somebody shows up! Remember: When using this, you are creating a NEW world by typing in a new name. You can't convert an existing world to a cave, that would be dangerous.|0|2|30000|0|||-1|-1||-1|-1||1|||||||";
					string text39 = "|\nadd_button|weather_stuff|`oWeather Machine - Stuff``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Weather Machine - Stuff.<CR><CR>`5Description:`` This is the most fun weather imaginable - Choose any item from your inventory, adjust some settings, and watch it rain down from the sky! Or up, if you prefer reversing the gravity. `5It's a perma-item, is never lost when destroyed.``|0|6|50000|0|||-1|-1||-1|-1||1|||||||";
					string text40 = "|\nadd_button|weather_jungle|`oWeather Machine - Jungle``|interface/large/store_buttons/store_buttons16.rttex|`2You Get:`` 1 Weather Machine - Jungle.<CR><CR>`5Description:`` This weather machine will turn the background of your world into a steamy jungle. `5It's a perma-item, is never lost when destroyed.``|0|5|20000|0|||-1|-1||-1|-1||1|||||||";
					string text41 = "|\nadd_button|weather_backgd|`oWeather Machine - Background``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Weather Machine - Background.<CR><CR>`5Description:`` This amazing device can scan any Background Block, and will make your entire world look like it's been filled with that block. Also handy for hiding music notes! `5It's a perma-item, is never lost when destroyed.``|0|1|150000|0|||-1|-1||-1|-1||1|||||||";
					string text42 = "|\nadd_button|digital_rain_weather|`oWeather Machine - Digital Rain``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 1 Weather Machine - Digital Rain.<CR><CR>`5Description:`` Take the grow pill, and we'll show you how deep the rabbit hole goes! Splash the scrolling code of creation across the skies of your worlds. They say you learn to understand it after a while... Note: You can only have one of these per world. `5It's a perma-item, is never lost when destroyed.``|0|6|30000|0|||-1|-1||-1|-1||1|||||||";
					string text43 = "|\nadd_button|treasure_blast|`oTreasure Blast``|interface/large/store_buttons/store_buttons26.rttex|`2You Get:`` 1 Treasure Blast.<CR><CR>`5Description:`` Enter a world of snow-capped peaks and long-forgotten mysteries! Riddles and secrets - and a ton of treasure - await those who brave this blast's blocks! Remember, when you use this, it'll create a new world by typing in a new name! No sense in searching for clues to great treasures in well-trod worlds, is there?|0|6|10000|0|||-1|-1||-1|-1||1|||||||";


					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23 + text24 + text25 + text26 + text27 + text28 + text29 + text30 + text31 + text32 + text33 + text34 + text35 + text36 + text37 + text38 + text39 + text40 + text41 + text42 + text43);

				}
				if (cch.find("action|buy\nitem|weather") == 0) {
					string text1 = "set_description_text|`2Weather Machines!``  Select the item you'd like more info on, or BACK to go back.";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|1|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_button|race_pack|`oRacing Action Pack``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 1 Racing Start Flag, 1 Racing End Flag, 2 Checkpoints, 2 Big Old Sideways Arrows, 1 Big Old Up Arrow, 1 Big Old Down Arrow, 1 WristBand, 1 HeadBand, 1 Sports Ball Jersey and 1 Air Robinsons.<CR><CR>`5Description:`` Get all you need to host races in your worlds! You'll win the races too, with new Air Robinsons that make you run faster!|0|7|3500|0|||-1|-1||-1|-1||1|||||||";
					string text10 = "|\nadd_button|music_pack|`oComposer's Pack``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 20 Sheet Music: Blank, 20 Sheet Music: Piano Note, 20 Sheet Music: Bass Note, 20 Sheet Music Drums, 5 Sheet Music: Sharp Piano, 5 Sheet Music: Flat Piano, 5 Sheet Music: Flat Bass and 5 Sheet Music: Sharp Bass .<CR><CR>`5Description:`` With these handy blocks, you'll be able to compose your own music, using your World-Locked world as a sheet of music. Requires a World Lock (sold separately!).|0|0|5000|0|||-1|-1||-1|-1||1|||||||";
					string text11 = "|\nadd_button|school_pack|`oEducation Pack``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 10 ChalkBoards, 3 School Desks, 20 Red Bricks, 1 Bulletin Board, 10 Pencils, 1 Growtopia Lunchbox, 1 Grey Hair Bun, 1 Apple and 1 Random School Uniform Item.<CR><CR>`5Description:`` If you want to build a school in Growtopia, here's what you need!|0|0|5000|0|||-1|-1||-1|-1||1|||||||";
					string text12 = "|\nadd_button|dungeon_pack|`oDungeon Pack``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 20 Grimstone, 20 Blackrock Wall, 20 Iron Bars, 3 Jail Doors, 3 Skeletons, 1 Headsman's Axe, 1 Worthless Rags. 5 Torches and a `#Rare Iron Mask!``.<CR><CR>`5Description:`` Lock up your enemies in a dank dungeon! Of course they can still leave whenever they want. But they won't want to, because it looks so cool! Iron Mask muffles your speech!|0|1|10000|0|||-1|-1||-1|-1||1|||||||";
					string text13 = "|\nadd_button|fantasy_pack|`oFantasy Pack``|interface/large/store_buttons/store_buttons3.rttex|`2You Get:`` 1 Mystical Wizard Hat Seed, 1 Wizards Robe, 1 Golden Sword, 1 Elvish Longbow, 10 Barrels, 3 Tavern Signs, 3 Treasure Chests and 3 Dragon Gates.<CR><CR>`5Description:`` Hear ye, hear ye! It's a pack of magical wonders!|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text14 = "|\nadd_button|vegas_pack|`oVegas Pack``|interface/large/store_buttons/store_buttons4.rttex|`2You Get:`` 10 Neon Lights, 1 Card Block Seed, 1 `#Rare Pink Cadillac`` 4 Flipping Coins, 1 Dice Block, 1 Gamblers Visor, 1 Slot Machine, 1 Roulette Wheel and 1 Showgirl Hat, 1 Showgirl top and 1 Showgirl Leggins.<CR><CR>`5Description:`` What happens in Growtopia stays in Growtopia!|0|5|20000|0|||-1|-1||-1|-1||1|||||||";
					string text15 = "|\nadd_button|farm_pack|`oFarm Pack``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Cow, 1 Chicken, 10 Wheat, 10 Barn Block, 10 Red Wood Walls, 1 Barn Door, 1 Straw Hat, 1 Overalls, 1 Pitchfork, 1 Farmgirl Hair, 1 `#Rare`` `2Dear John Tractor``.<CR><CR>`5Description:`` Put the `2Grow`` in Growtopia with this pack, including a Cow you can milk, a Chicken that lays eggs and a farmer's outfit. Best of all? You get a `#Rare`` `2Dear John Tractor`` you can ride that will mow down trees!|0|0|15000|0|||-1|-1||-1|-1||1|||||||";
					string text16 = "|\nadd_button|science_pack|`oMad Science Kit``|interface/large/store_buttons/store_buttons5.rttex|`2You Get:`` 1 Science Station, 1 Laboratory, 1 LabCoat, 1 Combover Hair, 1 Goggles, 5 Chemical 5, 10 Chemical G, 5 Chemical Y, 5 Chemical B, 5 Chemical P and 1 `#Rare`` `2Death Ray``.<CR><CR>`5Description:`` It's SCIENCE! Defy the natural order with a Science Station that produces chemicals, a Laboratory in which to mix them and a full outfit to do so safely! You'll also get a starter pack of assorted chemicals. Mix them up! Special bonus: A `#Rare`` `2Death Ray`` to make your science truly mad!|0|3|5000|0|||-1|-1||-1|-1||1|||||||";
					string text17 = "|\nadd_button|city_pack|`oCity Pack``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 10 Sidewalks, 3 Street Signs, 3 Streetlamps, 10 Gothic Building tiles, 10 Tenement Building tiles, 10 Fire Escapes, 3 Gargoyles, 10 Hedges, 1 Blue Mailbox, 1 Fire Hydrant and A `#Rare`` `2ATM Machine``.<CR><CR>`5Description:`` Life in the big city is rough but a `#Rare`` `2ATM Machine`` that dishes out gems once a day is very nice!|0|0|8000|0|||-1|-1||-1|-1||1|||||||";
					string text18 = "|\nadd_button|west_pack|`oWild West Pack``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Cowboy Hat, 1 Cowboy Boots, 1 War Paint, 1 Face Bandana, 1 Sheriff Vest, 1 Layer Cake Dress,  1 Corset, 1 Kansas Curls, 10 Western Building 1 Saloon Doors, 5 Western Banners, 1 Buffalo, 10 Rustic Fences, 1 Campfire and 1 Parasol.<CR><CR>`5Description:`` Yippee-kai-yay! This pack includes everything you need to have wild time in the wild west! The Campfire plays cowboy music, and the `#Parasol`` lets you drift down slowly. Special bonus: A `#Rare`` `2Six Shooter`` to blast criminals with!|0|2|8000|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_button|astro_pack|`oAstro Pack``|interface/large/store_buttons/store_buttons6.rttex|`2You Get:`` 1 Astronaut Helmet, 1 Space Suit, 1 Space Pants, 1 Moon Boots, 1 Rocket Thruster, 1 Solar Panel, 6 Space Connectors, 1 Porthole, 1 Compu Panel, 1 Forcefield and 1 `#Rare`` `2Zorbnik DNA``.<CR><CR>`5Description:`` Boldly go where no Growtopian has gone before with an entire Astronaut outfit. As a special bonus, you can have this `#Rare`` `2Zorbnik DNA`` we found on a distant planet. It doesn't do anything by itself, but by trading with your friends, you can collect 10 of them, and then... well, who knows?|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text20 = "|\nadd_button|prehistoric_pack|`oPrehistoric Pack``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Caveman Club, 1 Cave Woman Hair, 1 Caveman Hair, 1 Sabertooth Toga, 1 Fuzzy Bikini Top, 1 Fuzzy Bikni Bottom, 1 Cavewoman Outfit, 10 Cliffside, 5 Rock Platforms, 1 Cave Entrance, 3 Prehistoric Palms and 1 `#Rare Sabertooth Growtopian``.<CR><CR>`5Description:`` Travel way back in time with this pack, including full Caveman and Cavewoman outfits and `#Rare Sabertooth Growtopian`` (that's a mask of sorts). Unleash your inner monster!|0|0|5000|0|||-1|-1||-1|-1||1|||||||";
					string text21 = "|\nadd_button|shop_pack|`oShop Pack``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 4 Display Boxes, 1 For Sale Sign, 1 Gem Sign, 1 Exclamation Sign, 1 Shop Sign, 1 Open Sign, 1 Cash Register, 1 Mannequin and 1 Security Camera.<CR><CR>`5Description:`` Run a fancy shop with these new items! Advertise your wares with an Open/Closed Sign you can switch with a punch, a Cash Register, a Mannequin you can dress up to show off clothing, and a `#Rare`` Security Camera, which reports when people enter and take items!|0|7|10000|0|||-1|-1||-1|-1||1|||||||";
					string text22 = "|\nadd_button|home_pack|`oHome Pack``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Television, 4 Couches, 2 Curtains, 1 Wall Clock, 1 Microwave, 1 Meaty Apron, 1 Ducky Pants, 1 Ducky top and 1 Eggs Benedict.<CR><CR>`5Description:`` Welcome home to Growtopia! Decorate with a Television, Window Curtains, Couches, a `#Rare`` Wall Clock that actually tells time, and a Microwave to cook in. Then dress up in a Meaty Apron and Ducky Pajamas to sit down and eat Eggs Benedict, which increases the amount of XP you earn!|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nadd_button|cinema_pack|`oCinema Pack``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 ClapBoard, 1 Black Beret, 1 3D Glasses, 6 Theater Curtains, 6 Marquess Blocks, 1 Directors Chair, 4 Theater Seats, 6 Movie Screens, 1 Movie Camera and 1 `#Rare GHX Speaker``.<CR><CR>`5Description:`` It's movie time! Everything you need for the big screen experience including a `#Rare GHX Speaker`` that plays the score from Growtopia: The Movie.|0|2|6000|0|||-1|-1||-1|-1||1|||||||";
					string text24 = "|\nadd_button|adventure_pack|`oAdventure Pack``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 4 Gateways to Adventure, 4 Path Markers, 1 Lazy Cobra, 1 Adventure Brazier, 4 Adventure Barriers, 1 Rope, 1 Torch, 1 Key, 1 Golden Idol, 1 `#Rare Adventuring Mustache``, 1 Explorer's Ponytail and 1 Sling Bag .<CR><CR>`5Description:`` Join Dr. Exploro and her father (also technically Dr. Exploro) as they seek out adventure! You can make your own adventure maps with the tools in this pack.|0|7|20000|0|||-1|-1||-1|-1||1|||||||";
					string text25 = "|\nadd_button|rockin_pack|`oRockin' Pack``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 3 `#Rare Musical Instruments`` Including A Keytar, a Bass Guitar and Tambourine, 1 Starchild Make Up, 1 Rockin' Headband, 1 Leopard Leggings, 1 Shredded Ts-Shirt, 1 Drumkit, 6 Stage Supports, 6 Mega Rock Speakers and 6 Rock n' Roll Wallpaper.<CR><CR>`5Description:`` ROCK N' ROLL!!! Play live music in-game! We Formed a Band! Growtopia makes me want to rock out.|0|0|9999|0|||-1|-1||-1|-1||1|||||||";
					string text26 = "|\nadd_button|game_pack|`oGame Pack``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 `#Rare Game Generator``,  4 Game Blocks, 4 Game Flags, 4 Game Graves and 4 Game Goals.<CR><CR>`5Description:`` Growtopia's not all trading and socializing! Create games for your friends with the Game Pack (and a lot of elbow grease).|0|6|50000|0|||-1|-1||-1|-1||1|||||||";
					string text27 = "|\nadd_button|superhero|`oSuperhero Pack``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Mask, 1 Shirt, 1 Boots, 1 Tights, 1 Cape, `#Rare Super Logos`` or `#Rare Utility Belt`` and 1 `2Phone Booth``.<CR><CR>`5Description:`` Battle the criminal element in Growtopia with a complete random superhero outfit including a cape that lets you double jump. Each of these items comes in one of six random colors. You also get one of 5 `#Rare`` Super Logos, which automatically match the color of any shirt you wear or a `#Rare`` Utility Belt... of course use the `2Phone Booth`` to change into your secret identity!|0|0|10000|0|||-1|-1||-1|-1||1|||||||";
					string text28 = "|\nadd_button|fashion_pack|`oFashion Pack``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 3 Random Clothing Items, 3 Jade Blocks and 1 `#Rare Spotlight``.<CR><CR>`5Description:`` The hottest new looks for the season are here now with 3 random Fashion Clothing (dress, shoes, or purse), Jade Blocks to pose on, and a `#Rare`` Spotlight to shine on your fabulousness.|0|0|6000|0|||-1|-1||-1|-1||1|||||||";
					string text29 = "|\nadd_button|sportsball_pack|`oSportsball Pack``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 2 Basketball Hoops, 2 Sporty Goals, 5 Stadiums, 5 Crowded Stadiums, 10 Field Grass, 1 Football Helmet, 1 Growies Cap, 1 Ref's Jersey, 1 World Cup Jersey, 1 `#Rare Sports Item`` or `#Rare Growmoji!``.<CR><CR>`5Description:`` We like sports and we don't care who knows! This pack includes everything you need to get sporty! Use the Sports Items to launch Sportsballs at each other.|0|1|20000|0|||-1|-1||-1|-1||1|||||||";
					string text30 = "|\nadd_button|firefighter|`oFirefighter Pack``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Yellow Helmet, 1 Yellow Jacket, 1 Yellow Pants, 1 Firemans Boots, 1 Fire Hose, and 1 `#Rare Firehouse`` .<CR><CR>`5Description:`` Rescue Growtopians from the fire! Includes a full Yellow Firefighter Outfit, Fire Hose and a `#Rare Firehouse``, which will protect your own world from fires.|0|1|10000|0|||-1|-1||-1|-1||1|||||||";
					string text31 = "|\nadd_button|steampack|`oSteampack``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 10 Steam Tubes, 2 Steam Stompers, 2 Steam Organs, 2 Steam Vents, 2 Steam Valves and 1 `#Rare Steampunk Top Hat``.<CR><CR>`5Description:`` Steam! It's a wondrous new technology that lets you create paths of Steam Blocks, then jump on a Steam Stomper to launch a jet of steam through the path, triggering steam-powered devices. Build puzzles, songs, parkour challenges, and more!|0|6|20000|0|||-1|-1||-1|-1||1|||||||";
					string text32 = "|\nadd_button|paintbrush|`oPainter's Pack``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 `#Rare Paintbrush`` and 20 Random Colored Paint Buckets.<CR><CR>`5Description:`` Want to paint your world? This pack includes 20 buckets of random paint colors (may include Varnish, to clean up your messes)! You can paint any block in your world different colors to personalize it.|0|1|30000|0|||-1|-1||-1|-1||1|||||||";
					string text33 = "|\nadd_button|paleo_kit|`oPaleontologist's Kit``|interface/large/store_buttons/store_buttons16.rttex|`2You Get:`` 5 Fossil Brushes, 1 Rock Hammer, 1 Rock Chisel, 1 Blue Hardhat and 1 `#Rare Fossil Prep Station``.<CR><CR>`5Description:`` If you want to dig up fossils, this is the kit for you! Includes everything you need! Use the prepstation to get your fossils ready for display.|0|0|20000|0|||-1|-1||-1|-1||1|||||||";
					string text34 = "|\nadd_button|robot_starter_pack|`oCyBlocks Starter Pack``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 1 `5Rare ShockBot`` and 10 random movement commands.<CR><CR>`5Description:`` CyBlocks Starter Pack includes one `5Rare`` ShockBot and 10 random movement commands to use with it. `5ShockBot`` is a perma-item, is never lost when destroyed.|0|6|5000|0|||-1|-1||-1|-1||1|||||||";
					string text35 = "|\nadd_button|robot_command_pack|`oCyBlocks Command Pack``|interface/large/store_buttons/store_buttons19.rttex|`2You Get:`` 10 Random CyBlock Commands.<CR><CR>`5Description:`` Grants 10 random CyBlock Commands to help control your CyBots!|0|2|2000|0|||-1|-1||-1|-1||1|||||||";
					string text36 = "|\nadd_button|robot_pack|`oCyBot Pack``|interface/large/store_buttons/store_buttons19.rttex|`2You Get:`` 1 `5Rare CyBot``!<CR><CR>`5Description:`` Grants one random `5Rare`` CyBot! Use CyBlock Commands to send these mechanical monsters into action! `5Note: Each CyBot is a perma-item, and will never be lost when destroyed.``|0|3|15000|0|||-1|-1||-1|-1||1|||||||";
					string text37 = "|\nadd_button|gang_pack|`oGangland Style``|interface/large/store_buttons/store_buttons2.rttex|`2You Get:`` 1 Fedora, 1 Dames Fedora, 1 Pinstripe Suit with Pants, 1 Flapper Headband with Dress, 1 Cigar, 1 Tommy Gun, 1 Victola and 10 Art Deco Blocks .<CR><CR>`5Description:`` Step into the 1920's with a Complete Outfit, a Tommygun, a Victrola that plays jazz music, and 10 Art Deco Blocks. It's the whole package!|0|6|5000|0|||-1|-1||-1|-1||1|||||||";

					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23 + text24 + text25 + text26 + text27 + text28 + text29 + text30 + text31 + text32 + text33 + text34 + text35 + text36 + text37);

				}
				if (cch.find("action|buy\nitem|token") == 0) {
					string text1 = "set_description_text|`2Spend your Growtokens!`` (You have `52``) You earn Growtokens from Crazy Jim and Sales-Man. Select the item you'd like more info on, or BACK to go back.";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||0|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||1|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_button|challenge_timer|`oChallenge Timer``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Challenge Timer.<CR><CR>`5Description:`` Get more people playing your parkours with this secure prize system. You'll need a `#Challenge Start Flag`` and `#Challenge End Flag`` as well (not included). Stock prizes into the Challenge Timer, set a time limit, and watch as players race from start to end. If they make it in time, they win a prize!|0|0|-5|0|||-1|-1||-1|-1||1|||||||";
					string text10 = "|\nadd_button|xp_potion|`oExperience Potion``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Experience Potion.<CR><CR>`5Description:`` This `#Untradeable`` delicious fizzy drink will make you smarter! 10,000 XP smarter instantly, to be exact.|0|2|-10|0|||-1|-1||-1|-1||1|||||||";
					string text11 = "|\nadd_button|megaphone|`oMegaphone``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Megaphone.<CR><CR>`5Description:`` You like broadcasting messages, but you're not so big on spending gems? Buy a Megaphone with Growtokens! Each Megaphone can be used once to send a super broadcast to all players in the game.|0|7|-10|0|||-1|-1||-1|-1||1|||||||";
					string text13 = "|\nadd_button|mini_mod|`oMini-Mod``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Mini-Mod.<CR><CR>`5Description:`` Oh no, it's a Mini-Mod! Punch him to activate (you'll want to punch him!). When activated, he won't allow anyone to drop items in your world.|0|0|-20|0|||-1|-1||-1|-1||1|||||||";
					string text14 = "|\nadd_button|derpy_star|`oDerpy Star Block``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Derpy Star Block.<CR><CR>`5Description:`` DER IM A SUPERSTAR. This is a fairly ordinary block, except for the derpy star on it. Note: it is not permanent, and it doesn't drop seeds. So use it wisely!|0|3|-30|0|||-1|-1||-1|-1||1|||||||";
					string text15 = "|\nadd_button|dirt_gun|`oBLYoshi's Free Dirt``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 BLYoshi's Free Dirt.<CR><CR>`5Description:`` Free might be stretching it, but hey, once you buy this deadly rifle, you can spew out all the dirt you want for free! Note: the dirt is launched at high velocity and explodes on impact. Sponsored by BLYoshi.|0|4|-40|0|||-1|-1||-1|-1||1|||||||";
					string text16 = "|\nadd_button|nothingness|`oWeather Machine - Nothingness``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Weather Machine - Nothingness.<CR><CR>`5Description:`` Tired of all that fancy weather?  This machine will turn your world completely black. Yup, that's it. Not a single pixel in the background except pure blackness.|0|3|-50|0|||-1|-1||-1|-1||1|||||||";
					string text17 = "|\nadd_button|spike_juice|`oSpike Juice``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Spike Juice.<CR><CR>`5Description:`` It's fresh squeezed, with little bits of spikes still in it! Drinking this `#Untradeable`` one-use potion will make you immune to Death Spikes and Lava for 5 seconds.|0|5|-60|0|||-1|-1||-1|-1||1|||||||";
					string text18 = "|\nadd_button|doodad|`oDoodad``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Doodad.<CR><CR>`5Description:`` I have no idea what this thing does. It's something electronic? Maybe?|0|5|-75|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_button|crystal_cape|`oCrystal Cape``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Crystal Cape.<CR><CR>`5Description:`` This cape is woven of pure crystal, which makes it pretty uncomfortable. But it also makes it magical! It lets you double-jump off of an imaginary Crystal Block in mid-air. Sponsored by Edvoid20, HemeTems, and Aboge.|0|5|-90|0|||-1|-1||-1|-1||1|||||||";
					string text20 = "|\nadd_button|focused_eyes|`oFocused Eyes``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Focused Eyes.<CR><CR>`5Description:`` This `#Untradeable`` item lets you shoot electricity from your eyes! Wear them with pride, and creepiness.|0|4|-100|0|||-1|-1||-1|-1||1|||||||";
					string text21 = "|\nadd_button|grip_tape|`oGrip Tape``|interface/large/store_buttons/store_buttons14.rttex|`2You Get:`` 1 Grip Tape.<CR><CR>`5Description:`` This is handy for wrapping around the handle of a weapon or tool. It can improve your grip, as well as protect you from cold metal handles. If you aren't planning to craft a weapon that requires Grip Tape, this does you no good at all!|0|5|-100|0|||-1|-1||-1|-1||1|||||||";
					string text22 = "|\nadd_button|cat_eyes|`oCat Eyes``|interface/large/store_buttons/store_buttons23.rttex|`2You Get:`` 1 Cat Eyes.<CR><CR>`5Description:`` Wow, pawesome! These new eyes are the cat's meow, and the purrfect addition to any style.|0|5|-100|0|||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nadd_button|night_vision|`oNight Vision Goggles``|interface/large/store_buttons/store_buttons15.rttex|`2You Get:`` 1 Night Vision Goggles.<CR><CR>`5Description:`` Scared of the dark? We have a solution. You can wear these goggles just to look cool, but if you also happen to have a D Battery (`4batteries not included``) on you, you will be able to see through darkness like it's not even there! Each D Battery can power your goggles for 1 minute. `2If you are in a world you own, the goggles will not require batteries!`` Note: you can't turn the goggles off without removing them, so you'll be wasting your battery if you wear them in daylight while carrying D Batteries.|0|3|-110|0|||-1|-1||-1|-1||1|||||||";
					string text24 = "|\nadd_button|muddy_pants|`oMuddy Pants``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Muddy Pants.<CR><CR>`5Description:`` Well, this is just a pair of muddy pants. But it does come with a super secret bonus surprise that is sure to blow your mind!|0|7|-125|0|||-1|-1||-1|-1||1|||||||";
					string text25 = "|\nadd_button|piranha|`oCuddly Piranha``|interface/large/store_buttons/store_buttons10.rttex|`2You Get:`` 1 Cuddly Piranha.<CR><CR>`5Description:`` This friendly pet piranha won't stay in its bowl!  It just wants to snuggle with your face!|0|0|-150|0|||-1|-1||-1|-1||1|||||||";
					string text26 = "|\nadd_button|puddy_leash|`oPuddy Leash``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Puddy Leash.<CR><CR>`5Description:`` Puddy is a friendly little kitten who will follow you around forever.|0|7|-180|0|||-1|-1||-1|-1||1|||||||";
					string text27 = "|\nadd_button|golden_axe|`oGolden Pickaxe``|interface/large/store_buttons/store_buttons9.rttex|`2You Get:`` 1 Golden Pickaxe.<CR><CR>`5Description:`` Get your own sparkly pickaxe! This `#Untradeable`` item is a status symbol! Oh sure, it isn't any more effective than a normal pickaxe, but it sparkles!|0|1|-200|0|||-1|-1||-1|-1||1|||||||";
					string text28 = "|\nadd_button|puppy_leash|`oPuppy Leash``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Puppy Leash.<CR><CR>`5Description:`` Get your own pet puppy! This little dog will follow you around forever, never wavering in her loyalty, thus making her `#Untradeable``.|0|4|-200|0|||-1|-1||-1|-1||1|||||||";
					string text29 = "|\nadd_button|diggers_spade|`oDigger's Spade``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Digger's Spade.<CR><CR>`5Description:`` This may appear to be a humble shovel, but in fact it is enchanted with the greatest magic in Growtopia. It can smash Dirt or Cave Background in a single hit! Unfortunately, it's worthless at digging through anything else. Note: The spade is `#UNTRADEABLE``.|0|7|-200|0|||-1|-1||-1|-1||1|||||||";
					string text30 = "|\nadd_button|meow_ears|`oMeow Ears``|interface/large/store_buttons/store_buttons22.rttex|`2You Get:`` 1 Meow Ears.<CR><CR>`5Description:`` Meow's super special ears that everyone can now get! Note: These ears are `#UNTRADEABLE``.|0|0|-200|0|||-1|-1||-1|-1||1|||||||";
					string text31 = "|\nadd_button|frosty_hair|`oFrosty Hair``|interface/large/store_buttons/store_buttons23.rttex|`2You Get:`` 1 Frosty Hair.<CR><CR>`5Description:`` Coldplay is cold, but you can be freezing! Note: The frosty hair is `#UNTRADEABLE``.|0|0|-200|0|||-1|-1||-1|-1||1|||||||";
					string text32 = "|\nadd_button|zerkon_helmet|`oEvil Space Helmet``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Evil Space Helmet.<CR><CR>`5Description:`` Zerkon commands a starship too small to actually board - pah, time to rule the galaxy properly! Note: The evil space helmet is `#UNTRADEABLE``.|0|6|-200|0|||-1|-1||-1|-1||1|||||||";
					string text33 = "|\nadd_button|seils_magic_orb|`oSeil's Magic Orbs``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Seil's Magic Orbs.<CR><CR>`5Description:`` Seil is some kind of evil wizard, now you can be too! Note: These magic orbs are `#UNTRADEABLE``.|0|7|-200|0|||-1|-1||-1|-1||1|||||||";
					string text34 = "|\nadd_button|atomic_shadow_scythe|`oAtomic Shadow Scythe``|interface/large/store_buttons/store_buttons21.rttex|`2You Get:`` 1 Atomic Shadow Scythe.<CR><CR>`5Description:`` AtomicShadow might actually be evil, now you can try it out! Note: The shadow scythe is `#UNTRADEABLE``.|0|5|-200|0|||-1|-1||-1|-1||1|||||||";
					string text35 = "|\nadd_button|poseidon_diggers_trident|`oPoseidon's Digger's Trident``|interface/large/store_buttons/store_buttons25.rttex|`2You Get:`` 1 Poseidon's Digger's Trident.<CR><CR>`5Description:`` A gift from the gods. This may appear to be a humble trident, but in fact it has the power of Poseidon himself. It can smash `8Deep Sand`` or `8Ocean Rock`` in a single hit. Unfortunately, you don't get to wield the full might of Poseidon... the trident is worthless at smashing anything else. Note: The trident is `#UNTRADEABLE``.|0|6|-200|0|||-1|-1||-1|-1||1|||||||";

					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23 + text24 + text25 + text26 + text27 + text28 + text29 + text30 + text31 + text32 + text33 + text34 + text35);

				}
				if (cch.find("action|storenavigate\nitem|main\nselection|gems_rain") == 0) {
					string text1 = "set_description_text|Welcome to the `2Growtopia Store``! Select the item you'd like more info on.`o `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
					string text2 = "|enable_tabs|1";
					string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||1|0|0|0||||-1|-1||||";
					string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1||||";
					string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
					string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
					string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
					string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
					string text9 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|1|";
					string text10 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons/store_buttons16.rttex|September 2021:`` `9Ouroboros Charm``!<CR><CR>`2You Get:`` 1 `9Ouroboros Charm``.<CR><CR>`5Description: ``The endless loop of life and death, personified and celebrated. Is it a charm or is it a curse?|0|3|350000|0||interface/large/gui_store_button_overlays1.rttex|0|0||-1|-1||1|||||||";
					string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1|||||||";
					string text12 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|2|";
					string text13 = "|\nadd_button|gems_glory|Road To Glory|interface/large/store_buttons/store_buttons30.rttex|rt_grope_loyalty_bundle01|0|0|0|0||interface/large/gui_store_button_overlays1.rttex|0|0|/interface/large/gui_shop_buybanner.rttex|1|0|`2You Get:`` Road To Glory and 100k Gems Instantly.<CR>`5Description:`` Earn Gem rewards when you level up. Every 10 levels you will get additional Gem rewards up to Level 50! Claim all rewards instantly if you are over level 50!! 1.6M Gems in total!! |1|||||||";
					string text14 = "|\nadd_button|gems_rain|It's Rainin' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_rain|1|5|0|0|||-1|-1||-1|-1|`2You Get:`` 200,000 Gems, 2 Growtoken, and 1 Megaphone.<CR><CR>`5Description:`` All the gems you could ever want and more plus 2 Growtokens and a Megaphone to tell the whole world about it.|1|||||||";
					string text15 = "|\nadd_button|gems_fountain|Gem Fountain|interface/large/store_buttons/store_buttons2.rttex|rt_grope_gem_fountain|0|2|0|0|||-1|-1||-1|-1|`2You Get:`` 90,000 Gems and 1 Growtoken.<CR><CR>`5Description:`` Get a pile of gems to shop to your hearts desire and 1 Growtoken.|1|||||||";
					string text16 = "|\nadd_button|gems_chest|Chest o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_chest|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 30,000 Gems.<CR><CR>`5Description:`` Get a chest containing gems.|1|||||||";
					string text17 = "|\nadd_button|gems_bag|Bag o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_bag|1|0|0|0|||-1|-1||-1|-1|`2You Get:`` 14,000 Gems.<CR><CR>`5Description:`` Get a small bag of gems.|1|||||||";
					string text18 = "|\nadd_button|tapjoy|Earn Free Gems|interface/large/store_buttons/store_buttons.rttex||1|2|0|0|||-1|-1||-1|-1||1|||||||";
					string text19 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|3|";
					string text20 = "|\nadd_button|365d|`o1-Year Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle02|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 1-Year Subscription Token and 25 Growtokens.<CR><CR>`5Description:`` One full year of special treatment AND 25 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1|||||||";
					string text21 = "|\nadd_button|30d|`o30-Day Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle01|0|4|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 30-Day Free Subscription Token and 2 Growtokens.<CR><CR>`5Description:`` 30 full days of special treatment AND 2 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1|||||||";
					string text22 = "|\nadd_button|video_tapjoy|Watch Videos For Gems|interface/large/store_buttons/store_buttons29.rttex||0|1|0|0|1/5 VIDEOS WATCHED||-1|-1||-1|-1||1|||||||";
					string text23 = "|\nselect_item|gems_rain";


					packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22 + text23);

				}
				if (cch.find("action|buy\nitem|itemomonth") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygems1 = ((PlayerInfo*)(peer->data))->buygems;
					int ad = atoi(acontent.c_str());
					int buygemsz1 = buygems1 - 350000;
					int as = ad + buygemsz1;
					bool success = true;
					if (ad > 349999) {
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << as;
						myfile.close();
						GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), as));
						ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetsa);
						delete psa.data;

						packet::consolemessage(peer, "`5Got 1 `9Ouroboros Charm");
						packet::storepurchaseresult(peer, "`5You just bought an Ouroboros Charm and\n`oReceived: `o1 `9Ouroboros Charm.");

						SaveShopsItemMoreTimes(11232, 1, peer, success);
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");

					}
				}
				if (cch.find("action|buy\nitem|nyan_hat") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemsss1 = ((PlayerInfo*)(peer->data))->buygems;
					int asd = atoi(acontent.c_str());
					int buygemsssz1 = buygemsss1 - 25000;
					int aws = asd + buygemsssz1;
					bool success = true;
					if (asd > 24999) {
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << aws;
						myfile.close();
						GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), aws));
						ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetsa);
						delete psa.data;

						packet::consolemessage(peer, "`5Got 1 `#Turtle Hat");
						packet::storepurchaseresult(peer, "`5You just bought a Turtle Hat and\n`oReceived: `o1 Turtle Hat.");

						SaveShopsItemMoreTimes(574, 1, peer, success);
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");

					}
				}
				if (cch.find("action|buy\nitem|tiny_horsie") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemsss1 = ((PlayerInfo*)(peer->data))->buygems;
					int asd = atoi(acontent.c_str());
					int buygemsssz1 = buygemsss1 - 25000;
					int aws = asd + buygemsssz1;
					bool success = true;
					if (asd > 24999) {
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << aws;
						myfile.close();
						GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), aws));
						ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetsa);
						delete psa.data;

						packet::consolemessage(peer, "`5Got 1 `#Tiny Horsie");
						packet::storepurchaseresult(peer, "`5You just bought a Tiny Horsie and\n`oReceived: `o1 Tiny Horsie.");

						SaveShopsItemMoreTimes(592, 1, peer, success);
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");

					}
				}
				if (cch.find("action|buy\nitem|diggers_spade") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
						packet::consolemessage(peer, "`5Got 1 `oDigger´s Spade Digger´s Spade ");
						packet::storepurchaseresult(peer, "`5You just bought a Digger´s Spade and\n`oReceived: `o1 `2Digger´s Spade.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(2952, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|xp_potion") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 10, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oExperience Potion ");
						packet::storepurchaseresult(peer, "`5You just bought a Experience Potion and\n`oReceived: `o1 `2Experience Potion.");
						RemoveInventoryItem(1486, 10, peer);
						SaveShopsItemMoreTimes(1488, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|puddy_leash") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 180, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oPuddy Leash ");
						packet::storepurchaseresult(peer, "`5You just bought a Puddy Leash and\n`oReceived: `o1 `2Puddy Leash.");
						RemoveInventoryItem(1486, 180, peer);
						SaveShopsItemMoreTimes(2032, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|golden_axe") == 0) {
					auto iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						RemoveInventoryItem(1486, 200, peer);
						bool success = true;
						packet::PlayAudio(peer, "audio/piano_nice.wav", 0);
						SaveShopsItemMoreTimes(1438, 1, peer, success);
						packet::consolemessage(peer, "`5Got 1 `oGolden Pickaxe ");
						packet::storepurchaseresult(peer, "`5You just bought a Golden Pickaxe and\n`oReceived: `o1 `2Golden Pickaxe.");
					}
					else {
						packet::storepurchaseresult(peer, "You don't have enough Growtoken to buy this item.");
					}
					}
				if (cch.find("action|buy\nitem|puppy_leash") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 180, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oPuppy Leash ");
						packet::storepurchaseresult(peer, "`5You just bought a Puppy Leash and\n`oReceived: `o1 `2Puppy Leash.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(1742, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|meow_ears") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oMeow Ears ");
						packet::storepurchaseresult(peer, "`5You just bought Meow Ears and\n`oReceived: `o1 `2Meow Ears.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(698, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|frosty_hair") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oFrosty Hair ");
						packet::storepurchaseresult(peer, "`5You just bought Frosty Hair and\n`oReceived: `o1 `2Frosty Hair.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(1444, 1, peer, success);
					}
				}

				if (cch.find("action|buy\nitem|seils_magic_orb") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oSeils Magic Orbs ");
						packet::storepurchaseresult(peer, "`5You just bought Seils Magic Orbs and\n`oReceived: `o1 `2Seils Magic Orbs.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(820, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|zerkon_helmet") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oSeils Magic Orbs ");
						packet::storepurchaseresult(peer, "`5You just bought Seils Magic Orbs and\n`oReceived: `o1 `2Seils Magic Orbs.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(1440, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|atomic_shadow_scythe") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oAtomic Shadow Scythe ");
						packet::storepurchaseresult(peer, "`5You just bought a Atomic Shadow Scythe and\n`oReceived: `o1 `2Atomic Shadow Scythe.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(1484, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|poseidon_diggers_trident") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 200, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oPoseidon's Digger's Trident ");
						packet::storepurchaseresult(peer, "`5You just bought a Poseidon's Digger's Trident and\n`oReceived: `o1 `2Poseidon's Digger's Trident.");
						RemoveInventoryItem(1486, 200, peer);
						SaveShopsItemMoreTimes(7434, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|megaphone") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 10, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oMegaphone ");
						packet::storepurchaseresult(peer, "`5You just bought a Megaphone and\n`oReceived: `o1 `2Megaphone.");
						RemoveInventoryItem(1486, 10, peer);
						SaveShopsItemMoreTimes(2480, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|mini_mod") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 20, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oMini-Mod ");
						packet::storepurchaseresult(peer, "`5You just bought a Mini-Mod and\n`oReceived: `o1 `2Mini-Mod.");
						RemoveInventoryItem(1486, 20, peer);
						SaveShopsItemMoreTimes(4758, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|derpy_star") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 30, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oDerpy Star Block ");
						packet::storepurchaseresult(peer, "`5You just bought a Derpy Star Block and\n`oReceived: `o1 `2Derpy Star Block.");
						RemoveInventoryItem(1486, 30, peer);
						SaveShopsItemMoreTimes(1628, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|dirt_gun") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 40, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oBlYoshi's Dirtgun ");
						packet::storepurchaseresult(peer, "`5You just bought a BLYoshi's Dirtgun and\n`oReceived: `o1 `2BlYoshi's Dirtgun.");
						RemoveInventoryItem(1486, 40, peer);
						SaveShopsItemMoreTimes(2876, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|nothingness") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 50, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oWeather Machine - Nothingness. ");
						packet::storepurchaseresult(peer, "`5You just bought a Nothingness and\n`oReceived: `o1 `2Weather Machine - Nothingness.");
						RemoveInventoryItem(1486, 50, peer);
						SaveShopsItemMoreTimes(1490, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|spike_juice") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 60, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oSpike Juice ");
						packet::storepurchaseresult(peer, "`5You just bought a Spike Juice and\n`oReceived: `o1 `2Spike Juice.");
						RemoveInventoryItem(1486, 60, peer);
						SaveShopsItemMoreTimes(1662, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|doodad") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 75, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oDoodad ");
						packet::storepurchaseresult(peer, "`5You just bought a Doodad and\n`oReceived: `o1 `2Doodad.");
						RemoveInventoryItem(1486, 75, peer);
						SaveShopsItemMoreTimes(1492, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|crystal_cape") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 90, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oCrystal Cape ");
						packet::storepurchaseresult(peer, "`5You just bought a Doodad and\n`oReceived: `o1 `2Crystal Cape.");
						RemoveInventoryItem(1486, 90, peer);
						SaveShopsItemMoreTimes(1738, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|focused_eyes") == 0) {
					auto Price = 100;
					PlayerInfo* pData = ((PlayerInfo*)(peer->data));
					auto ItemID = 1204;
					auto count = 1;
					auto contains = false;
					auto KiekTuri = 0;
					try {
						for (auto i = 0; i < pData->inventory.items.size(); i++) {
							if (pData->inventory.items.at(i).itemID == 1486 && pData->inventory.items.at(i).itemCount >= 1) {
								KiekTuri = pData->inventory.items.at(i).itemCount;
								break;
							}
						}
					}
					catch (const std::out_of_range& e) {
						std::cout << e.what() << std::endl;
					}
					SearchInventoryItem(peer, 1486, Price, contains);
					if (contains) {
						if (CheckItemMaxed(peer, ItemID, count) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && CheckItemExists(peer, ItemID) && CheckItemMaxed(peer, ItemID, 1) || pData->inventory.items.size() + 1 >= pData->currentInventorySize && !CheckItemExists(peer, ItemID)) {
							packet::PlayAudio(peer, "audio/bleep_fail.wav", 0);
							packet::storepurchaseresult(peer, "You don't have enough space in your inventory that. You may be carrying to many of one of the items you are trying to purchase or you don't have enough free spaces to fit them all in your backpack!");
							break;
						}
						RemoveInventoryItem(1486, Price, peer);
						bool success = false;
						SaveShopsItemMoreTimes(ItemID, count, peer, success);
						packet::PlayAudio(peer, "audio/piano_nice.wav", 0);
						packet::storepurchaseresult(peer, "You've purchased " + to_string(count) + " `o" + getItemDef(ItemID).name + " `wfor `$" + to_string(Price) + " `wGrowtokens.\nYou have `$" + to_string(KiekTuri - Price) + " `wGrowtokens left.\n\n`5Received: ``" + to_string(count) + " " + getItemDef(ItemID).name + "");
					}
					else {
						packet::PlayAudio(peer, "audio/bleep_fail.wav", 0);
						packet::storepurchaseresult(peer, "You can't afford `o" + getItemDef(ItemID).name + "``!  You're `$" + to_string(Price - KiekTuri) + "`` Growtokens short.");
					}
				}
				if (cch.find("action|buy\nitem|grip_tape") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 100, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
							packet::consolemessage(peer, "`5Got 1 `oGrip Tape ");
						packet::storepurchaseresult(peer, "`5You just bought a Grip Tape and\n`oReceived: `o1 `2Grip Tape.");
						RemoveInventoryItem(1486, 100, peer);
						SaveShopsItemMoreTimes(3248, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|cat_eyes") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 100, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
						packet::consolemessage(peer, "`5Got 1 `oCat Eyes ");
						packet::storepurchaseresult(peer, "`5You just bought a Cat Eyes and\n`oReceived: `o1 `2Cat Eyes.");
						RemoveInventoryItem(1486, 100, peer);
						SaveShopsItemMoreTimes(7106, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|night_vision") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 110, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
						packet::consolemessage(peer, "`5Got 1 `oNight Vision Goggles ");
						packet::storepurchaseresult(peer, "`5You just bought a Night Vision Goggles and\n`oReceived: `o1 `2Night Vision Goggles.");
						RemoveInventoryItem(1486, 110, peer);
						SaveShopsItemMoreTimes(3576, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|piranha") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 150, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
						packet::consolemessage(peer, "`5Got 1 `oCuddly Piranha ");
						packet::storepurchaseresult(peer, "`5You just bought a Cuddly Piranha and\n`oReceived: `o1 `2Cuddly Piranha.");
						RemoveInventoryItem(1486, 150, peer);
						SaveShopsItemMoreTimes(1534, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|muddy_pants") == 0) {
					bool iscontains = false;
					SearchInventoryItem(peer, 1486, 125, iscontains);
					if (!iscontains)
					{
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Tokens To Buy This Items. `5Try again later.");

					}
					else {
						bool success = true;
						if (success)
						packet::consolemessage(peer, "`5Got 1 `oMuddy Pants ");
						packet::storepurchaseresult(peer, "`5You just bought a Muddy Pants and\n`oReceived: `o1 `2Muddy Pants.");
						RemoveInventoryItem(1486, 125, peer);
						SaveShopsItemMoreTimes(2584, 1, peer, success);

					}
				}
				if (cch.find("action|buy\nitem|door_mover") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemsss1 = ((PlayerInfo*)(peer->data))->buygems;
					int asd = atoi(acontent.c_str());
					int buygemsssz1 = buygemsss1 - 5000;
					int aws = asd + buygemsssz1;
					bool success = true;
					if (asd > 4999) {
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << aws;
						myfile.close();
						GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), aws));
						ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetsa);
						delete psa.data;

						packet::consolemessage(peer, "`5Got 1 `#Door Mover");
						packet::storepurchaseresult(peer, "`5You just bought a Door Mover and\n`oReceived: `o1 Door Mover.");

						SaveShopsItemMoreTimes(1404, 1, peer, success);
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");

					}
				}
				if (cch.find("action|buy\nitem|rockin_pack") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 10000;
					int asss = adss + buygemssz12;
					if (adss > 9999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `oKeytar");
						packet::consolemessage(peer, "`5Got 1 `oBass Guitar");
						packet::consolemessage(peer, "`5Got 1 `oTambourine");
						packet::consolemessage(peer, "`5Got 1 `oStarchild Makeup");
						packet::consolemessage(peer, "`5Got 1 `oRockin' Headband");
						packet::consolemessage(peer, "`5Got 1 `oLeopard Leggings");
						packet::consolemessage(peer, "`5Got 1 `oShredded T-Shirt");
						packet::consolemessage(peer, "`5Got 1 `oDrumkit");
						packet::consolemessage(peer, "`5Got 6 Stage Support");
						packet::consolemessage(peer, "`5Got 6 Mega Rock Speaker");
						packet::consolemessage(peer, "`5Got 6 Rock N' Roll Wallpaper");
						packet::storepurchaseresult(peer, "`5You just bought 1 Rockin'Pack and\n`oReceived: `o1 Rockin'Pack.");

						SaveShopsItemMoreTimes(1714, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1710, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1712, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1718, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1732, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1722, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1720, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1724, 1, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1728, 6, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1730, 6, peer, success); // aposition, itemid, quantity, peer, success
						SaveShopsItemMoreTimes(1726, 6, peer, success); // aposition, itemid, quantity, peer, success
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|upgrade_backpack") == 0) {
					packet::storepurchaseresult(peer, "`4You already have max inventory");
				}
				if (cch.find("action|buy\nitem|unicorn") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 50000;
					int asss = adss + buygemssz12;
					if (adss > 49999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Unicorn");
						packet::storepurchaseresult(peer, "`5You just bought an Unicorn and\n`oReceived: `o1 Unicorn.");

						SaveShopsItemMoreTimes(1648, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|owl") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 25000;
					int asss = adss + buygemssz12;
					if (adss > 24999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Mid Pacific Owl");
						packet::storepurchaseresult(peer, "`5You just bought a Mid Pacific Owl and\n`oReceived: `o1 Mid Pacific Owl.");

						SaveShopsItemMoreTimes(1540, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|raptor") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 25000;
					int asss = adss + buygemssz12;
					if (adss > 24999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Riding Raptor");
						packet::storepurchaseresult(peer, "`5You just bought a Riding Raptor and\n`oReceived: `o1 Riding Raptor.");

						SaveShopsItemMoreTimes(1320, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|ambulance") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 25000;
					int asss = adss + buygemssz12;
					if (adss > 24999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Ambulance");
						packet::storepurchaseresult(peer, "`5You just bought an Ambulance and\n`oReceived: `o1 Ambulance.");

						SaveShopsItemMoreTimes(1272, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|corvette") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 25000;
					int asss = adss + buygemssz12;
					if (adss > 24999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Little Red Corvette");
						packet::storepurchaseresult(peer, "`5You just bought a Little Red Corvette and\n`oReceived: `o1 Little Red Corvette.");

						SaveShopsItemMoreTimes(766, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|dragon_hand") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 50000;
					int asss = adss + buygemssz12;
					if (adss > 49999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `4Dragon Hand");
						packet::storepurchaseresult(peer, "`5You just bought a Dragon Hand and\n`oReceived: `o1 Dragon Hand.");

						SaveShopsItemMoreTimes(900, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|star_ship") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 25000;
					int asss = adss + buygemssz12;
					if (adss > 24999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Pleiadian Star Ship");
						packet::storepurchaseresult(peer, "`5You just bought a Pleiadian Star Ship and\n`oReceived: `o1 Pleiadian Star Ship.");

						SaveShopsItemMoreTimes(760, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|small_lock") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 50;
					int asss = adss + buygemssz12;
					if (adss > 49) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Small Lock");
						packet::storepurchaseresult(peer, "`5You just bought 1 Small Lock and\n`oReceived: `o1 Small Lock.");

						SaveShopsItemMoreTimes(202, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|big_lock") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 200;
					int asss = adss + buygemssz12;
					if (adss > 199) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Big Lock");
						packet::storepurchaseresult(peer, "`5You just bought 1 Big Lock and\n`oReceived: `o1 Big Lock.");

						SaveShopsItemMoreTimes(204, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|big_lock") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 500;
					int asss = adss + buygemssz12;
					if (adss > 499) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Huge Lock");
						packet::storepurchaseresult(peer, "`5You just bought 1 Huge Lock and\n`oReceived: `o1 Huge Lock.");

						SaveShopsItemMoreTimes(206, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|door_pack") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 15;
					int asss = adss + buygemssz12;
					if (adss > 14) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Door");
						packet::consolemessage(peer, "`5Got 1 `#Sign");
						packet::storepurchaseresult(peer, "`5You just bought a Door and Sign and\n`oReceived: `o1 Door and Sign.");

						SaveShopsItemMoreTimes(12, 1, peer, success);
						SaveShopsItemMoreTimes(20, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|10_wl") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 20000;
					int asss = adss + buygemssz12;
					if (adss > 19999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 10 `#World Locks");
						packet::storepurchaseresult(peer, "`5You just bought 10 World Locks and\n`oReceived: `o10 World Locks.");

						SaveShopsItemMoreTimes(242, 10, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|vending_machine") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygemss12 = ((PlayerInfo*)(peer->data))->buygems;
					int adss = atoi(acontent.c_str());
					int buygemssz12 = buygemss12 - 8000;
					int asss = adss + buygemssz12;
					if (adss > 7999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#Vending Machine");
						packet::storepurchaseresult(peer, "`5You just bought 1 Vending Machine and\n`oReceived: `o1 Vending Machine.");

						SaveShopsItemMoreTimes(242, 10, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|world_lock") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygems12 = ((PlayerInfo*)(peer->data))->buygems;
					int ads = atoi(acontent.c_str());
					int buygemsz12 = buygems12 - 2000;
					int ass = ads + buygemsz12;
					if (ads > 1999) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << ass;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#World Lock");
						packet::storepurchaseresult(peer, "`5You just bought a World Lock and\n`oReceived: `o1 World Lock.");

						SaveShopsItemMoreTimes(242, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), ass));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|buy\nitem|ads_tv") == 0) {
					std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
					std::string acontent((std::istreambuf_iterator<char>(ifsz)),
						(std::istreambuf_iterator<char>()));
					int buygems123 = ((PlayerInfo*)(peer->data))->buygems;
					int ads1 = atoi(acontent.c_str());
					int buygemsz123 = buygems123 - 50;
					int asss = ads1 + buygemsz123;
					if (ads1 > 49) {
						bool success = true;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << asss;
						myfile.close();

						packet::consolemessage(peer, "`5Got 1 `#GrowShow TV");
						packet::storepurchaseresult(peer, "`5You just bought a GrowShow TV and\n`oReceived: `o1 GrowShow TV.");

						SaveShopsItemMoreTimes(9466, 1, peer, success);
						GamePacket pssa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), asss));
						ENetPacket* packetssa = enet_packet_create(pssa.data, pssa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetssa);
						delete pssa.data;
						packet::PlayAudio(peer, "audio/cash_register.wav", 0);
					}
					else {
						packet::storepurchaseresult(peer, "`4Purchase Failed: You Don't Have Enough Gems To Buy This Items. `5Try again later.");
					}
				}
				if (cch.find("action|respawn") == 0)
				{
					int x = 3040;
					int y = 736;

					if (!world) continue;

					for (int i = 0; i < world->width * world->height; i++)
					{
						if (world->items[i].foreground == 6) {
							x = (i % world->width) * 32;
							y = (i / world->width) * 32;
						}
					}
					{
						PlayerMoving data;
						data.packetType = 0x0;
						data.characterState = 0x924; // animation
						data.x = x;
						data.y = y;
						data.punchX = -1;
						data.punchY = -1;
						data.XSpeed = 0;
						data.YSpeed = 0;
						data.netID = ((PlayerInfo*)(peer->data))->netID;
						data.plantingTree = 0x0;
						SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
					}

					{
						int x = 3040;
						int y = 736;

						for (int i = 0; i < world->width * world->height; i++)
						{
							if (world->items[i].foreground == 6) {
								x = (i % world->width) * 32;
								y = (i / world->width) * 32;
							}
						}

						gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
						p.Insert("OnSetPos");
						p.Insert(x, y);
						p.CreatePacket(peer);
					}
					{
						int x = 3040;
						int y = 736;

						for (int i = 0; i < world->width * world->height; i++)
						{
							if (world->items[i].foreground == 6) {
								x = (i % world->width) * 32;
								y = (i / world->width) * 32;
							}
						}

						gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
						p.Insert("OnSetFreezeState");
						p.Insert(0);
						p.CreatePacket(peer);
					}
#ifdef TOTAL_LOG
					cout << "Respawning... " << endl;
#endif
				}
				if (cch.find("action|growid") == 0)
				{
					string ip = std::to_string(peer->address.host);
					if (ip.length() == 3) {
						packet::SendTalkSelf(peer, "`4Oops! `wYou've reached the max `5GrowID `waccounts you can make for this device or IP address!");
						continue;
					}
					Sleep(1000);
#ifndef REGISTRATION
					{
						GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "Registration is not supported yet!"));
						ENetPacket* packet = enet_packet_create(p.data,
							p.len,
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
						delete p.data;
						//enet_host_flush(server);
					}
#endif
#ifdef REGISTRATION
					//GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`w" + itemDefs.at(id).name + "``|left|" + std::to_string(id) + "|\n\nadd_spacer|small|\nadd_textbox|" + itemDefs.at(id).description + "|left|\nadd_spacer|small|\nadd_quick_exit|\nadd_button|chc0|Close|noflags|0|0|\nnend_dialog|gazette||OK|"));
					packet::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\n\nadd_spacer|small|\nadd_textbox|A `wGrowID `wmeans `oyou can use a name and password to logon from any device.|\nadd_spacer|small|\nadd_textbox|This `wname `owill be reserved for you and `wshown to other players`o, so choose carefully!|\nadd_text_input|username|GrowID||30|\nadd_text_input|password|Password||100|\nadd_text_input|passwordverify|Password Verify||100|\nadd_textbox|Your `wemail address `owill only be used for account verification purposes and won't be spammed or shared. If you use a fake email, you'll never be able to recover or change your password.|\nadd_text_input|email|Email||100|\nadd_textbox|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag`o. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o.|\nadd_text_input|discord|Discord||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
#endif
				}
				if (cch.find("action|store\nlocation|gem") == 0)
				{
				string text1 = "set_description_text|Welcome to the `2Growtopia Store``! Select the item you'd like more info on.`o `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
				string text2 = "|enable_tabs|1";
				string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||1|0|0|0||||-1|-1||||";;
				string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||";
				string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
				string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
				string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
				string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
				string text9 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|1|";
				string text10 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons/store_buttons16.rttex|September 2021:`` `9Ouroboros Charm``!<CR><CR>`2You Get:`` 1 `9Ouroboros Charm``.<CR><CR>`5Description: ``The endless loop of life and death, personified and celebrated. Is it a charm or is it a curse?|0|3|350000|0||interface/large/gui_store_button_overlays1.rttex|0|0||-1|-1||1|||||||";
				string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1||||||";
				string text12 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|2|";
				string text13 = "|\nadd_button|gems_glory|Road To Glory|interface/large/store_buttons/store_buttons30.rttex|rt_grope_loyalty_bundle01|0|0|0|0||interface/large/gui_store_button_overlays1.rttex|0|0|/interface/large/gui_shop_buybanner.rttex|1|0|`2You Get:`` Road To Glory and 100k Gems Instantly.<CR>`5Description:`` Earn Gem rewards when you level up. Every 10 levels you will get additional Gem rewards up to Level 50! Claim all rewards instantly if you are over level 50!! 1.6M Gems in total!! |1|||||||";
				string text14 = "|\nadd_button|gems_rain|It's Rainin' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_rain|1|5|0|0|||-1|-1||-1|-1|`2You Get:`` 200,000 Gems, 2 Growtoken, and 1 Megaphone.<CR><CR>`5Description:`` All the gems you could ever want and more plus 2 Growtokens and a Megaphone to tell the whole world about it.|1|||||||";
				string text15 = "|\nadd_button|gems_fountain|Gem Fountain|interface/large/store_buttons/store_buttons2.rttex|rt_grope_gem_fountain|0|2|0|0|||-1|-1||-1|-1|`2You Get:`` 90,000 Gems and 1 Growtoken.<CR><CR>`5Description:`` Get a pile of gems to shop to your hearts desire and 1 Growtoken.|1|||||||";
				string text16 = "|\nadd_button|gems_chest|Chest o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_chest|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 30,000 Gems.<CR><CR>`5Description:`` Get a chest containing gems.|1|||||||";
				string text17 = "|\nadd_button|gems_bag|Bag o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_bag|1|0|0|0|||-1|-1||-1|-1|`2You Get:`` 14,000 Gems.<CR><CR>`5Description:`` Get a small bag of gems.|1|||||||";
				string text18 = "|\nadd_button|tapjoy|Earn Free Gems|interface/large/store_buttons/store_buttons.rttex||1|2|0|0|||-1|-1||-1|-1||1|||||||";
				string text19 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|3|";
				string text20 = "|\nadd_button|365d|`o1-Year Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle02|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 1-Year Subscription Token and 25 Growtokens.<CR><CR>`5Description:`` One full year of special treatment AND 25 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
				string text21 = "|\nadd_button|30d|`o30-Day Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle01|0|4|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 30-Day Free Subscription Token and 2 Growtokens.<CR><CR>`5Description:`` 30 full days of special treatment AND 2 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
				string text22 = "|\nadd_button|video_tapjoy|Watch Videos For Gems|interface/large/store_buttons/store_buttons29.rttex||0|1|0|0|1/5 VIDEOS WATCHED||-1|-1||-1|-1||1||||||";
				/*
							string text1 = "set_description_text|Welcome to the `2Growtopia Store``!  Tap the item you'd like more info on.`o  `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
							string text2 = "|\nadd_button|iapp_menu|Buy Gems|interface/large/store_buttons5.rttex||0|2|0|0|";
							string text3 = "|\nadd_button|subs_menu|Subscriptions|interface/large/store_buttons22.rttex||0|1|0|0|";
							string text4 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons16.rttex|`2September 2021:`` `9The endless loop of life and death, personified and celebrated. Is it a charm or is it a curse?``|0|3|350000|0|";
							string text5 = "|\nadd_button|locks_menu|Locks And Stuff|interface/large/store_buttons3.rttex||0|4|5|0|";
							string text6 = "|\nadd_button|itempack_menu|Item Packs|interface/large/store_buttons3.rttex||0|3|0|10|";
							string text7 = "|\nadd_button|bigitems_menu|Awesome Items|interface/large/store_buttons4.rttex||0|6|12|0|";
							string text8 = "|\nadd_button|weather_menu|Weather Machines|interface/large/store_buttons5.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|4|15|0||";
							string text9 = "|\nadd_button|token_menu|Growtoken Items|interface/large/store_buttons9.rttex||0|1|15|0|";

							packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22);
						}*/
				packet::storerequest(peer, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22);
                }
				if (cch.find("action|info") == 0)
				{
					std::stringstream ss(cch);
					std::string to;
					int id = -1;
					int count = -1;
					while (std::getline(ss, to, '\n')) {
						vector<string> infoDat = explode("|", to);
						if (infoDat.size() == 3) {
							if (infoDat[1] == "itemID") id = atoi(infoDat[2].c_str());
							if (infoDat[1] == "count") count = atoi(infoDat[2].c_str());
						}
					}
					if (id == -1 || count == -1) continue;
					if (itemDefs.size() < id || id < 0) continue;
					packet::dialog(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`w"+ itemDefs.at(id).name +"``|left|"+std::to_string(id)+"|\n\nadd_spacer|small|\nadd_textbox|"+ itemDefs.at(id).description +"|left|\nadd_spacer|small|\nadd_quick_exit|\nend_dialog|item_info|OK||");
				}
				if (cch.find("action|dialog_return\ndialog_name|sign_edit") == 0) {
					if (world != NULL) {
						if (((PlayerInfo*)(peer->data))->rawName == PlayerDB::getProperName(world->owner)) {
							std::stringstream ss(GetTextPointerFromPacket(event.packet));
							std::string to;
							int x = 0;
							int y = 0;
							bool created = false;
							string text = "";
							string world = ((PlayerInfo*)(peer->data))->currentWorld;
							while (std::getline(ss, to, '\n')) {
								string id = to.substr(0, to.find("|"));
								string act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
								if (id == "tilex")
								{
									x = atoi(act.c_str());

								}
								else if (id == "tiley")
								{
									y = atoi(act.c_str());
								}
								else if (id == "ch3")
								{
									text = act;
									created = true;
								}
								if (created == true) {
									if (text == "__%&P&%__") {
										continue;
									}
								}
								if (text.length() < 255) {
									WorldInfo* worldInfo = getPlyersWorld(peer);
									int squaresign = ((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * 100);
									updateSignSound(peer, worldInfo->items[squaresign].foreground, squaresign % worldInfo->width, squaresign / worldInfo->width, text, worldInfo->items[squaresign].background);
									worldInfo->items[squaresign].text = text;
								}
							}
						}
					}
				}
				if (cch.find("action|dialog_return") == 0)
				{
					std::stringstream ss(cch);
					std::string to;
					string btn = "";
					bool captcha = false;
					bool isRegisterDialog = false;
					bool SurgDialog = false;
					bool isGuildDialog = false;
					string guildName = "";
					string guildStatement = "";
					string guildFlagBg = "";
					string guildFlagFg = "";
					string item = "";
					bool marsdialog = false;
					string marstext = "";
					bool isFindDialog = false;
					bool isEpoch = false;
					bool ice = false;
					bool volcano = false;
					bool land = false;
					string itemFind = "";
					string username = "";
					string password = "";
					string passwordverify = "";
					string email = "";
					string discord = "";
					string captchaanswer, number1, number2 = "";
					int level = 1;
					int xp = 0;
					int hasil = 0;
					int resultnbr1 = 0;
					int resultnbr2 = 0;
					string textlevel = "";
					bool isTrashDialog = false;
					string trashitemcount = "";
					bool isEntranceDialog = false;
					string entranceresult = "";
					bool isGEntrance = false;
					string gentranceresult = "";
					bool isBG = false;
					string bgitem = "";
					bool levels = false;
					bool gems = false;
					bool cbox1 = false;
					int cboxEpoch = 0;
					bool isStuff = false;
					string stuff_gravity = "";
					bool stuff_invert = false;
					bool stuff_spin = false;
					string stuffITEM = "";
					bool gacha = false;
					bool isBuyItemDialog = false;
					string textgems = "";
					string textgacha = "";
					while (std::getline(ss, to, '\n')) {
						vector<string> infoDat = explode("|", to);
						if (infoDat.size() == 2) {
							if (infoDat[0] == "dialog_name" && infoDat[1] == "findid") {
								isFindDialog = true;
							}
							if (isFindDialog) {
								if (infoDat[0] == "item") itemFind = infoDat[1];
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "sendmb") marsdialog = true;
							if (infoDat[0] == "dialog_name" && infoDat[1] == "buyitembyrarity") isBuyItemDialog = true;
							if (infoDat[0] == "dialog_name" && infoDat[1] == "surge") SurgDialog = true;
							if (infoDat[0] == "dialog_name" && infoDat[1] == "leveldialog")
							{
								levels = true;
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "entrance") {
								isEntranceDialog = true;
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "gentrance") {
								isGEntrance = true;
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "trashdialog") {
								isTrashDialog = true;
							}
							if (isTrashDialog) {
								if (infoDat[0] == "trashitemcount") trashitemcount = infoDat[1];
							}
							if (levels) {
								if (infoDat[0] == "textlevel")
								{
									textlevel = infoDat[1];
									bool contains_non_int = !std::regex_match(textlevel, std::regex("^[0-9]+$"));

									if (contains_non_int == true) {
										GamePacket pfi = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`9Requesting offer failed... You may only use positive numbers!"));
										ENetPacket* packetfi = enet_packet_create(pfi.data,
											pfi.len,
											ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packetfi);

										delete pfi.data;
									}
									else
									{
										int a = atoi(textlevel.c_str());
										if (a > 0) {
											int level = ((PlayerInfo*)(peer->data))->level;
											int a = atoi(textlevel.c_str());
											int blevel = a * 10;
											int wls = ((PlayerInfo*)(peer->data))->premwl;
											((PlayerInfo*)(peer->data))->buygems = blevel;
											if (wls >= a)
											{
												GamePacket p2 = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`w\n\nadd_label_with_icon|big|`oPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_label|small|`4You'll give:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`o" + std::to_string(a) + "`w) Premium WLS``|left|242|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_label|small|`2You'll get:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`o" + std::to_string(blevel) + "`w) Level``|left|1488|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_button|confirmlevel|`wDo The Purchase!|noflags|0|0|\nadd_button|cancel|`oCancel|noflags|0|0|"));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);

												enet_peer_send(peer, 0, packet2);
												delete p2.data;
											}
											else
											{
												GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "`4Not enough WL's"));
												ENetPacket* packet = enet_packet_create(p.data,
													p.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet);
												delete p.data;
											}
										}
									}
								}
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "gemsdialog")
							{
								gems = true;
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "gachadialog")
							{
								gacha = true;
							}
							if (infoDat[0] == "dialog_name" && infoDat[1] == "bg_weather") isBG = true;
							if (infoDat[0] == "bg_pick") isBG = true;
							if (infoDat[0] == "dialog_name" && infoDat[1] == "stuff_weather") isStuff = true;
							if (infoDat[0] == "stuff_pick") isStuff = true;
							if (isStuff) {
								if (infoDat[0] == "stuff_pick") stuffITEM = infoDat[1];
								if (infoDat[0] == "stuff_gravity") stuff_gravity = infoDat[1];
								if (infoDat[0] == "stuff_spin") stuff_spin = atoi(infoDat[1].c_str());
								if (infoDat[0] == "stuff_invert") stuff_invert = atoi(infoDat[1].c_str());
							}
							if (isEntranceDialog) {
								if (infoDat[0] == "opentopublic") entranceresult = infoDat[1];
								if (entranceresult == "1") {
									world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].isOpened = true;
									updateEntrance(peer, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].foreground, ((PlayerInfo*)(peer->data))->wrenchx, ((PlayerInfo*)(peer->data))->wrenchy, true, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].background);
								}
								else {
									world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].isOpened = false;
									updateEntrance(peer, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].foreground, ((PlayerInfo*)(peer->data))->wrenchx, ((PlayerInfo*)(peer->data))->wrenchy, false, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].background);
								}
							}
							if (isGEntrance) {
								if (infoDat[0] == "gopentopublic") entranceresult = infoDat[1];
								if (gentranceresult == "1") {
									world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].isOpened = true;
									updateEntrance(peer, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].foreground, ((PlayerInfo*)(peer->data))->wrenchx, ((PlayerInfo*)(peer->data))->wrenchy, true, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].background);
								}
								else {
									world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].isOpened = false;
									updateEntrance(peer, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].foreground, ((PlayerInfo*)(peer->data))->wrenchx, ((PlayerInfo*)(peer->data))->wrenchy, false, world->items[((PlayerInfo*)(peer->data))->wrenchx + (((PlayerInfo*)(peer->data))->wrenchy * world->width)].background);
								}
							}
							if (isStuff) {
								if (world != NULL) {
									if (stuffITEM != "") {
										int x = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation % world->width;
										int y = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation / world->width;
										int id = atoi(stuffITEM.c_str());
										int stuffGra = atoi(stuff_gravity.c_str());
										world->stuff_invert = stuff_invert;
										world->stuff_spin = stuff_spin;
										world->stuff_gravity = stuffGra;
										world->stuffID = atoi(stuffITEM.c_str());
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
											if (isHere(peer, currentPeer)) {
												sendStuffweather(currentPeer, x, y, world->stuffID, stuffGra, stuff_spin, stuff_invert);
												getPlyersWorld(currentPeer)->weather = 29;
											}
										}
									}
									else {
										int x = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation % world->width;
										int y = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation / world->width;
										int stuffGra = atoi(stuff_gravity.c_str());
										world->stuff_invert = stuff_invert;
										world->stuff_spin = stuff_spin;
										world->stuff_gravity = stuffGra;
										for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
											if (isHere(peer, currentPeer)) {
												sendStuffweather(currentPeer, x, y, world->stuffID, stuffGra, stuff_spin, stuff_invert);
												getPlyersWorld(currentPeer)->weather = 29;
											}
										}
									}
									int x = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation % world->width;
									int y = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation / world->width;
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
										if (isHere(peer, currentPeer)) {
											OnSetCurrentWeather(currentPeer, 29);
											continue;
										}
									}
								}
							}
							if (isBG) {
							if (world != NULL) {
								if (infoDat[0] == "bg_pick") bgitem = infoDat[1];
								int id = atoi(bgitem.c_str());
								int x = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation % world->width;
								int y = ((PlayerInfo*)(peer->data))->wrenchedBlockLocation / world->width;
								if (world->bgID == id || infoDat.at(1) == "") {
									break;
								}
								if (getItemDef(id).blockType == BlockTypes::BACKGROUND) {
									world->bgID = atoi(bgitem.c_str());
									for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
										if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
										if (isHere(peer, currentPeer)) {
											OnSetCurrentWeather(currentPeer, 34);
											sendBackground(currentPeer, x, y, atoi(bgitem.c_str()));
											getPlyersWorld(currentPeer)->weather = 34;
										}
								else {
									packet::SendTalkSelf(peer, "You can only set background blocks to Weather Machine - Background!");
									}
								}
									break;
								}
							}
								}
							if (gems) {
								if (infoDat[0] == "textgems")
								{
									textgems = infoDat[1];
									bool contains_non_int
										= !std::regex_match(textgems, std::regex("^[0-9]+$"));

									if (contains_non_int == true) {
										packet::consolemessage(peer, "You can only use positive numbers.");
									}
									else
									{
										int a = atoi(textgems.c_str());
										if (a > 0) {
											std::ifstream ifsz("/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
											std::string acontent((std::istreambuf_iterator<char>(ifsz)),
												(std::istreambuf_iterator<char>()));
											int cgems = atoi(acontent.c_str());
											int bgems = a * 20000;
											int wls = ((PlayerInfo*)(peer->data))->premwl;
											((PlayerInfo*)(peer->data))->buygems = a;
											if (wls >= a)
											{
												string sukses = "set_default_color|`w\n\nadd_label_with_icon|big|`oPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_label|small|`4You'll give:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`o" + std::to_string(a) + "`w) Premium WLS``|left|242|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_label|small|`2You'll get:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`o" + std::to_string(bgems) + "`w) Gems``|left|112|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_button|confirmgems|`wDo The Purchase!|noflags|0|0|\nadd_button|cancel|`oCancel|noflags|0|0|";
												packet::dialog(peer, sukses);
											}
											else
											{
												packet::consolemessage(peer, "`4You don't have enough wl");
											}
										}
									}
								}
							}
							if (infoDat[0] == "buttonClicked") btn = infoDat[1];
							if (infoDat[0] == "dialog_name" && infoDat[1] == "guildconfirm") 
								isGuildDialog = true;
							if (infoDat[0] == "dialog_name" && infoDat[1] == "register")
							{
								isRegisterDialog = true;
							}
							if (isRegisterDialog) {
								if (infoDat[0] == "username") username = infoDat[1];
								if (infoDat[0] == "password") password = infoDat[1];
								if (infoDat[0] == "passwordverify") passwordverify = infoDat[1];
								if (infoDat[0] == "email") email = infoDat[1];
								if (infoDat[0] == "discord") discord = infoDat[1];
							}
							if (isGuildDialog) {
								if (infoDat[0] == "gname") guildName = infoDat[1];
								if (infoDat[0] == "gstatement") guildStatement = infoDat[1];
								if (infoDat[0] == "ggcflagbg") guildFlagBg = infoDat[1];
								if (infoDat[0] == "ggcflagfg") guildFlagFg = infoDat[1];
							}

							if (infoDat[0] == "dialog_name" && infoDat[1] == "epochweather")
								isEpoch = true;
							if (isEpoch) {
								if (infoDat[0] == "epochice") ice = atoi(infoDat[1].c_str());
								if (infoDat[0] == "epochvol") volcano = atoi(infoDat[1].c_str());
								if (infoDat[0] == "epochland") land = atoi(infoDat[1].c_str());
							}

							if (infoDat[0] == "buttonClicked" && infoDat[1] == "Submit_") captcha = true;
							if (captcha) {
								if (infoDat[0] == "captcha_answer") captchaanswer = infoDat[1];
							}
							if (infoDat[0] == "timedilation") {
								string settimedilation = infoDat[1];
								if (settimedilation.size() > 2 || settimedilation.size() <= 0 || settimedilation.size() < 1)
								{
									packet::SendTalkSelf(peer, "`wYou can't set Cycle time more than 2 digits");
									break;
								}
								if (settimedilation.find_first_not_of("0123456789") != string::npos)
								{
									packet::SendTalkSelf(peer, "`wYou can't set Cycle time with non digits");
									break;
								}
								if (stoi(settimedilation) < 0)
								{
									packet::SendTalkSelf(peer, "`wYou can't set Cycle time below 0 minute");
									break;
								}
								if (settimedilation == "")
								{
									packet::SendTalkSelf(peer, "`wYou can't set Cycle time below 0 minute");
									break;
								}
								((PlayerInfo*)(peer->data))->timedilation = stoi(infoDat[1]);
							}
						}
					}
					
					if (isTrashDialog) {
						int itemid = ((PlayerInfo*)(peer->data))->lasttrashitem;
						int x;
						try {
							x = stoi(trashitemcount);
						}
						catch (std::invalid_argument& e) {
							continue;
						}
						short int currentItemCount = 0;
						for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
							if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == itemid) {
								currentItemCount = (unsigned int)((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount;
							}
						}
						if (x <= 0 || x > currentItemCount) continue;
						if (itemid < 1 || itemid > maxItems) continue;
						bool iscontainseas = false;
						SearchInventoryItem(peer, itemid, 1, iscontainseas);
						if (!iscontainseas) break;
						((PlayerInfo*)(peer->data))->lastunt = itemid;
						((PlayerInfo*)(peer->data))->lastuntc = x;
						int id = ((PlayerInfo*)(peer->data))->lasttrashitem;
						packet::consolemessage(peer, "`o" + std::to_string(x) + " `w" + getItemDef(itemid).name + " `otrashed.");
						RemoveInventoryItem(((PlayerInfo*)(peer->data))->lasttrashitem, x, peer);
						packet::PlayAudio(peer, "trash.wav", 0);
						((PlayerInfo*)(peer->data))->lasttrashitem = 0;
					}
					
					
					if (isFindDialog && btn.substr(0, 4) == "tool") {
						if (has_only_digits(btn.substr(4, btn.length() - 4)) == false) break;
						int id = atoi(btn.substr(4, btn.length() - 4).c_str());
						int intid = atoi(btn.substr(4, btn.length() - 4).c_str());
						bool iscontains = false;
						string ide = btn.substr(4, btn.length() - 4).c_str();
						ItemDefinition gaydef;
						gaydef = getItemDef(id);

						if (id == 9350 || id == 11232 || id == 242 || id == 1796 || id == 7188 || id == 6802 || id == 1486) {
							if (((PlayerInfo*)(peer->data))->adminLevel < 1337) {
								packet::consolemessage(peer, "`4You must purchase this item");
								break;
							}
							else {}
						}
						if (id == 10456 || id == 7480 || id == 112 || id == 1790 || id == 1900 || id == 6336) {
							if (((PlayerInfo*)(peer->data))->adminLevel < 1337) {
								packet::consolemessage(peer, "This item is too heavy for you!");
								break;
							}
							else {
							}
						}
						for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
							if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == intid) {
								packet::consolemessage(peer, "`oSorry! Your inventory already contains this item!");
								iscontains = true;
							}
						}
						if (iscontains) {
							iscontains = false;
							continue;
						}
						if (CheckItemMaxed(peer, id, 200)) {
							packet::consolemessage(peer, "You already reached the max count of the item!");
							continue;
						}
						bool success = true;
						if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
							packet::consolemessage(peer, "`oItem `w" + getItemDef(intid).name + " `o(`w" + ide + "`o) With rarity : `o(`w" + std::to_string(getItemDef(intid).rarity) + "`o) `ohas been `2added `oto your inventory");
							packet::dialog(peer, "add_label_with_icon|big|`wItem Finder``|left|6016|\nadd_spacer|small|\nadd_label|small|`$Items Found :|left|\nadd_label_with_icon|small|`c" + getItemDef(intid).name + "``|left|" + ide + "|\nadd_textbox|`$Items ID `o[`c" + ide + "`o]``|\nadd_spacer|small|\nadd_textbox|`2Find Again :|\nadd_textbox|`$Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|`2Find the item!|\nadd_quick_exit|");
						}
						else {
							packet::consolemessage(peer, "`oItem `w" + getItemDef(intid).name + " `oWith rarity : `o(`w" + std::to_string(getItemDef(intid).rarity) + "`o) `ohas been `2added `oto your inventory");
							packet::dialog(peer, "add_label_with_icon|big|`wItem Finder``|left|6016|\nadd_spacer|small|\nadd_label|small|`$Items Found :|left|\nadd_label_with_icon|small|`c" + getItemDef(intid).name + "``|left|" + ide + "|\nadd_spacer|small|\nadd_textbox|`2Find Again :|\nadd_textbox|`$Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|`2Find the item!|\nadd_quick_exit|");
						}
						SaveShopsItemMoreTimes(intid, 200, peer, success);
					}

					else if (isFindDialog) {
						string itemLower2;
						vector<ItemDefinition> itemDefsfind;
						for (char c : itemFind) if (c < 0x20 || c>0x7A) goto SKIPFind;
						if (itemFind.length() < 3) goto SKIPFind3;
						for (const ItemDefinition& item : itemDefs)
						{
							string itemLower;
							for (char c : item.name) if (c < 0x20 || c>0x7A) goto SKIPFind2;
							if (!(item.id % 2 == 0)) goto SKIPFind2;
							itemLower2 = item.name;
							std::transform(itemLower2.begin(), itemLower2.end(), itemLower2.begin(), ::tolower);
							if (itemLower2.find(itemLower) != std::string::npos) {
								itemDefsfind.push_back(item);
							}
						SKIPFind2:;
						}
					SKIPFind3:;
						string listMiddle = "";
						string listFull = "";

						for (const ItemDefinition& item : itemDefsfind)
						{
							string kys = item.name;
							std::transform(kys.begin(), kys.end(), kys.begin(), ::tolower);
							string kms = itemFind;
							std::transform(kms.begin(), kms.end(), kms.begin(), ::tolower);
							if (kys.find(kms) != std::string::npos)
								listMiddle += "add_button_with_icon|tool" + to_string(item.id) + "|`$" + item.name + "``|left|" + to_string(item.id) + "||\n";
						}
						if (itemFind.length() < 3) {
							listFull = "add_textbox|`4Word is less then 3 letters!``|\nadd_spacer|small|\n";
							showWrong(peer, listFull, itemFind);
						}
						else if (itemDefsfind.size() == 0) {
							//listFull = "add_textbox|`4Found no item match!``|\nadd_spacer|small|\n";
							showWrong(peer, listFull, itemFind);

						}
						else {
							string finddialogs = "add_label_with_icon|big|`wFound item : " + itemFind + "``|left|6016|\nadd_spacer|small|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||20|\nend_dialog|findid|Cancel|Find the item!|\nadd_spacer|big|\n" + listMiddle + "add_quick_exit|\n";
							packet::dialog(peer, finddialogs);
						}
					}
				SKIPFind:;

					if (captcha) {
						int x;
						try {
							x = stoi(captchaanswer);
						}
						catch (std::invalid_argument& e) {
							continue;
						}
						string youareanalien = captchaanswer;
						if (x > hasil || x != hasil || captchaanswer.length() < 0 || captchaanswer == "") {
							packet::dialog(peer, "set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following numbers|left|\nadd_textbox|" + to_string(resultnbr1) + " + " + to_string(resultnbr2) + "|left|\nadd_text_input|captcha_answer|Answer:||32|\nadd_button|Captcha_|Submit|");
							break;
						}
						else {
							continue;
						}
					}

					if (isGuildDialog) {


						int GCState = PlayerDB::guildRegister(peer, guildName, guildStatement, guildFlagFg, guildFlagBg);
						if (GCState == -1) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oSpecial characters are not allowed in Guild name.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -2) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild name you've entered is too short.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -3) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild name you've entered is too long.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -4) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild name you've entered is already taken.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -5) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild Flag Background ID you've entered must be a number.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -6) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild Flag Foreground ID you've entered must be a number.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -7) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild Flag Background ID you've entered is too long or too short.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (GCState == -8) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oThe Guild Flag Foreground ID you've entered is too long or too short.``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						if (world->owner != ((PlayerInfo*)(peer->data))->rawName) {
							GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation|left|5814|\nadd_textbox|`4Oops! `oYou must make guild in world you owned!``|\nadd_text_input|gname|`oGuild Name:``|" + guildName + "|15|\nadd_text_input|gstatement|`oGuild Statement:``|" + guildStatement + "|40|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``|" + guildFlagBg + "|5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``|" + guildFlagFg + "|5|\n\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\n\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\n\nadd_spacer|small|\nend_dialog|guildconfirm|`wCancel``|`oCreate Guild``|\n"));
							ENetPacket* packet = enet_packet_create(ps.data,
								ps.len,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packet);

							delete ps.data;
						}
						else {
							if (GCState == 1) {

								((PlayerInfo*)(peer->data))->createGuildName = guildName;
								((PlayerInfo*)(peer->data))->createGuildStatement = guildStatement;


								((PlayerInfo*)(peer->data))->createGuildFlagBg = guildFlagBg;
								((PlayerInfo*)(peer->data))->createGuildFlagFg = guildFlagFg;

								GamePacket ps = packetEnd(appendString(appendString(createPacket(), "OnDialogRequest"), "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild|left|5814|\nadd_textbox|`1Guild Name: `o" + guildName + "``|\nadd_textbox|`1Guild Statement: `o" + guildStatement + "``|\nadd_label_with_icon|small|`1<-Guild Flag Background``|left|" + guildFlagBg + "|\nadd_label_with_icon|small|`1<-Guild Flag Foreground``|left|" + guildFlagFg + "|\n\nadd_spacer|small|\nadd_textbox|`oCost: `4250,000 Gems``|\n\nadd_spacer|small|\nadd_button|confirmcreateguild|`oCreate Guild``|\nend_dialog||`wCancel``||\n"));
								ENetPacket* packet = enet_packet_create(ps.data,
									ps.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);

								delete ps.data;

							}
						}
					}

					if (btn == "worldPublic") if (((PlayerInfo*)(peer->data))->rawName == getPlyersWorld(peer)->owner) getPlyersWorld(peer)->isPublic = true;
					if (btn == "worldPrivate") if (((PlayerInfo*)(peer->data))->rawName == getPlyersWorld(peer)->owner) getPlyersWorld(peer)->isPublic = false;
					if (btn == "backsocialportal") {
						string kamugay = "set_default_color|`w\n\nadd_label_with_icon|big|Social Portal``|left|1366|\n\nadd_spacer|small|\nadd_button|backonlinelist|Show Friends``|0|0|\nend_dialog||OK||\nadd_quick_exit|";
						packet::dialog(peer, kamugay);
					}
					if (btn == "frnoption") {
						string checkboxshit = "add_checkbox|checkbox_public|Show location to friends|1";
						string checkboxshits = "add_checkbox|checkbox_notifications|Show friend notifications|1";;
						string req = "set_default_color|`o\n\nadd_label_with_icon|big|`wFriend Options``|left|1366|\n\nadd_spacer|small|\n" + checkboxshit + "\n" + checkboxshits + "\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
						packet::dialog(peer, req);
					}
					if (btn == "open2") {
						WorldInfo* world = getPlyersWorld(peer);
						int x = 0;
						int y = 0;
						if (world->items[x + (y * world->width)].foreground == 9350) {

							int premwl = ((PlayerInfo*)(peer->data))->premwl;

							int valwls = 0;
							int valwl;
							valwls = rand() % 10;
							valwl = valwls + 1;
							((PlayerInfo*)(peer->data))->premwl = premwl + valwl;

							string message1 = "`9You found `2" + to_string(valwl) + " `9Premium WLS";
							packet::SendTalkSelf(peer, message1);
							std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


							if (ifff.fail()) {
								ifff.close();


							}
							if (ifff.is_open()) {
							}
							json j;
							ifff >> j; //load


							j["premwl"] = ((PlayerInfo*)(peer->data))->premwl;
							std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
							if (!o.is_open()) {
								cout << GetLastError() << endl;
								_getch();
							}

							o << j << std::endl;
						}
					}
					if (btn == "backonlinelist") {

						string onlinefrnlist = "";
						int onlinecount = 0;
						int totalcount = ((PlayerInfo*)(peer->data))->friendinfo.size();
						ENetPeer* currentPeer;

						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;

							string name = ((PlayerInfo*)(currentPeer->data))->rawName;
							if (find(((PlayerInfo*)(peer->data))->friendinfo.begin(), ((PlayerInfo*)(peer->data))->friendinfo.end(), name) != ((PlayerInfo*)(peer->data))->friendinfo.end()) {
								onlinefrnlist += "\nadd_button|onlinefrns_" + ((PlayerInfo*)(currentPeer->data))->rawName + "|`2ONLINE: `o" + ((PlayerInfo*)(currentPeer->data))->displayName + "``|0|0|";
								onlinecount++;

							}

						}
						if (totalcount == 0) {
							string singsing = "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_label|small|`oYou currently have no friends.  That's just sad.  To make some, click a person's wrench icon, then choose `5Add as friend`o.``|left|4|\n\nadd_spacer|small|\nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|backsocialportal|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, singsing);
						}
						else if (onlinecount == 0) {
							string kamusimp = "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\nadd_label|small|`oNone of your friends are currently online.``|left|4|\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|backsocialportal|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, kamusimp);
						}

						else {
							string ngl = "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online``|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|" + onlinefrnlist + "\n\nadd_spacer|small|\nadd_button|showoffline|`oShow offline``|0|0|\nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|backsocialportal|Back|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, ngl);
						}
					}
					if (btn == "showoffline") {
						string onlinelist = "";
						string offlinelist = "";
						string offname = "";
						int onlinecount = 0;
						int totalcount = ((PlayerInfo*)(peer->data))->friendinfo.size();
						vector<string>offliness = ((PlayerInfo*)(peer->data))->friendinfo;

						ENetPeer* currentPeer;

						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							string name = ((PlayerInfo*)(currentPeer->data))->rawName;

							if (find(((PlayerInfo*)(peer->data))->friendinfo.begin(), ((PlayerInfo*)(peer->data))->friendinfo.end(), name) != ((PlayerInfo*)(peer->data))->friendinfo.end()) {
								onlinelist += "\nadd_button|onlinefrns_" + ((PlayerInfo*)(currentPeer->data))->rawName + "|`2ONLINE: `o" + ((PlayerInfo*)(currentPeer->data))->displayName + "``|0|0|";
								onlinecount++;

								offliness.erase(std::remove(offliness.begin(), offliness.end(), name), offliness.end());
							}
						}
						for (std::vector<string>::const_iterator i = offliness.begin(); i != offliness.end(); ++i) {
							offname = *i;
							offlinelist += "\nadd_button|offlinefrns_" + offname + "|`4OFFLINE: `o" + offname + "``|0|0|";

						}

						/*if (onlinecount > 0) {
							string ureallygay = "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\n\nadd_spacer|small|\nadd_textbox|All of your friend are online!|\n\nadd_spacer|small| \n\nadd_spacer|small| \nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|backonlinelist|Back``|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, ureallygay);
						}
						else {*/
						string gaying = "set_default_color|`o\n\nadd_label_with_icon|big|`o" + std::to_string(onlinecount) + " of " + std::to_string(totalcount) + " `wFriends Online|left|1366|\n\nadd_spacer|small|\nadd_button|chc0|`wClose``|0|0|\nadd_spacer|small|" + offlinelist + "\nadd_spacer|small|\n\nadd_button|frnoption|`oFriend Options``|0|0|\nadd_button|backonlinelist|Back``|0|0|\nadd_button||`oClose``|0|0|\nadd_quick_exit|";
						packet::dialog(peer, gaying);
					}
					if (btn == "createguild") {
						string guildisigay = "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild Creation``|left|5814|\nadd_spacer|small|\nadd_text_input|gname|Guild Name: ||20|\nadd_text_input|gstatement|Guild Statement: ||100|\nadd_text_input|ggcflagbg|`oGuild Flag Background ID:``||5|\nadd_text_input|ggcflagfg|`oGuild Flag Foreground ID:``||5|\nadd_spacer|small|\nadd_textbox|`oConfirm your guild settings by selecting `2Create Guild `obelow to create your guild.|\nadd_spacer|small|\nadd_textbox|`8Remember`o: A guild can only be created in a world owned by you and locked with a `5World Lock`o!|\nadd_spacer|small|\nadd_textbox|`4Warning! `oThe guild name cannot be changed once you have confirmed the guild settings!|\nadd_spacer|small|\nend_dialog|guildconfirm|Cancel|Create Guild|\nadd_quick_exit|";
						packet::dialog(peer, guildisigay);
					}
					if (btn == "createguildinfo") {
						string gayguild = "set_default_color|`o\n\nadd_label_with_icon|big|`wGrow Guild|left|5814|\nadd_label|small|`oWelcome to Grow Guilds where you can create a Guild! With a Guild you can level up the Guild to add more members.``|left|4|\n\nadd_spacer|small|\nadd_textbox|`oYou will be charged `60 `oGems.``|\nadd_spacer|small|\nadd_button|createguild|`oCreate a Guild``|0|0|\nadd_button|backsocialportal|Back|0|0|\nend_dialog||Close||\nadd_quick_exit|";
						packet::dialog(peer, gayguild);
						/*packet::consolemessage(peer, "`w[`2+`w] `wThis option will be added soon!");*/
					}
					if (btn == "confirmcreateguild") {
						packet::consolemessage(peer, "You created guild");
						string guildName = ((PlayerInfo*)(peer->data))->createGuildName;
						string guildStatement = ((PlayerInfo*)(peer->data))->createGuildStatement;
						string fixedguildName = PlayerDB::getProperName(guildName);
						string guildFlagbg = ((PlayerInfo*)(peer->data))->createGuildFlagBg;
						string guildFlagfg = ((PlayerInfo*)(peer->data))->createGuildFlagFg;

						//guildmem.push_back(((PlayerInfo*)(peer->data))->rawName);

						std::ofstream o("guilds/" + fixedguildName + ".json");
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}
						json j;
						vector<string> test1s;
						vector<string>test2s;

						((PlayerInfo*)(peer->data))->guildMembers.push_back(((PlayerInfo*)(peer->data))->rawName);
						j["GuildName"] = ((PlayerInfo*)(peer->data))->createGuildName;
						j["GuildRawName"] = fixedguildName;
						j["GuildStatement"] = ((PlayerInfo*)(peer->data))->createGuildStatement;
						j["Leader"] = ((PlayerInfo*)(peer->data))->rawName;
						j["Co-Leader"] = test1s;
						j["Elder-Leader"] = test2s;
						j["Member"] = ((PlayerInfo*)(peer->data))->guildMembers;
						j["GuildLevel"] = 0;
						j["GuildExp"] = 0;
						j["GuildWorld"] = ((PlayerInfo*)(peer->data))->currentWorld;
						j["backgroundflag"] = stoi(((PlayerInfo*)(peer->data))->createGuildFlagBg);
						j["foregroundflag"] = stoi(((PlayerInfo*)(peer->data))->createGuildFlagFg);
						o << j << std::endl;

						((PlayerInfo*)(peer->data))->guild = guildName;
						((PlayerInfo*)(peer->data))->joinguild = true;
						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json x;
						ifff >> x; //load

						x["guild"] = ((PlayerInfo*)(peer->data))->guild;
						x["joinguild"] = ((PlayerInfo*)(peer->data))->joinguild; //edit

						std::ofstream y("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!y.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						y << x << std::endl;
					}
					if (btn == "showguild") {
						string onlinegmlist = "";
						string grole = "";
						int onlinecount = 0;
						string guildname = PlayerDB::getProperName(((PlayerInfo*)(peer->data))->guild);
						if (guildname != "") {
							try {
								std::ifstream ifff("guilds/" + guildname + ".json");
								if (ifff.fail()) {
									ifff.close();
									((PlayerInfo*)(peer->data))->guild = "";
									continue;
								}
								json j;
								ifff >> j;
								int gfbg, gffg, guildlvl, guildxp;
								string gstatement, gleader;
								vector<string> gmembers; vector<string> GE; vector<string> GC;
								gfbg = j["backgroundflag"];
								gffg = j["foregroundflag"];
								gstatement = j["GuildStatement"].get<std::string>();
								gleader = j["Leader"].get<std::string>();
								guildlvl = j["GuildLevel"];
								guildxp = j["GuildExp"];
								for (int i = 0; i < j["Member"].size(); i++) {
									gmembers.push_back(j["Member"][i]);
								}
								for (int i = 0; i < j["Elder-Leader"].size(); i++) {
									GE.push_back(j["Elder-Leader"][i]);
								}
								for (int i = 0; i < j["Co-Leader"].size(); i++) {
									GC.push_back(j["Co-Leader"][i]);
								}
								((PlayerInfo*)(peer->data))->guildlevel = guildlvl;
								((PlayerInfo*)(peer->data))->guildexp = guildxp;
								((PlayerInfo*)(peer->data))->guildBg = gfbg;
								((PlayerInfo*)(peer->data))->guildFg = gffg;
								((PlayerInfo*)(peer->data))->guildStatement = gstatement;
								((PlayerInfo*)(peer->data))->guildLeader = gleader;
								((PlayerInfo*)(peer->data))->guildMembers = gmembers;
								((PlayerInfo*)(peer->data))->guildGE = GE;
								((PlayerInfo*)(peer->data))->guildGC = GC;
								ifff.close();
							}
							catch (std::exception&) {
								SendConsole("showguild Critical error details: rawName(" + ((PlayerInfo*)(peer->data))->rawName + ")", "ERROR");
								enet_peer_disconnect_now(peer, 0);
							}
							catch (std::runtime_error&) {
								SendConsole("showguild Critical error details: name(" + ((PlayerInfo*)(peer->data))->rawName + ")", "ERROR");
								enet_peer_disconnect_now(peer, 0);
							}
							catch (...) {
								SendConsole("showguild Critical error details: name(" + ((PlayerInfo*)(peer->data))->rawName + ")", "ERROR");
								enet_peer_disconnect_now(peer, 0);
							}
						}
						for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
							string name = ((PlayerInfo*)(currentPeer->data))->rawName;
							if (find(((PlayerInfo*)(peer->data))->guildMembers.begin(), ((PlayerInfo*)(peer->data))->guildMembers.end(), name) != ((PlayerInfo*)(peer->data))->guildMembers.end()) {
								if (((PlayerInfo*)(currentPeer->data))->rawName == ((PlayerInfo*)(peer->data))->guildLeader) {
									onlinegmlist += "\nadd_button|onlinegm_" + ((PlayerInfo*)(currentPeer->data))->rawName + "|`2ONLINE: `o" + ((PlayerInfo*)(currentPeer->data))->tankIDName + " `e(GL)``|0|0|";
									onlinecount++;
								}
								else {
									onlinegmlist += "\nadd_button|onlinegm_" + ((PlayerInfo*)(currentPeer->data))->rawName + "|`2ONLINE: `o" + ((PlayerInfo*)(currentPeer->data))->tankIDName + " " + grole + "``|0|0|";
									onlinecount++;
								}
							}
						}
						int guildsize = 15;
						if (((PlayerInfo*)(peer->data))->guildlevel == 2) guildsize = 20;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 3) guildsize = 25;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 4) guildsize = 30;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 5) guildsize = 35;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 6) guildsize = 40;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 7) guildsize = 45;
						else if (((PlayerInfo*)(peer->data))->guildlevel == 8) guildsize = 50;
						if (((PlayerInfo*)(peer->data))->rawName == ((PlayerInfo*)(peer->data))->guildLeader) {
							string guilds = "set_default_color|`o\n\nadd_dual_layer_icon_label|big|`wGuild Home|left|" + std::to_string(((PlayerInfo*)(peer->data))->guildBg) + "|" + std::to_string(((PlayerInfo*)(peer->data))->guildFg) + "|1.0|0|\n\nadd_spacer|small|\nadd_textbox|`oGuild Name: " + guildname + "|left|\nadd_textbox|Guild Statement: " + ((PlayerInfo*)(peer->data))->guildStatement + "``|\nadd_textbox|`oGuild size: " + to_string(guildsize) + " members|\nadd_textbox|`oGuild Level: " + std::to_string(((PlayerInfo*)(peer->data))->guildlevel) + "|\n\nadd_spacer|small|\nadd_button|guildoffline|`wShow offline too``|0|0|\nadd_button|goguildhome|`wGo to Guild Home``|0|0|\nadd_button|guildleveluper|`wUpgrade Guild``|0|0|\nadd_button|EditStatement|`wEdit Guild Statement|0|0|\nadd_button|leavefromguild|`4Abandon Guild``|0|0|\n\nadd_spacer|small|\nadd_textbox|`5" + std::to_string(onlinecount) + " of " + std::to_string(((PlayerInfo*)(peer->data))->guildMembers.size()) + " `wGuild Members Online|" + onlinegmlist + "\n\nadd_spacer|small|\nadd_button||`wClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, guilds);
						}
						else {
							string guild = "set_default_color|`o\n\nadd_dual_layer_icon_label|big|`wGuild Home|left|" + std::to_string(((PlayerInfo*)(peer->data))->guildBg) + "|" + std::to_string(((PlayerInfo*)(peer->data))->guildFg) + "|1.0|0|\n\nadd_spacer|small|\nadd_textbox|`oGuild Name: " + guildname + "|left|\nadd_textbox|Guild Statement: " + ((PlayerInfo*)(peer->data))->guildStatement + "``|\nadd_textbox|`oGuild size: " + to_string(guildsize) + " members|\nadd_textbox|`oGuild Level: " + std::to_string(((PlayerInfo*)(peer->data))->guildlevel) + "|\n\nadd_spacer|small|\nadd_button|guildoffline|`wShow offline too``|0|0|\nadd_button|goguildhome|`wGo to Guild Home``|0|0|\nadd_button|leavefromguild|`4Leave from guild``|0|0|\n\nadd_spacer|small|\nadd_textbox|`5" + std::to_string(onlinecount) + " of " + std::to_string(((PlayerInfo*)(peer->data))->guildMembers.size()) + " `wGuild Members Online|" + onlinegmlist + "\n\nadd_spacer|small|\nadd_button||`wClose``|0|0|\nadd_quick_exit|";
							packet::dialog(peer, guild);
						}
					}
					if (btn == "buygems") {
						string gayinggems = "set_default_color|\nadd_label_with_icon|big|`wPurchase Gems``|left|112|\nadd_smalltext|`4Make sure to read this information clearly!``|left|\n\nadd_spacer|small|\nadd_textbox|Price: `320000 / 1 Growtopia World Lock.|left|\nadd_textbox|Duration: `w[`4~`w]|left|\nadd_textbox|Stock: `w[`4~`w]|\nadd_spacer|small|\nadd_textbox|`9Rules:|left|\nadd_smalltext|`91. `2Do not sell it to other people.|left|\nadd_smalltext|`92. `2Trying To Sell Your Gems To Other People Will Result Ban/Ipban.|left|\nadd_spacer|left|\nadd_textbox|`eHow To Buy:|\nadd_smalltext|`rIf u want buy `9Gems`r, Message `4@OWNER `ron Discord Server.``|left|\nadd_spacer|small|\nadd_textbox|`eWhen will i received my purchase:|\nadd_smalltext|`rYou will receive within `424`r hours after you have made your payment.|left|\n\nadd_textbox|`oHow much you want to buy??|\nadd_text_input|textgems|||100|\nend_dialog|gemsdialog|Cancel|OK|\n";
						packet::dialog(peer, gayinggems);
					}
					if (btn == "growmojis") {
						sendGrowmojis(peer);
					}
					if (btn == "buyminimod")
					{
						if (((PlayerInfo*)(peer->data))->adminLevel > 665) {
							packet::consolemessage(peer, "`4You can't longer purchase Mod Rank.");
							continue;
						}
						string gaymod = "set_default_color|\nadd_label_with_icon|big|`wPurchase Moderator``|left|278|\nadd_smalltext|`4Make sure to read this information clearly!``|left|\n\nadd_spacer|small|\nadd_textbox|Price: `3600 `9Premium Wls`w.|left|\nadd_textbox|Duration: `w[`4~`w]|left|\nadd_textbox|Stock: `w[`4~`w]|\nadd_spacer|small|\nadd_textbox|`9Rules:|left|\nadd_smalltext|`91. `2Do Not Abuse Your Role|left|\nadd_smalltext|`92. `2if you are going to ban people, make sure to have screenshots/video proof.|left|\nadd_smalltext|`93. `2Sharing Acoount will result in account loss.|left|\nadd_smalltext|`94. `2Trying to sell account will result in ip-banned.|left|\nadd_spacer|small|\nadd_textbox|`9Commands:|small|\nadd_smalltext|`eAll commands are displayed in /mhelp (moderator help).|small|\nadd_spacer|left|\nadd_textbox|`eHow To Buy:|\nadd_smalltext|`rIf u want buy `#@Moderator `rRank, Message `6Server Creator `ron Discord Server.``|left|\nadd_spacer|small|\nadd_textbox|`eWhen will i receive my purchase:|\nadd_smalltext|`rYou will receive it within `424`r hours after you have made your payment.|left|\nadd_button|buymodpremium|`wPurchase `^Moderator `wWith premium wls!|noflags|0|0|\nadd_spacer|small|\nend_dialog|gazette|Close||";
						packet::dialog(peer, gaymod);
					}
					if (btn == "buyvip")
					{
						if (((PlayerInfo*)(peer->data))->adminLevel > 443) {
							packet::consolemessage(peer, "`4You can't longer purchase VIP Rank.");
							continue;
						}
						string gayvip = "set_default_color|\nadd_label_with_icon|big|`wPurchase VIP``|left|6802|\nadd_smalltext|`4Make sure to read this information clearly!``|left|\n\nadd_spacer|small|\nadd_textbox|Price: `2200 `9Premium Wls`w.|left|\nadd_textbox|Duration: `w[`4~`w]|left|\nadd_textbox|Stock: `w[`4~`w]|\nadd_spacer|small|\nadd_textbox|`9Rules:|left|\nadd_smalltext|`91. `2Do Not Abuse Your Role|left|\nadd_smalltext|`94. `2Trying to sell account will result in ip-banned.|left|\nadd_spacer|small|\nadd_textbox|`9Commands:|small|\nadd_smalltext|`eAll commands are displayed in /vhelp (vip help).|small|\nadd_spacer|left|\nadd_textbox|`eHow To Buy:|\nadd_smalltext|``rIf u want buy `9VIP `rRank, Message `6SERVER Creator `ron Discord Server.``|left|\nadd_spacer|small|\nadd_textbox|`eWhen will i receive my purchase:|\nadd_smalltext|`rYou will receive it within `424`r hours after you have made your payment.|left|\nadd_button|confirmvip1|`wPurchase `1VIP `wWith Premium WL|noflags|0|0|\nadd_spacer|small|\nend_dialog|gazette|Close||";
						packet::dialog(peer, gayvip);
					}
					if (btn == "buygacha")
					{
						string gaycha = "set_default_color|\nadd_label_with_icon|big|`wPurchase Gacha``|left|6802|\nadd_smalltext|`4Make sure to read this information clearly!``|left|\n\nadd_spacer|small|\nadd_textbox|Price: `21 `9Premium Wls per chest`w.|left|\nadd_textbox|Benefit : Earn 0-10 WLS with this chest|left|\nadd_textbox|Stock: `w[`4~`w]|small|left|\nadd_spacer|small|\nadd_textbox|`eWhen will i receive my purchase:|\nadd_smalltext|`rYou will receive it within `424`r hours after you have made your payment.|left|\nadd_button|confirmgacha|`wPurchase `1Gacha `wWith Premium WL|noflags|0|0|\nadd_spacer|small|\nend_dialog|gazette|Close||";
						packet::dialog(peer, gaycha);
					}
					if (btn == "buymodpremium")
					{
						int premwl = ((PlayerInfo*)(peer->data))->premwl;
						if (premwl >= 600)
						{
							string buygaymod = "set_default_color|`w\n\nadd_label_with_icon|big|`oPurchase Confirmation``|left|1366|\nadd_spacer|small|\nadd_label|small|`4You'll give:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`o600`w) Premium WLS``|left|242|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_label|small|`2You'll get:|left|23|\nadd_spacer|small|\nadd_label_with_icon|small|`w(`oPermanent`w) Moderator Role``|left|32|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_spacer|small|\nadd_button|confirmmod1|`wDo The Purchase!|noflags|0|0|\nadd_button|cancel|`oCancel|noflags|0|0|";
							packet::dialog(peer, buygaymod);
						}
						else
						{
							packet::consolemessage(peer, "`4You don't have enough Premium Wls!");
						}
					}
					if (btn == "confirmgems") {
						std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						std::string acontent((std::istreambuf_iterator<char>(ifsz)),
							(std::istreambuf_iterator<char>()));
						int buygems = ((PlayerInfo*)(peer->data))->buygems;
						int buygemsz = buygems * 20000;
						int a = atoi(acontent.c_str());
						int aa = a + buygemsz;
						int cwl = ((PlayerInfo*)(peer->data))->premwl;
						int rwl = cwl - buygems;
						((PlayerInfo*)(peer->data))->premwl = rwl;
						ofstream myfile;
						myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						myfile << aa;
						myfile.close();
						GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), aa));
						ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packetsa);
						delete psa.data;

						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json j;
						ifff >> j; //load


						j["premwl"] = ((PlayerInfo*)(peer->data))->premwl; //edit



						std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						o << j << std::endl;
					}
					if (btn == "confirmlevel") {
						int level = ((PlayerInfo*)(peer->data))->level;
						int blevel = ((PlayerInfo*)(peer->data))->buygems;
						int tlevel = level + blevel;
						int wls = ((PlayerInfo*)(peer->data))->premwl;
						int minuswl = blevel / 10;
						int rwl = wls - minuswl;
						((PlayerInfo*)(peer->data))->premwl = rwl;
						((PlayerInfo*)(peer->data))->level = tlevel;
						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json j;
						ifff >> j; //load


						j["level"] = tlevel; //edit
						j["premwl"] = ((PlayerInfo*)(peer->data))->premwl = rwl;; //edit



						std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						o << j << std::endl;
					}
					if (btn == "confirmmod1") {
						int premwl = ((PlayerInfo*)(peer->data))->premwl;

						((PlayerInfo*)(peer->data))->premwl = premwl - 600;
						((PlayerInfo*)(peer->data))->adminLevel = 666;

						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json j;
						ifff >> j; //load


						j["premwl"] = ((PlayerInfo*)(peer->data))->premwl; //edit
						j["adminLevel"] = ((PlayerInfo*)(peer->data))->adminLevel; //edit



						std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						o << j << std::endl;
						string imie = ((PlayerInfo*)(peer->data))->rawName;
						string message2 = "`w** Player `3" + imie + " `wis the newest member of `bMODERATOR!`w **";
						packet::consolemessage(peer, "`2You bought moderator.");
						string text = "action|play_sfx\nfile|audio/double_chance.wav\ndelayMS|0\n";
						BYTE* data = new BYTE[5 + text.length()];
						BYTE zero = 0;
						int type = 3;
						memcpy(data, &type, 4);
						memcpy(data + 4, text.c_str(), text.length());
						memcpy(data + 4 + text.length(), &zero, 1);
						ENetPeer* currentPeer;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							packet::consolemessage(currentPeer, message2);
						}
						enet_peer_disconnect_later(peer, 0);
					}
					if (btn == "confirmvip1") {
						int premwl = ((PlayerInfo*)(peer->data))->premwl;

						((PlayerInfo*)(peer->data))->premwl = premwl - 200;
						((PlayerInfo*)(peer->data))->adminLevel = 444;

						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json j;
						ifff >> j; //load


						j["premwl"] = ((PlayerInfo*)(peer->data))->premwl; //edit
						j["adminLevel"] = ((PlayerInfo*)(peer->data))->adminLevel; //edit



						std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						o << j << std::endl;
						string imie = ((PlayerInfo*)(peer->data))->rawName;
						string message2 = "`w** Player `3" + imie + " `wis the newest member of `1VIP!`w **";

						string text = "action|play_sfx\nfile|audio/double_chance.wav\ndelayMS|0\n";
						BYTE* data = new BYTE[5 + text.length()];
						BYTE zero = 0;
						int type = 3;
						memcpy(data, &type, 4);
						memcpy(data + 4, text.c_str(), text.length());
						memcpy(data + 4 + text.length(), &zero, 1);
						ENetPeer* currentPeer;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							packet::consolemessage(peer, message2);
						}
						enet_peer_disconnect_later(peer, 0);
					}
					if (btn == "confirmgacha") {
						int premwl = ((PlayerInfo*)(peer->data))->premwl;

						((PlayerInfo*)(peer->data))->premwl = premwl - 1;

						std::ifstream ifff("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json");


						if (ifff.fail()) {
							ifff.close();


						}
						if (ifff.is_open()) {
						}
						json j;
						ifff >> j; //load


						j["premwl"] = ((PlayerInfo*)(peer->data))->premwl; //edit

						bool success = true;
						SaveShopsItemMoreTimes(9350, 1, peer, success);
						packet::consolemessage(peer, "`9You bought 1 Super Golden Booty Chest.");

						std::ofstream o("players/" + ((PlayerInfo*)(peer->data))->rawName + ".json"); //save
						if (!o.is_open()) {
							cout << GetLastError() << endl;
							_getch();
						}

						o << j << std::endl;
					}
					if (btn == "buylvl") {
						int level = ((PlayerInfo*)(peer->data))->level;
						if (level > 124) {
							packet::consolemessage(peer, "`4You can't longer purchase level.");
							continue;
						}
						string buygaylvl = "set_default_color|\nadd_label_with_icon|big|`wPurchase Level``|left|18|\nadd_smalltext|`4Make sure to read this information clearly!``|left|\n\nadd_spacer|small|\nadd_textbox|Price: `310 / 1 Growtopia World Lock.|left|\nadd_textbox|Duration: `w[`4~`w]|left|\nadd_textbox|Stock: `w[`4~`w]|\nadd_spacer|small|\nadd_textbox|`9Rules:|left|\nadd_smalltext|`91. `2Trying Sell Your Account Will Result Ipban.|left|\nadd_spacer|left|\nadd_textbox|`eHow To Buy:|\nadd_smalltext|`rIf u want buy `#2Level`r, Message `4@OWNER `ron Discord Server.``|left|\nadd_spacer|small|\nadd_textbox|`eWhen will i receive my purchase:|\nadd_smalltext|`rYou will receive it within `424`r hours after you have made your payment.|left|\nadd_text_input|textlevel|||100|\nend_dialog|leveldialog|Cancel|OK|\n";
						packet::dialog(peer, buygaylvl);
					}
					if (btn == "ingamerole") {
						int wl = ((PlayerInfo*)(peer->data))->premwl;

						string gaysstore = "set_default_color|`o\n\nadd_label_with_icon|big|`wWelcome to our store!``|left|1430|\nadd_label_with_icon|small|`9Your `9Premium WLS = `w " + to_string(wl) + "|noflags|242|\nadd_spacer|\nadd_smalltext|`5Deposit world in `2Your World Name`5, `91 Premium WL = 1 Real GT WL`5.|left|\nadd_button_with_icon|buyminimod||staticBlueFrame|276|\nadd_button_with_icon|buyvip||staticBlueFrame|274|\nadd_button_with_icon|buygpass1||staticBlueFrame|10410|\nadd_button_with_icon|buygacha||staticBlueFrame|9350|\nadd_button_with_icon|buygems||staticBlueFrame|112|\nadd_button_with_icon|buylvl||staticBlueFrame|1488||\nadd_button_with_icon||END_LIST|noflags|0|0|\nadd_button|continue|Close|";
						packet::dialog(peer, gaysstore);
					}

					if (isEpoch) {
						PlayerInfo* pinfo = ((PlayerInfo*)(peer->data));
						int x = pinfo->wrenchedBlockLocation % world->width;
						int y = pinfo->wrenchedBlockLocation / world->width;
						if (land == true) {
							world->land = land;
							world->weather = 40;
							ENetPeer* currentPeer;

							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
									continue;
								if (isHere(peer, currentPeer))
								{
									GamePacket p2 = packetEnd(appendInt(appendString(createPacket(), "OnSetCurrentWeather"), world->weather));
									ENetPacket* packet2 = enet_packet_create(p2.data,
										p2.len,
										ENET_PACKET_FLAG_RELIABLE);

									enet_peer_send(currentPeer, 0, packet2);
									delete p2.data;
									continue;
								}
							}

						}
						else {
							world->land = false;
						}
						if (volcano == true) {
							world->volcano = volcano;
							world->weather = 39;
							ENetPeer* currentPeer;

							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
									continue;
								if (isHere(peer, currentPeer))
								{
									GamePacket p2 = packetEnd(appendInt(appendString(createPacket(), "OnSetCurrentWeather"), world->weather));
									ENetPacket* packet2 = enet_packet_create(p2.data,
										p2.len,
										ENET_PACKET_FLAG_RELIABLE);

									enet_peer_send(currentPeer, 0, packet2);
									delete p2.data;
									continue;
								}
							}

						}
						else {
							world->volcano = false;
						}
						if (ice == true) {
							world->ice = ice;
							world->weather = 38;
							ENetPeer* currentPeer;

							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
									continue;
								if (isHere(peer, currentPeer))
								{
									GamePacket p2 = packetEnd(appendInt(appendString(createPacket(), "OnSetCurrentWeather"), world->weather));
									ENetPacket* packet2 = enet_packet_create(p2.data,
										p2.len,
										ENET_PACKET_FLAG_RELIABLE);

									enet_peer_send(currentPeer, 0, packet2);
									delete p2.data;
									continue;
								}
							}

						}
						else {
							world->ice = false;
						}

					}
#ifdef REGISTRATION
					if (isRegisterDialog) {

						int regState = PlayerDB::playerRegister(peer, username, password, passwordverify, email, discord);
						if (regState == 1) {
							packet::consolemessage(peer, "`rYour account has been created!``");
							gamepacket_t p;
							p.Insert("SetHasGrowID");
							p.Insert(1);
							p.Insert(username);
							p.Insert(password);
							((PlayerInfo*)(event.peer->data))->haveGrowId = true;
							p.CreatePacket(peer);

							enet_peer_disconnect_later(peer, 0);
						}
						else if(regState==-1) {
							packet::consolemessage(peer, "`rAccount creation has failed, because it already exists!``");
						}
						else if (regState == -2) {
							packet::consolemessage(peer, "`rAccount creation has failed, because the name is too short!``");
						}
						else if (regState == -3) {
							packet::consolemessage(peer, "`4Passwords mismatch!``");
						}
						else if (regState == -4) {
							packet::consolemessage(peer, "`4Account creation has failed, because email address is invalid!``");
						}
						else if (regState == -5) {
							packet::consolemessage(peer, "`4Account creation has failed, because Discord ID is invalid!``");
						}
					}
#endif
				}
				if (cch.find("action|trash\n|itemID|") == 0) {
					std::stringstream ss(cch);
					std::string to;
					int idx = -1;
					int count = -1;
					while (std::getline(ss, to, '\n')) {
						vector<string> infoDat = explode("|", to);
						if (infoDat.size() == 3) {
							if (infoDat[1] == "itemID") idx = atoi(infoDat[2].c_str());
							if (infoDat[1] == "count") count = atoi(infoDat[2].c_str());
						}
					}
					((PlayerInfo*)(peer->data))->lasttrashitem = idx;
					((PlayerInfo*)(peer->data))->lasttrashitemcount = count;
					int id = ((PlayerInfo*)(peer->data))->lasttrashitem;
					if (idx == -1) continue;
					if (itemDefs.size() < idx || idx < 0) continue;
					if (id < 1 || id > maxItems) continue;
					if (id == 6336 || id == 18 || id == 32 || id == 10456 || id == 7480) {
						packet::OnTextOverlay(peer, "`wYou'd be sorry if you lost that!");
						continue;
					}
					else {
						auto trash = 0;
						for (auto i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
							if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == ((PlayerInfo*)(peer->data))->lasttrashitem && ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount >= 1) {
								trash = ((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount;
							}
						}
						string gaetrash = "add_label_with_icon|big|`4Trash `w" + itemDefs.at(idx).name + "``|left|" + std::to_string(idx) + "|\nadd_textbox|`oHow many to `4destroy`o? `o(You have " + to_string(trash) + ")|\nadd_text_input|trashitemcount||0|5|\nend_dialog|trashdialog|Cancel|Ok|\n";
						packet::dialog(peer, gaetrash);
					}
				}
				string dropText = "action|drop\n|itemID|";
				if (cch.find(dropText) == 0)
				{
					sendDrop(peer, -1, ((PlayerInfo*)(peer->data))->x + (32 * (((PlayerInfo*)(peer->data))->isRotatedLeft?-1:1)), ((PlayerInfo*)(peer->data))->y, atoi(cch.substr(dropText.length(), cch.length() - dropText.length() - 1).c_str()), 1, 0, false);
					/*int itemID = atoi(cch.substr(dropText.length(), cch.length() - dropText.length() - 1).c_str());
					PlayerMoving data;
					data.packetType = 14;
					data.x = ((PlayerInfo*)(peer->data))->x;
					data.y = ((PlayerInfo*)(peer->data))->y;
					data.netID = -1;
					data.plantingTree = itemID;
					float val = 1; // item count
					BYTE val2 = 0; // if 8, then geiger effect
					
					BYTE* raw = packPlayerMoving(&data);
					memcpy(raw + 16, &val, 4);
					memcpy(raw + 1, &val2, 1);
					SendPacketRaw(4, raw, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);*/
				}
				if (cch.find("text|") != std::string::npos){
					PlayerInfo* pData = ((PlayerInfo*)(peer->data));
					if (str == "/mod")
					{
						((PlayerInfo*)(peer->data))->canWalkInBlocks = true;
						sendState(peer);
							/*PlayerMoving data;
							data.packetType = 0x14;
							data.characterState = 0x0; // animation
							data.x = 1000;
							data.y = 1;
							data.punchX = 0;
							data.punchY = 0;
							data.XSpeed = 300;
							data.YSpeed = 600;
							data.netID = ((PlayerInfo*)(peer->data))->netID;
							data.plantingTree = 0xFF;
							SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);*/
					}
					else if (str.substr(0, 7) == "/state ")
					{
						PlayerMoving data;
						data.packetType = 0x14;
						data.characterState = 0x0; // animation
						data.x = 1000;
						data.y = 0;
						data.punchX = 0;
						data.punchY = 0;
						data.XSpeed = 300;
						data.YSpeed = 600;
						data.netID = ((PlayerInfo*)(peer->data))->netID;
						data.plantingTree = atoi(str.substr(7, cch.length() - 7 - 1).c_str());
						SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
					}
					else if (str == "/save")
					{
						if (((PlayerInfo*)(peer->data))->adminLevel < 1337) {
							packet::consolemessage(peer, "You can't do that");
							break;
						}
						saveAllWorlds();
					}
					else if (str == "/howgay") {
						ENetPeer* currentPeer;
						int val = rand() % 100;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							if (isHere(peer, currentPeer))
							{
								GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`w" + ((PlayerInfo*)(peer->data))->displayName + " `oare `2" + std::to_string(val) + "% `wgay!"), 0));
								ENetPacket* packet2 = enet_packet_create(p2.data,
									p2.len,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(currentPeer, 0, packet2);
								delete p2.data;
								GamePacket p0 = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), ((PlayerInfo*)(peer->data))->displayName + " `ware `2%" + std::to_string(val) + " `wgay!"));
								ENetPacket* packet0 = enet_packet_create(p0.data,
									p0.len,
									ENET_PACKET_FLAG_RELIABLE);

								enet_peer_send(currentPeer, 0, packet0);
								delete p0.data;
							}
						}
					}
					else if (str.substr(0, 9) == "/givedev ") {
						if (((PlayerInfo*)(peer->data))->rawName == "rocki") {
							string name = str.substr(11, str.length());
							if ((str.substr(11, cch.length() - 11 - 1).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos)) continue;
							bool found = false;
							for (ENetPeer* currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED) continue;
								string name2 = ((PlayerInfo*)(currentPeer->data))->rawName;
								std::transform(name.begin(), name.end(), name.begin(), ::tolower);
								std::transform(name2.begin(), name2.end(), name2.begin(), ::tolower);
								if (name == name2) {
									packet::OnAddNotification(currentPeer, "`$You have been promoted to `6@Developer", "audio/hub_open.wav", "interface/atomic_button.rttex");
									((PlayerInfo*)(currentPeer->data))->adminLevel = 1337;
									ifstream fg("players/" + ((PlayerInfo*)(currentPeer->data))->rawName + ".json");
									json j;
									fg >> j;
									fg.close();
									j["adminLevel"] = ((PlayerInfo*)(currentPeer->data))->adminLevel;
									ofstream fs("players/" + ((PlayerInfo*)(currentPeer->data))->rawName + ".json");
									fs << j;
									fs.close();
									found = true;
									string name3;
									string namemsg = ((PlayerInfo*)(currentPeer->data))->rawName;
									name3 = "`6@" + ((PlayerInfo*)(currentPeer->data))->tankIDName;
									((PlayerInfo*)(currentPeer->data))->haveSuperSupporterName = true;
									((PlayerInfo*)(currentPeer->data))->displayName = name3;
									packet::OnNameChanged(currentPeer, ((PlayerInfo*)(currentPeer->data))->netID, name3);
								}
							}
							if (found) {
								cout << "Server operator " << ((PlayerInfo*)(peer->data))->rawName << " has giveowner to " << str.substr(7, cch.length() - 7 - 1) << "." << endl;
								packet::consolemessage(peer, "`oSuccessfully given `6@Developer `oto player " + name + ".");
							}
							else {
								packet::consolemessage(peer, "`4Player not found");
							}
						}
					}
					else if (str == "/unequip")
					{
						((PlayerInfo*)(peer->data))->cloth_hair = 0;
						((PlayerInfo*)(peer->data))->cloth_shirt = 0;
						((PlayerInfo*)(peer->data))->cloth_pants = 0;
						((PlayerInfo*)(peer->data))->cloth_feet = 0;
						((PlayerInfo*)(peer->data))->cloth_face = 0;
						((PlayerInfo*)(peer->data))->cloth_hand = 0;
						((PlayerInfo*)(peer->data))->cloth_back = 0;
						((PlayerInfo*)(peer->data))->cloth_mask = 0;
						((PlayerInfo*)(peer->data))->cloth_necklace = 0;
						sendClothes(peer);
					}
					else if (str == "/purchase")
					{
						int wl = ((PlayerInfo*)(peer->data))->premwl;

						string gaystore = "set_default_color|`o\n\nadd_label_with_icon|big|`wWelcome to our store!``|left|1430|\nadd_label_with_icon|small|`9Your `9Premium WLS = `w " + to_string(wl) + "|noflags|242|\nadd_spacer|\nadd_smalltext|`5Deposit world in `2Your World Name`5, `91 Premium WL = 1 Real GT WL`5.|left|\nadd_button_with_icon|buyminimod||staticBlueFrame|276|\nadd_button_with_icon|buyvip||staticBlueFrame|274|\nadd_button_with_icon|buygpass1||staticBlueFrame|10410|\nadd_button_with_icon|buygacha||staticBlueFrame|9350|\nadd_button_with_icon|buygems||staticBlueFrame|112|\nadd_button_with_icon|buylvl||staticBlueFrame|1488||\nadd_button_with_icon||END_LIST|noflags|0|0|\nadd_button|continue|Close|";
						packet::dialog(peer, gaystore);
					}
					else if (str == "/wizard")
					{
						((PlayerInfo*)(peer->data))->cloth_hair = 0;
						((PlayerInfo*)(peer->data))->cloth_shirt = 0;
						((PlayerInfo*)(peer->data))->cloth_pants = 0;
						((PlayerInfo*)(peer->data))->cloth_feet = 0;
						((PlayerInfo*)(peer->data))->cloth_face = 1790;
						((PlayerInfo*)(peer->data))->cloth_hand = 0;
						((PlayerInfo*)(peer->data))->cloth_back = 0;
						((PlayerInfo*)(peer->data))->cloth_mask = 0;
						((PlayerInfo*)(peer->data))->cloth_necklace = 0;
						((PlayerInfo*)(peer->data))->skinColor = 2;
						sendClothes(peer);
						packet::consolemessage(peer, "`^Legendary Wizard Set Mod has been Enabled! ");
					}
					else if (str.substr(0, 5) == "/find") {
						packet::dialog(peer, "add_label_with_icon|big|`wFind item Machine``|left|3802|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|`2Find the item!|\nadd_quick_exit|\n");
					}
					else if (str == "/mods") {
						string x;

						ENetPeer* currentPeer;

						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;

							if (getAdminLevel(((PlayerInfo*)(currentPeer->data))->rawName, ((PlayerInfo*)(currentPeer->data))->tankIDPass) > 665) {
								x.append("`#@" + ((PlayerInfo*)(currentPeer->data))->rawName + "``, ");
							}

						}
						x = x.substr(0, x.length() - 2);

						gamepacket_t p;
						p.Insert("OnConsoleMessage");
						p.Insert("``Moderators online: " + x);
						p.CreatePacket(peer);
					}
					else if (str.substr(0,6) == "/mute ") {
						if (getAdminLevel(((PlayerInfo*)(peer->data))->rawName, ((PlayerInfo*)(peer->data))->tankIDPass) > 665) {
							string name = str.substr(10, str.length());

							ENetPeer* currentPeer;

							bool found = false;

							for (currentPeer = server->peers;
								currentPeer < &server->peers[server->peerCount];
								++currentPeer)
							{
								if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
									continue;

								if (((PlayerInfo*)(currentPeer->data))->rawName == name) {
									found = true;
									if (((PlayerInfo*)(currentPeer->data))->taped) {
										((PlayerInfo*)(currentPeer->data))->taped = false;
										((PlayerInfo*)(currentPeer->data))->isDuctaped = false;
										
										packet::consolemessage(peer, "`2You are no longer duct-taped!");
										sendState(currentPeer);
										{
											gamepacket_t p;
											p.Insert("OnConsoleMessage");
											p.Insert("`2You have un duct-taped the player!");
											p.CreatePacket(peer);
										}
									}
									else {
										((PlayerInfo*)(currentPeer->data))->taped = true;
										((PlayerInfo*)(currentPeer->data))->isDuctaped = true;
										gamepacket_t p;
										p.Insert("OnConsoleMessage");
										p.Insert("`4You have been duct-taped!");
										p.CreatePacket(peer);
										sendState(currentPeer);
										{
											gamepacket_t p;
											p.Insert("OnConsoleMessage");
											p.Insert("`2You have duct-taped the player!");
											p.CreatePacket(peer);
										}
									}
								}
							}
							if (!found) {
								gamepacket_t p;
								p.Insert("OnConsoleMessage");
								p.Insert("`4Player not found!");
								p.CreatePacket(peer);
							}
						}
						else {
							gamepacket_t p;
							p.Insert("OnConsoleMessage");
							p.Insert("`4You need to have a higher admin-level to do that!");
							p.CreatePacket(peer);
						}
					}
					else if (str.substr(0, 6) == "/give ") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 1337) {
						packet::consolemessage(peer, "You can't do that");
						continue;
					}
					if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
						std::ifstream ifs("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
						std::string content((std::istreambuf_iterator<char>(ifs)),
							(std::istreambuf_iterator<char>()));

						int gembux = atoi(content.c_str());
						if (gembux < 1000000000) {
							ofstream myfile;
							myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
							myfile << str.substr(6, str.length());
							myfile.close();

							GamePacket pp = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), stoi(str.substr(6, cch.length() - 6 - 1))));
							ENetPacket* packetpp = enet_packet_create(pp.data,
								pp.len,
								ENET_PACKET_FLAG_RELIABLE);

							enet_peer_send(peer, 0, packetpp);
							delete pp.data;
						}
					}
				}
					else if (str == "/help"){
						packet::consolemessage(peer, "Supported commands are: /mods, /help, /mod, /unmod, /inventory, /item id, /team id, /color number, /who, /state number, /count, /sb message, /alt, /radio, /find, /unequip, /bc message, /weather id, /news, /mhelp");
					}
					else if (str == "/mhelp") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 666) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
					    packet::consolemessage(peer, "Supported commands are: /mods, /help, /mod, /unmod, /inventory, /item id, /team id, /color number, /who, /state number, /count, /sb message, /alt, /radio, /find, /unequip, /weather id, /news, /bc message, /jsb, /mhelp, /nick name, /invis, /asb, /msb");
					}

					else if (str.substr(0, 3) == "/p ") {
					if ((str.substr(3, cch.length() - 3 - 1).find_first_not_of("0123456789") != string::npos)) continue;
					if (((PlayerInfo*)(peer->data))->cloth_hand == 3300) break;
					bool contains_non_alpha = !std::regex_match(str.substr(3, cch.length() - 3 - 1), std::regex("^[0-9]+$"));
					if (contains_non_alpha == false) {
						if (stoi(str.substr(3, cch.length() - 3 - 1)) < 0) {
							packet::consolemessage(peer, "`oPlease `wenter `obetween `20-230`w!");
						}
						if (stoi(str.substr(3, cch.length() - 3 - 1)) > 2300) {
							packet::consolemessage(peer, "`oPlease `wenter `obetween `20-230`w!");
						}
						else {
							int effect = atoi(str.substr(3, cch.length() - 3 - 1).c_str());
							((PlayerInfo*)(peer->data))->peffect = 8421376 + effect;
							sendState(peer);
							sendPuncheffect(peer);
							packet::consolemessage(peer, "`oYour `2punch `oeffect has been updated to `w" + str.substr(3, cch.length() - 3 - 1));
						}
					}
					else {
						packet::consolemessage(peer, "`oPlease enter only `2numbers`o!");
					}
					}
					else if (str == "/news"){
					    sendGazette(peer);
                    }
					else if (str.substr(0, 6) == "/nick ") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 444) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
						string nam1e = "``" + str.substr(6, cch.length() - 6 - 1) + "``";
						((PlayerInfo*)(event.peer->data))->displayName = str.substr(6, cch.length() - 6 - 1);
						gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
						p.Insert("OnNameChanged");
						p.Insert(nam1e);
						ENetPeer * currentPeer;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							if (isHere(peer, currentPeer))
							{
								p.CreatePacket(currentPeer);
							}
						}
					}
					else if (str.substr(0, 6) == "/flag ") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 444) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
						int flagid = atoi(str.substr(6).c_str());
			
						gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
						p.Insert("OnGuildDataChanged");
						p.Insert(1);
						p.Insert(2);
						p.Insert(flagid);
						p.Insert(3);

						ENetPeer * currentPeer;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							if (isHere(peer, currentPeer))
							{
								p.CreatePacket(currentPeer);
							}
						}
					}
					else if (str.substr(0, 9) == "/weather ") {
							if (world->name != "ADMIN") {
								if (world->owner != "") {
									if (((PlayerInfo*)(peer->data))->rawName == world->owner || isSuperAdmin(((PlayerInfo*)(peer->data))->rawName, ((PlayerInfo*)(peer->data))->tankIDPass))

									{
										ENetPeer* currentPeer;

										for (currentPeer = server->peers;
											currentPeer < &server->peers[server->peerCount];
											++currentPeer)
										{
											if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
												continue;
											if (isHere(peer, currentPeer))
											{
												packet::consolemessage(peer, "`oPlayer `2" + ((PlayerInfo*)(peer->data))->displayName + "`o has just changed the world's weather!");

												gamepacket_t p;
												p.Insert("OnSetCurrentWeather");
												p.Insert(atoi(str.substr(9).c_str()));
												p.CreatePacket(currentPeer);
												continue;
											}
										}
									}
								}
							}
						}
					else if (str == "/count"){
						int count = 0;
						ENetPeer * currentPeer;
						string name = "";
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							count++;
						}
						packet::consolemessage(peer, "There are "+std::to_string(count)+" people online out of 1024 limit.");
					}
					else if (str.substr(0, 5) == "/asb "){
					if (((PlayerInfo*)(peer->data))->adminLevel < 666) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
						if (!canSB(((PlayerInfo*)(peer->data))->rawName, ((PlayerInfo*)(peer->data))->tankIDPass)) continue;
						if (str.find("player_chat") != std::string::npos) {
							continue;
						}

						gamepacket_t p;
						p.Insert("OnAddNotification");
						p.Insert("interface/atomic_button.rttex");
						p.Insert(str.substr(4, cch.length() - 4 - 1).c_str());
						p.Insert("audio/hub_open.wav");
						p.Insert(0);

						ENetPeer * currentPeer;
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							p.CreatePacket(currentPeer);
						}
					}

					else if (str.substr(0, 5) == "/fakeautoban") {

					gamepacket_t p;
					p.Insert("OnAddNotification");
					p.Insert("interface/atomic_button.rttex");
					p.Insert("Warning from Admin : You've been `4BANNED `$from Growtopia for 730 days.");
					p.Insert("audio/hub_open.wav");
					enet_peer_disconnect_later(peer, 0);
					p.Insert(0);

					ENetPeer* currentPeer;
					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;
						p.CreatePacket(currentPeer);
					}
					}

					else if (str == "/invis") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 666) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
						packet::consolemessage(peer, "`6" + str);
						if (!pData->isGhost) {

							packet::consolemessage(peer, "`oYour atoms are suddenly aware of quantum tunneling. (Ghost in the shell mod added)");

							gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
							p.Insert("OnSetPos");
							p.Insert(pData->x, pData->y);
							p.CreatePacket(peer);

							sendState(peer);
							sendClothes(peer);
							pData->isGhost = true;
						}
						else {
							packet::consolemessage(peer, "`oYour body stops shimmering and returns to normal. (Ghost in the shell mod removed)");
							gamepacket_t p(0, ((PlayerInfo*)(peer->data))->netID);
							p.Insert("OnSetPos");
							p.Insert(pData->x1, pData->y1);
							p.CreatePacket(peer);

							((PlayerInfo*)(peer->data))->isInvisible = false;
							sendState(peer);
							sendClothes(peer);
							pData->isGhost = false;
						}
					}
					
					else if (str == "/sb") {
					    packet::consolemessage(peer, "Usage : /sb <message>");
					}
					else if (str == "/bc") {
					packet::consolemessage(peer, "Usage : /bc <message>");
					}

					else if (str.substr(0, 4) == "/sb ") {
						using namespace std::chrono;
						if (((PlayerInfo*)(peer->data))->lastSB + 45000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
						{
							((PlayerInfo*)(peer->data))->lastSB = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
						}
						else {
							packet::consolemessage(peer, "Wait a minute before using the SB command again!");
							continue;
						}
						if (str.find("player_chat") != std::string::npos) {
							continue;
						}

						string name = ((PlayerInfo*)(peer->data))->displayName;
						gamepacket_t p;
						p.Insert("OnConsoleMessage");
						p.Insert("CP:0_PL:4_OID:_CT:[SB]_ `w** `5Super-Broadcast`` from `$`2" + name + "```` (in `$" + ((PlayerInfo*)(peer->data))->currentWorld + "``) ** :`` `# " + str.substr(4, cch.length() - 4 - 1));

						string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
						BYTE* data = new BYTE[5 + text.length()];
						BYTE zero = 0;
						int type = 3;
						memcpy(data, &type, 4);
						memcpy(data + 4, text.c_str(), text.length());
						memcpy(data + 4 + text.length(), &zero, 1);
						ENetPeer * currentPeer;
						
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							if (!((PlayerInfo*)(currentPeer->data))->radio)
								continue;
							
							p.CreatePacket(currentPeer);
							
							ENetPacket * packet2 = enet_packet_create(data,
								5+text.length(),
								ENET_PACKET_FLAG_RELIABLE);

							enet_peer_send(currentPeer, 0, packet2);
							
							//enet_host_flush(server);
						}
						delete[] data;
					}
					else if (str.substr(0, 5) == "/msb ") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 666) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
					using namespace std::chrono;
					if (((PlayerInfo*)(peer->data))->lastSB + 45000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
					{
						((PlayerInfo*)(peer->data))->lastSB = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
					}
					else {
						packet::consolemessage(peer, "Wait a minute before using the MSB command again!");
						continue;
					}
					if (str.find("player_chat") != std::string::npos) {
						continue;
					}

					string name = ((PlayerInfo*)(peer->data))->displayName;
					gamepacket_t p;
					p.Insert("OnConsoleMessage");
					p.Insert("`w`#[`bMOD-SB`#]`` `5** from `6" + name + "```` in `#[`4HIDDEN!`#] ** :`` `^ " + str.substr(5, cch.length() - 5 - 1));
					string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length());
					memcpy(data + 4 + text.length(), &zero, 1);
					ENetPeer* currentPeer;

					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;
						if (!((PlayerInfo*)(currentPeer->data))->radio)
							continue;

						p.CreatePacket(currentPeer);


						ENetPacket* packet2 = enet_packet_create(data,
							5 + text.length(),
							ENET_PACKET_FLAG_RELIABLE);

						enet_peer_send(currentPeer, 0, packet2);

						//enet_host_flush(server);
					}
					delete data;
					}
					else if (str.substr(0, 4) == "/bc ") {
					using namespace std::chrono;
					if (((PlayerInfo*)(peer->data))->lastSB + 45000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
					{
						((PlayerInfo*)(peer->data))->lastSB = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
					}
					else {
						packet::consolemessage(peer, "Wait a minute before using the Broadcast command again!");
						continue;
					}
					if (str.find("player_chat") != std::string::npos) {
						continue;
					}

					string name = ((PlayerInfo*)(peer->data))->displayName;
					gamepacket_t p;
					p.Insert("OnConsoleMessage");
					p.Insert("CP:0_PL:4_OID:_CT:[SB]_ `w** Broadcast`` `5from `$`2" + name + "```` (in `$" + ((PlayerInfo*)(peer->data))->currentWorld + "``) ** :`` `# " + str.substr(4, cch.length() - 4 - 1));

					string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
					BYTE* data = new BYTE[5 + text.length()];
					BYTE zero = 0;
					int type = 3;
					memcpy(data, &type, 4);
					memcpy(data + 4, text.c_str(), text.length());
					memcpy(data + 4 + text.length(), &zero, 1);
					ENetPeer* currentPeer;

					for (currentPeer = server->peers;
						currentPeer < &server->peers[server->peerCount];
						++currentPeer)
					{
						if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
							continue;
						if (!((PlayerInfo*)(currentPeer->data))->radio)
							continue;

						p.CreatePacket(currentPeer);

						ENetPacket* packet2 = enet_packet_create(data,
							5 + text.length(),
							ENET_PACKET_FLAG_RELIABLE);

						enet_peer_send(currentPeer, 0, packet2);

						//enet_host_flush(server);
					}
					delete[] data;
					}
					else if (str.substr(0, 5) == "/jsb ") {
					if (((PlayerInfo*)(peer->data))->adminLevel < 666) {
						packet::consolemessage(peer, "`4You can't do that.");
						continue;
					}
						using namespace std::chrono;
						if (((PlayerInfo*)(peer->data))->lastSB + 45000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count())
						{
							((PlayerInfo*)(peer->data))->lastSB = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
						}
						else {
							packet::consolemessage(peer, "Wait a minute before using the JSB command again!");
							continue;
						}
						if (str.find("player_chat") != std::string::npos) {
							continue;
						}

						string name = ((PlayerInfo*)(peer->data))->displayName;
						gamepacket_t p;
						p.Insert("OnConsoleMessage");
						p.Insert("`w** `5Super-Broadcast`` from `$`2" + name + "```` (in `4JAMMED``) ** :`` `^ " + str.substr(5, cch.length() - 5 - 1));
						string text = "action|play_sfx\nfile|audio/beep.wav\ndelayMS|0\n";
						BYTE* data = new BYTE[5 + text.length()];
						BYTE zero = 0;
						int type = 3;
						memcpy(data, &type, 4);
						memcpy(data + 4, text.c_str(), text.length());
						memcpy(data + 4 + text.length(), &zero, 1);
						ENetPeer * currentPeer;
						
						for (currentPeer = server->peers;
							currentPeer < &server->peers[server->peerCount];
							++currentPeer)
						{
							if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
								continue;
							if (!((PlayerInfo*)(currentPeer->data))->radio)
								continue;
							
							p.CreatePacket(currentPeer);
							
							
							ENetPacket * packet2 = enet_packet_create(data,
								5+text.length(),
								ENET_PACKET_FLAG_RELIABLE);

							enet_peer_send(currentPeer, 0, packet2);
							
							//enet_host_flush(server);
						}
						delete data;
					}
					
					
					else if (str.substr(0, 6) == "/radio") {
						gamepacket_t p;
						if (((PlayerInfo*)(peer->data))->radio) {
							p.Insert("OnConsoleMessage");
							p.Insert("You won't see broadcasts anymore.");
							((PlayerInfo*)(peer->data))->radio = false;
						} else {
							p.Insert("OnConsoleMessage");
							p.Insert("You will now see broadcasts again.");
							((PlayerInfo*)(peer->data))->radio = true;
						}
						p.CreatePacket(peer);
					}
					else if (str == "/unmod")
					{
						((PlayerInfo*)(peer->data))->canWalkInBlocks = false;
						sendState(peer);
					}
					else if (str == "/alt") {
					gamepacket_t p;
					p.Insert("OnSetBetaMode");
					p.Insert(1);
					}
					else
					if (str == "/inventory")
					{
						sendInventory(peer, ((PlayerInfo*)(peer->data))->inventory);
					} else
					if (str.substr(0,6) == "/item ")
					{
						if (((PlayerInfo*)(peer->data))->adminLevel < 1337) {
							packet::consolemessage(peer, "`4You can't do that.");
							continue;
						}
						bool success = true;

						int itemID = atoi(str.substr(6, cch.length() - 6 - 1).c_str());
						SaveShopsItemMoreTimes(itemID, 200, peer, success);

						PlayerInventory inventory = ((PlayerInfo *)(peer->data))->inventory;
						InventoryItem item;
						if (itemID != 112 && itemID != 18 && itemID != 32) {
							item.itemID = itemID;
							item.itemCount = 200;
							inventory.items.push_back(item);
							sendInventory(peer, inventory);
						}
					} else
					if (str.substr(0, 6) == "/team ")
					{
						int val = 0;
						val = atoi(str.substr(6, cch.length() - 6 - 1).c_str());
						PlayerMoving data;
						//data.packetType = 0x14;
						data.packetType = 0x1B;
						//data.characterState = 0x924; // animation
						data.characterState = 0x0; // animation
						data.x = 0;
						data.y = 0;
						data.punchX = val;
						data.punchY = 0;
						data.XSpeed = 0;
						data.YSpeed = 0;
						data.netID = ((PlayerInfo*)(peer->data))->netID;
						data.plantingTree = 0;
						SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);

					} else 
					if (str.substr(0, 7) == "/color ")
					{
						((PlayerInfo*)(peer->data))->skinColor = atoi(str.substr(6, cch.length() - 6 - 1).c_str());
						sendClothes(peer);
					}
					if (str.substr(0, 4) == "/who")
					{
						sendWho(peer);

					}
					if (str.length() && str[0] == '/')
					{
						sendAction(peer, ((PlayerInfo*)(peer->data))->netID, str);
					} else if (str.length()>0)
					{
						if (((PlayerInfo*)(peer->data))->taped == false) {
							sendChatMessage(peer, ((PlayerInfo*)(peer->data))->netID, str);
						}
						else {
							// Is duct-taped
							sendChatMessage(peer, ((PlayerInfo*)(peer->data))->netID, randomDuctTapeMessage(str.length()));
						}
					}
					
			}
				if (!((PlayerInfo*)(event.peer->data))->isIn)
				{
					if (itemdathash == 0) {
						enet_peer_disconnect_later(peer, 0);
					}

					gamepacket_t p;
					p.Insert("OnSuperMainStartAcceptLogonHrdxs47254722215a");
					p.Insert(itemdathash);
					p.Insert("ubistatic-a.akamaihd.net");
					p.Insert(configCDN);
					p.Insert("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
					p.Insert("proto=149|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|server_tick=226933875|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|");
					p.CreatePacket(peer);
					
					std::stringstream ss(GetTextPointerFromPacket(event.packet));
					std::string to;
					while (std::getline(ss, to, '\n')){
						string id = to.substr(0, to.find("|"));
						string act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
						if (id == "tankIDName")
						{
							((PlayerInfo*)(event.peer->data))->tankIDName = act;
							((PlayerInfo*)(event.peer->data))->haveGrowId = true;
						}
						else if(id == "tankIDPass")
						{
							((PlayerInfo*)(event.peer->data))->tankIDPass = act;
						}
						else if(id == "requestedName")
						{
							((PlayerInfo*)(event.peer->data))->requestedName = act;
						}
						else if (id == "country")
						{
							((PlayerInfo*)(event.peer->data))->country = act;
						}
					}
					if (!((PlayerInfo*)(event.peer->data))->haveGrowId)
					{
						((PlayerInfo*)(event.peer->data))->hasLogon = true;
						((PlayerInfo*)(event.peer->data))->rawName = "";
						((PlayerInfo*)(event.peer->data))->displayName = PlayerDB::fixColors(((PlayerInfo*)(event.peer->data))->requestedName.substr(0, ((PlayerInfo*)(event.peer->data))->requestedName.length()>15?15:((PlayerInfo*)(event.peer->data))->requestedName.length()));
					}
					else {
						((PlayerInfo*)(event.peer->data))->rawName = PlayerDB::getProperName(((PlayerInfo*)(event.peer->data))->tankIDName);
#ifdef REGISTRATION
						int logStatus = PlayerDB::playerLogin(peer, ((PlayerInfo*)(event.peer->data))->rawName, ((PlayerInfo*)(event.peer->data))->tankIDPass);
						if (logStatus == 1) {
							PlayerInfo* p = ((PlayerInfo*)(peer->data));
							std::ifstream ifff("players/" + PlayerDB::getProperName(p->rawName) + ".json");
							json j;
							ifff >> j;

							int adminLevel;
							int xp;
							int level;
							int premwl;
							int back;
							int hand;
							int shirt;
							int pants;
							int neck;
							int hair;
							int feet;
							bool joinguild;
							string guild;
							int mask;
							int ances;
							int face;
							int effect;
							int skin;
							adminLevel = j["adminLevel"];
							level = j["level"];
							xp = j["xp"];
							back = j["ClothBack"];
							face = j["ClothFace"];
							hand = j["ClothHand"];
							shirt = j["ClothShirt"];
							pants = j["ClothPants"];
							premwl = j["premwl"];
							neck = j["ClothNeck"];
							hair = j["ClothHair"];
							feet = j["ClothFeet"];
							mask = j["ClothMask"];
							ances = j["ClothAnces"];
							effect = j["effect"];
							skin = j["skinColor"];
							if (j.count("guild") == 1) {
								guild = j["guild"].get<string>();
							}
							else {
								guild = "";
							}
							if (j.count("joinguild") == 1) {
								joinguild = j["joinguild"];
							}

							p->adminLevel = adminLevel;
							p->level = level;
							p->xp = xp;
							p->premwl = premwl;
							p->cloth_back = back;
							p->cloth_hand = hand;
							p->cloth_face = face;
							p->cloth_hair = hair;
							p->cloth_feet = feet;
							p->guild = guild;
							p->joinguild = joinguild;
							p->cloth_pants = pants;
							p->cloth_necklace = neck;
							p->cloth_shirt = shirt;
							p->cloth_mask = mask;
							p->cloth_ances = ances;
							p->peffect = effect;
							p->skinColor = skin;

							updateAllClothes(peer);

							ifff.close();

							string guildname = PlayerDB::getProperName(((PlayerInfo*)(peer->data))->guild);
							if (guildname != "") {
								std::ifstream ifff("guilds/" + guildname + ".json");
								if (ifff.fail()) {
									ifff.close();
									cout << "[!] Failed loading guilds/" + guildname + ".json! From " + ((PlayerInfo*)(peer->data))->displayName + "." << endl;
									((PlayerInfo*)(peer->data))->guild = "";
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
								((PlayerInfo*)(peer->data))->guildBg = gfbg;
								((PlayerInfo*)(peer->data))->guildFg = gffg;
								((PlayerInfo*)(peer->data))->guildStatement = gstatement;
								((PlayerInfo*)(peer->data))->guildLeader = gleader;
								((PlayerInfo*)(peer->data))->guildMembers = gmembers;
								ifff.close();
							}
							std::ifstream ifsz("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
							std::string acontent((std::istreambuf_iterator<char>(ifsz)),
								(std::istreambuf_iterator<char>()));
							int ac = atoi(acontent.c_str());
							ofstream myfile;
							myfile.open("gemdb/" + ((PlayerInfo*)(peer->data))->rawName + ".txt");
							myfile << ac;
							myfile.close();
							GamePacket psa = packetEnd(appendInt(appendString(createPacket(), "OnSetBux"), ac));
							ENetPacket* packetsa = enet_packet_create(psa.data, psa.len, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(peer, 0, packetsa);
							delete psa.data;
							string gayname = ((PlayerInfo*)(peer->data))->rawName;
							packet::consolemessage(peer, "`rSuccessfully logged into your account `2(`9" + gayname + "`2)");
							((PlayerInfo*)(event.peer->data))->displayName = ((PlayerInfo*)(event.peer->data))->tankIDName;
							if (((PlayerInfo*)(peer->data))->adminLevel == 1337) {
								((PlayerInfo*)(event.peer->data))->displayName = "`6@" + ((PlayerInfo*)(event.peer->data))->tankIDName;
								((PlayerInfo*)(peer->data))->haveSuperSupporterName = true;
							}
							else if (((PlayerInfo*)(peer->data))->adminLevel == 999) {
								((PlayerInfo*)(event.peer->data))->displayName = "`4@" + ((PlayerInfo*)(event.peer->data))->tankIDName;
								((PlayerInfo*)(peer->data))->haveSuperSupporterName = true;
							}
							else if (((PlayerInfo*)(peer->data))->adminLevel == 777) {
								((PlayerInfo*)(event.peer->data))->displayName = "`c@" + ((PlayerInfo*)(event.peer->data))->tankIDName;
								((PlayerInfo*)(peer->data))->haveSuperSupporterName = true;
							}
							else if (((PlayerInfo*)(peer->data))->adminLevel == 666) {
								((PlayerInfo*)(event.peer->data))->displayName = "`#@" + ((PlayerInfo*)(event.peer->data))->tankIDName;
								((PlayerInfo*)(peer->data))->haveSuperSupporterName = true;
							}
							else if (((PlayerInfo*)(peer->data))->adminLevel == 444) {
								((PlayerInfo*)(event.peer->data))->displayName = "`w[`1VIP`w] " + ((PlayerInfo*)(event.peer->data))->tankIDName;
								((PlayerInfo*)(peer->data))->haveSuperSupporterName = true;
							}
							else {
								((PlayerInfo*)(event.peer->data))->displayName = ((PlayerInfo*)(event.peer->data))->tankIDName;
							}
						}
						else {
							packet::consolemessage(peer, "`rWrong username or password!``");
							enet_peer_disconnect_later(peer, 0);
						}
#else
						
						((PlayerInfo*)(event.peer->data))->displayName = PlayerDB::fixColors(((PlayerInfo*)(event.peer->data))->tankIDName.substr(0, ((PlayerInfo*)(event.peer->data))->tankIDName.length()>18 ? 18 : ((PlayerInfo*)(event.peer->data))->tankIDName.length()));
						if (((PlayerInfo*)(event.peer->data))->displayName.length() < 3) ((PlayerInfo*)(event.peer->data))->displayName = "Person that doesn't know how the name looks!";
#endif
					}
					for (char c : ((PlayerInfo*)(event.peer->data))->displayName) if (c < 0x20 || c>0x7A) ((PlayerInfo*)(event.peer->data))->displayName = "Bad characters in name, remove them!";
					
					if (((PlayerInfo*)(event.peer->data))->country.length() > 4)
					{
						((PlayerInfo*)(event.peer->data))->country = "|showGuild";
					}
					if (getAdminLevel(((PlayerInfo*)(event.peer->data))->rawName, ((PlayerInfo*)(event.peer->data))->tankIDPass) > 665)
					{
						((PlayerInfo*)(event.peer->data))->country = "rt";
					}
					if (getAdminLevel(((PlayerInfo*)(event.peer->data))->rawName, ((PlayerInfo*)(event.peer->data))->tankIDPass) > 0)
					{
						((PlayerInfo*)(event.peer->data))->country = "interface/cash_icon_overlay";
					}
					if (((PlayerInfo*)(event.peer->data))->level >= 125) {
						((PlayerInfo*)(event.peer->data))->country = "|showGuild|maxLevel";
					}

					gamepacket_t p2;
					p2.Insert("SetHasGrowID");
					p2.Insert(((PlayerInfo*)(event.peer->data))->haveGrowId);
					p2.Insert(((PlayerInfo*)(peer->data))->tankIDName);
					p2.Insert(((PlayerInfo*)(peer->data))->tankIDPass);
					p2.CreatePacket(peer);
				}
				string pStr = GetTextPointerFromPacket(event.packet);
				if (pStr.substr(0, 17) == "action|enter_game" && !((PlayerInfo*)(event.peer->data))->isIn)
				{
					ENetPeer* currentPeer;
					((PlayerInfo*)(event.peer->data))->isIn = true;
					sendWorldOffers(peer);

					// growmoji
					gamepacket_t  p;
					p.Insert("OnEmoticonDataChanged");
					p.Insert(0);
					p.Insert("(wl)|ā|1&(yes)|Ă|1&(no)|ă|1&(love)|Ą|1&(oops)|ą|1&(shy)|Ć|1&(wink)|ć|1&(tongue)|Ĉ|1&(agree)|ĉ|1&(sleep)|Ċ|1&(punch)|ċ|1&(music)|Č|1&(build)|č|1&(megaphone)|Ď|1&(sigh)|ď|1&(mad)|Đ|1&(wow)|đ|1&(dance)|Ē|1&(see-no-evil)|ē|1&(bheart)|Ĕ|1&(heart)|ĕ|1&(grow)|Ė|1&(gems)|ė|1&(kiss)|Ę|1&(gtoken)|ę|1&(lol)|Ě|1&(smile)|Ā|1&(cool)|Ĝ|1&(cry)|ĝ|1&(vend)|Ğ|1&(bunny)|ě|1&(cactus)|ğ|1&(pine)|Ĥ|1&(peace)|ģ|1&(terror)|ġ|1&(troll)|Ġ|1&(evil)|Ģ|1&(fireworks)|Ħ|1&(football)|ĥ|1&(alien)|ħ|1&(party)|Ĩ|1&(pizza)|ĩ|1&(clap)|Ī|1&(song)|ī|1&(ghost)|Ĭ|1&(nuke)|ĭ|1&(halo)|Į|1&(turkey)|į|1&(gift)|İ|1&(cake)|ı|1&(heartarrow)|Ĳ|1&(lucky)|ĳ|1&(shamrock)|Ĵ|1&(grin)|ĵ|1&(ill)|Ķ|1&");
					p.CreatePacket(peer);

					if (((PlayerInfo*)(peer->data))->haveGrowId) {
						packet::consolemessage(peer, "`2Growtopia Private Server`0 made with ENet");

						ifstream fg("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
						json j;
						fg >> j;
						fg.close();
						if (j["items"][0]["itemid"] != 18 || j["items"][1]["itemid"] != 32) {
							j["items"][0]["itemid"] = 18;
							j["items"][1]["itemid"] = 32;
							j["items"][0]["quantity"] = 1;
							j["items"][1]["quantity"] = 1;
							ofstream fs("inventory/" + ((PlayerInfo*)(peer->data))->rawName + ".json");
							fs << j;
							fs.close();
						}
						PlayerInventory inventory; {
							InventoryItem item;
							for (int i = 0; i < ((PlayerInfo*)(peer->data))->currentInventorySize; i++) {
								int itemid = j["items"][i]["itemid"];
								int quantity = j["items"][i]["quantity"];
								if (itemid != 0 && quantity != 0) {
									item.itemCount = quantity;
									item.itemID = itemid;
									inventory.items.push_back(item);
									sendInventory(peer, inventory);
								}
							}
						}
						((PlayerInfo*)(event.peer->data))->inventory = inventory;
						{
							sendGazette(peer);
						}
					}
				}
				if (strcmp(GetTextPointerFromPacket(event.packet), "action|refresh_item_data\n") == 0)
				{
					if (itemsDat != NULL) {
						ENetPacket * packet = enet_packet_create(itemsDat,
							itemsDatSize + 60,
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
						((PlayerInfo*)(peer->data))->isUpdating = true;
						//enet_peer_disconnect_later(peer, 0); // TODO: add this back, and fix it properly
						//enet_host_flush(server);
					}
					// TODO FIX refresh_item_data ^^^^^^^^^^^^^^
				}
				break;
			}
			default:
				cout << "Unknown packet type " << messageType << endl;
				break;
			case 3:
			{
				//cout << GetTextPointerFromPacket(event.packet) << endl;
				std::stringstream ss(GetTextPointerFromPacket(event.packet));
				std::string to;
				bool isJoinReq = false;
								
				while (std::getline(ss, to, '\n')) {
					string id = to.substr(0, to.find("|"));
					string act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
					if (id == "name" && isJoinReq)
					{
#ifdef TOTAL_LOG
						cout << "Entering some world..." << endl;
#endif
						if (!((PlayerInfo*)(peer->data))->hasLogon) break;
						try {
							if (act.length() > 30) {
								packet::consolemessage(peer, "`4Sorry, but world names with more than 30 characters are not allowed!");
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								gamepacket_t p;
								p.Insert("OnFailedToEnterWorld");
								p.Insert(1);
								p.CreatePacket(peer);

							} else {
								WorldInfo info = worldDB.get(act);
								sendWorld(peer, &info);

								int x = 3040;
								int y = 736;

								for (int j = 0; j < info.width*info.height; j++)
								{
									if (info.items[j].foreground == 6) {
										x = (j%info.width) * 32;
										y = (j / info.width) * 32;
									}
								}
								packet::onspawn(peer, "spawn|avatar\nnetID|" + std::to_string(cId) + "\nuserID|" + std::to_string(cId) + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(x) + "|" + std::to_string(y) + "\nname|``" + ((PlayerInfo*)(event.peer->data))->displayName + "``\ncountry|" + ((PlayerInfo*)(event.peer->data))->country + "\ninvis|0\nmstate|0\nsmstate|0\ntype|local\n");
								((PlayerInfo*)(event.peer->data))->netID = cId;
								onPeerConnect(peer);
								cId++;

								sendInventory(peer, ((PlayerInfo*)(event.peer->data))->inventory);

                                 if (((PlayerInfo*)(peer->data))->taped) {
									 ((PlayerInfo*)(peer->data))->isDuctaped = true;
									 sendState(peer);
								 }
							}
						}
						catch (int e) {
							if (e == 1) {
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								gamepacket_t p;
								p.Insert("OnFailedToEnterWorld");
								p.Insert(1);
								p.CreatePacket(peer);
								packet::consolemessage(peer, "You have exited the world.");
							}
							else if (e == 2) {
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								gamepacket_t p;
								p.Insert("OnFailedToEnterWorld");
								p.Insert(1);
								p.CreatePacket(peer);
								packet::consolemessage(peer, "You have entered bad characters in the world name!");
							}
							else if (e == 3) {
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								gamepacket_t p;
								p.Insert("OnFailedToEnterWorld");
								p.Insert(1);
								p.CreatePacket(peer);
								packet::consolemessage(peer, "Exit from what? Click back if you're done playing.");
							}
							else {
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								gamepacket_t p;
								p.Insert("OnFailedToEnterWorld");
								p.Insert(1);
								p.CreatePacket(peer);
								packet::consolemessage(peer, "I know this menu is magical and all, but it has its limitations! You can't visit this world!");
							}
						}
					}
						if (id == "action")
						{

							if (act == "join_request")
							{
								isJoinReq = true;
							}
							if (act == "quit_to_exit")
							{
								sendPlayerLeave(peer, (PlayerInfo*)(event.peer->data));
								((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
								sendWorldOffers(peer);

							}
							if (act == "quit")
							{
								enet_peer_disconnect_later(peer, 0);
							}
						}
					}
					break;
			}
			case 4:
			{
				{
					BYTE* tankUpdatePacket = GetStructPointerFromTankPacket(event.packet); 
					
					if (tankUpdatePacket)
					{
						PlayerMoving* pMov = unpackPlayerMoving(tankUpdatePacket);
						if (((PlayerInfo*)(event.peer->data))->isGhost) {
							((PlayerInfo*)(event.peer->data))->isInvisible = true;
							((PlayerInfo*)(event.peer->data))->x1 = pMov->x;
							((PlayerInfo*)(event.peer->data))->y1 = pMov->y;
							pMov->x = -1000000;
							pMov->y = -1000000;
						}
						
						switch (pMov->packetType)
						{
						case 0:
							((PlayerInfo*)(event.peer->data))->x = pMov->x;
							((PlayerInfo*)(event.peer->data))->y = pMov->y;
							((PlayerInfo*)(event.peer->data))->isRotatedLeft = pMov->characterState & 0x10;
							sendPData(peer, pMov);
							if (!((PlayerInfo*)(peer->data))->joinClothesUpdated)
							{
								((PlayerInfo*)(peer->data))->joinClothesUpdated = true;
								updateAllClothes(peer);
							}
							break;

						default:
							break;
						}
						PlayerMoving *data2 = unpackPlayerMoving(tankUpdatePacket);
						//cout << data2->packetType << endl;
						if (data2->packetType == 11)
						{
							sendCollect(peer, ((PlayerInfo*)(peer->data))->netID, data2->plantingTree);
						}
						if (data2->packetType == 7)
						{
							if(data2->punchX < world->width && data2->punchY < world->height)
							if (getItemDef(world->items[data2->punchX + (data2->punchY * world->width)].foreground).blockType == BlockTypes::MAIN_DOOR) {
									sendPlayerLeave(peer, (PlayerInfo*)(event.peer->data));
									((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
									sendWorldOffers(peer);

								}
						}
						if (data2->packetType == 10)
						{
							//cout << pMov->x << ";" << pMov->y << ";" << pMov->plantingTree << ";" << pMov->punchX << ";" << pMov->punchY << ";" << pMov->characterState << endl;
							int item = pMov->plantingTree;
							PlayerInfo* info = ((PlayerInfo*)(peer->data));
							ItemDefinition pro;
							pro = getItemDef(item);
							ItemDefinition def;
							try {
								def = getItemDef(pMov->plantingTree);
							}
							catch (int e) {
								goto END_CLOTHSETTER_FORCE;
							}
							if (pMov->plantingTree == 242)
							{
								for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
								{
									if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == 242)
									{
										if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 100)
										{
											bool isValid = SaveConvertedItem(1796, 1, peer);
											if (isValid)
											{

												RemoveInventoryItem(242, 100, peer);
												GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`wYou compressed 100 `2World Lock `winto a `2Diamond Lock`w!"), 0));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												packet::consolemessage(peer, "`oYou compressed 100 `2World Lock `ointo a `2Diamond Lock`o!");
											}
										}
									}
								}
								break;
							}
							if (pMov->plantingTree == 1796) // shatter
							{
								for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
								{
									if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == 1796)
									{
										if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 1)
										{
											bool isValid = SaveConvertedItem(242, 100, peer);
											if (isValid)
											{

												RemoveInventoryItem(1796, 1, peer);
												GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`wYou shattered a `2Diamond Lock `winto `2100 World Lock`w!"), 0));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												packet::consolemessage(peer, "`oYou shattered a `2Diamond Lock `winto `2100 World Lock`o!");
											}
										}
									}
								}
								break;
							}
							if (pMov->plantingTree == 7188) // shatter
							{
								for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
								{
									if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == 6802)
									{
										if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 1)
										{
											bool isValid = SaveConvertedItem(1796, 100, peer);
											if (isValid)
											{

												RemoveInventoryItem(7188, 1, peer);
												GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`wYou shattered `2Mega Growtoken `winto `2100 Growtoken`w!"), 0));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												packet::consolemessage(peer, "`oYou shattered `2Blue Gem Lock `winto `2100 Diamond Locks`o!");
											}
										}
									}
								}
								continue;
							}
							if (pMov->plantingTree == 1486) // shatter
							{
								for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
								{
									if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == 1486)
									{
										if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 100)
										{
											bool isValid = SaveConvertedItem(6802, 1, peer);
											if (isValid)
											{

												RemoveInventoryItem(1486, 100, peer);
												GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`wYou compressed `2100 Growtoken `winto a `2Mega Growtoken`w!"), 0));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												packet::consolemessage(peer, "`oYou compressed `2100 Growtoken `winto a `2Mega Growtoken`o!");
											}
										}
									}
								}
								continue;
							}
							if (pMov->plantingTree == 6802) // shatter
							{
								for (int i = 0; i < ((PlayerInfo*)(peer->data))->inventory.items.size(); i++)
								{
									if (((PlayerInfo*)(peer->data))->inventory.items[i].itemID == 6802)
									{
										if (((PlayerInfo*)(peer->data))->inventory.items[i].itemCount >= 1)
										{
											bool isValid = SaveConvertedItem(1486, 100, peer);
											if (isValid)
											{

												RemoveInventoryItem(6802, 1, peer);
												GamePacket p2 = packetEnd(appendIntx(appendString(appendIntx(appendString(createPacket(), "OnTalkBubble"), ((PlayerInfo*)(peer->data))->netID), "`wYou shattered `2Mega Growtoken `winto `2100 Growtoken`w!"), 0));
												ENetPacket* packet2 = enet_packet_create(p2.data,
													p2.len,
													ENET_PACKET_FLAG_RELIABLE);
												enet_peer_send(peer, 0, packet2);
												delete p2.data;
												packet::consolemessage(peer, "`oYou shattered `2Mega Growtoken `winto `2100 Growtoken`o!");
											}
										}
									}
								}
								break;
							}
							
							switch (def.clothType) {
							case 0:
								if (((PlayerInfo*)(event.peer->data))->cloth0 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth0 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth0 = pMov->plantingTree;
								break;
							case 1:
								if (((PlayerInfo*)(event.peer->data))->cloth1 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth1 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth1 = pMov->plantingTree;
								break;
							case 2:
								if (((PlayerInfo*)(event.peer->data))->cloth2 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth2 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth2 = pMov->plantingTree;
								break;
							case 3:
								if (((PlayerInfo*)(event.peer->data))->cloth3 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth3 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth3 = pMov->plantingTree;
								break;
							case 4:
								if (((PlayerInfo*)(event.peer->data))->cloth4 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth4 = 0;
									getAutoEffect(peer);
									packet::consolemessage(peer, itemDefs.at(pMov->plantingTree).effect);
									((PlayerInfo*)(peer->data))->noEyes = false;
									sendState(peer); //here
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth4 = pMov->plantingTree;
								packet::consolemessage(peer, itemDefs.at(pMov->plantingTree).effects);
								if (item == 1204) {
									((PlayerInfo*)(peer->data))->peffect = 8421386;
								}
								else if (item == 10128) {
									((PlayerInfo*)(peer->data))->peffect = 8421376 + 683;
								}
								else if (item == 138) {
									((PlayerInfo*)(peer->data))->peffect = 8421377;
								}
								else if (item == 2476) {
									((PlayerInfo*)(peer->data))->peffect = 8421415;
								}
								else {
									getAutoEffect(peer);
								}
								break;
							case 5:
								if (((PlayerInfo*)(event.peer->data))->cloth5 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth5 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth5 = pMov->plantingTree;
								if (pMov->plantingTree == 11440) {
									info->peffect = 8421376 + 111;
									sendState(peer);
									break;
								}
							case 6:
								if (((PlayerInfo*)(event.peer->data))->cloth6 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth6 = 0;
									((PlayerInfo*)(event.peer->data))->canDoubleJump = false;
									sendState(peer);
									break;
								}
								{
									((PlayerInfo*)(event.peer->data))->cloth6 = pMov->plantingTree;
									int item = pMov->plantingTree;
									if (item == 156 || item == 362 || item == 678 || item == 736 || item == 818 || item == 1206 || item == 1460 || item == 1550 || item == 1574 || item == 1668 || item == 1672 || item == 1674 || item == 1784 || item == 1824 || item == 1936 || item == 1938 || item == 1970 || item == 2254 || item == 2256 || item == 2258 || item == 2260 || item == 2262 || item == 2264 || item == 2390 || item == 2392 || item == 3120 || item == 3308 || item == 3512 || item == 4534 || item == 4986 || item == 5754 || item == 6144 || item == 6334 || item == 6694 || item == 6818 || item == 6842 || item == 1934 || item == 3134 || item == 6004 || item == 1780 || item == 2158 || item == 2160 || item == 2162 || item == 2164 || item == 2166 || item == 2168 || item == 2438 || item == 2538 || item == 2778 || item == 3858 || item == 350 || item == 998 || item == 1738 || item == 2642 || item == 2982 || item == 3104 || item == 3144 || item == 5738 || item == 3112 || item == 2722 || item == 3114 || item == 4970 || item == 4972 || item == 5020 || item == 6284 || item == 4184 || item == 4628 || item == 5322 || item == 4112 || item == 4114 || item == 3442) {
										((PlayerInfo*)(event.peer->data))->canDoubleJump = true;
									}
									else {
										((PlayerInfo*)(event.peer->data))->canDoubleJump = false;
									}
									// ^^^^ wings
									sendState(peer);
								}
								break;
							case 7:
								if (((PlayerInfo*)(event.peer->data))->cloth7 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth7 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth7 = pMov->plantingTree;
								break;
							case 8:
								if (((PlayerInfo*)(event.peer->data))->cloth8 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth8 = 0;
									getAutoEffect(peer);
									packet::consolemessage(peer, itemDefs.at(pMov->plantingTree).effect);
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth8 = pMov->plantingTree;
								if (pMov->plantingTree == 11232) {
									info->peffect = 8421376 + 224;
									sendState(peer);
									break;
								}
							case 9:
								if (((PlayerInfo*)(event.peer->data))->cloth9 == pMov->plantingTree)
								{
									((PlayerInfo*)(event.peer->data))->cloth9 = 0;
									break;
								}
								((PlayerInfo*)(event.peer->data))->cloth9 = pMov->plantingTree;
								break;							
							default:
#ifdef TOTAL_LOG
								cout << "Invalid item activated: " << pMov->plantingTree << " by " << ((PlayerInfo*)(event.peer->data))->displayName << endl;
#endif
								break;
							}
							sendClothes(peer);
							// activate item
						END_CLOTHSETTER_FORCE:;
						}
						if (data2->packetType == 18)
						{
							sendPData(peer, pMov);
							// add talk buble
						}
						if (data2->punchX != -1 && data2->punchY != -1) {
							//cout << data2->packetType << endl;
							if (data2->packetType == 3)
							{
								sendTileUpdate(data2->punchX, data2->punchY, data2->plantingTree, ((PlayerInfo*)(event.peer->data))->netID, peer);
							}
							else {

							}
							/*PlayerMoving data;
							//data.packetType = 0x14;
							data.packetType = 0x3;
							//data.characterState = 0x924; // animation
							data.characterState = 0x0; // animation
							data.x = data2->punchX;
							data.y = data2->punchY;
							data.punchX = data2->punchX;
							data.punchY = data2->punchY;
							data.XSpeed = 0;
							data.YSpeed = 0;
							data.netID = ((PlayerInfo*)(event.peer->data))->netID;
							data.plantingTree = data2->plantingTree;
							SendPacketRaw(4, packPlayerMoving(&data), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
							cout << "Tile update at: " << data2->punchX << "x" << data2->punchY << endl;*/
							
						}
						delete data2;
						delete pMov;
					}

					else {
						cout << "Got bad tank packet";
					}
					/*char buffer[2048];
					for (int i = 0; i < event->packet->dataLength; i++)
					{
					sprintf(&buffer[2 * i], "%02X", event->packet->data[i]);
					}
					cout << buffer;*/
				}
			}
			break;
			case 5:
				break;
			case 6:
				//cout << GetTextPointerFromPacket(event.packet) << endl;
				break;
			}
			enet_packet_destroy(event.packet);
			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
#ifdef TOTAL_LOG
			printf("Peer disconnected.\n");
#endif
			/* Reset the peer's client information. */
			/*ENetPeer* currentPeer;
			for (currentPeer = server->peers;
				currentPeer < &server->peers[server->peerCount];
				++currentPeer)
			{
				if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
					continue;

				GamePacket p = packetEnd(appendString(appendString(createPacket(), "OnConsoleMessage"), "Player `o" + ((PlayerInfo*)(event.peer->data))->tankIDName + "`o just left the game..."));
				ENetPacket * packet = enet_packet_create(p.data,
					p.len,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(currentPeer, 0, packet);
				enet_host_flush(server);
			}*/
			sendPlayerLeave(peer, (PlayerInfo*)(event.peer->data));
			((PlayerInfo*)(event.peer->data))->inventory.items.clear();
			delete (PlayerInfo*)event.peer->data;
			event.peer->data = NULL;
		}
	}
	cout << "Program ended??? Huh?" << endl;
	while (1);
	return 0;
}
