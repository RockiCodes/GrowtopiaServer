#pragma once
#include "stdafx.h"
#include <iostream>

#include "enet/enet.h"
#include <filesystem>
#include <experimental/filesystem>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <sstream>
#include <chrono>
#include <fstream>
#include "json.hpp"
#include "bcrypt.h"
#include "crypt_blowfish/crypt_gensalt.cpp"
#include "crypt_blowfish/crypt_blowfish.h"
#include "crypt_blowfish/crypt_blowfish.cpp"
#include "crypt_blowfish/ow-crypt.cpp"
#include "bcrypt.cpp"
#include <regex>
#include <thread> // TODO
#include <mutex> // TODO
#pragma warning(disable : 4996)

using namespace std;
using json = nlohmann::json;
string newslist = "set_default_color|`o\n\nadd_label_with_icon|big|`wThe Growtopia Gazette``|left|5016|\n\nadd_spacer|small|\nadd_label_with_icon|small|`4WARNING:`` `5Worlds (and accounts)`` might be deleted at any time if database issues appear (once per day or week).|left|4|\nadd_label_with_icon|small|`4WARNING:`` `5Accounts`` are in beta, bugs may appear and they will be probably deleted often, because of new account updates, which will cause database incompatibility.|left|4|\nadd_spacer|small|\n\nadd_url_button||``Watch: `1Watch a video about GT Private Server``|NOFLAGS|https://www.youtube.com/watch?v=_3avlDDYBBY|Open link?|0|0|\nadd_url_button||``Channel: `1Watch Growtopia Noobs' channel``|NOFLAGS|https://www.youtube.com/channel/UCLXtuoBlrXFDRtFU8vPy35g|Open link?|0|0|\nadd_url_button||``Items: `1Item database by Nenkai``|NOFLAGS|https://raw.githubusercontent.com/Nenkai/GrowtopiaItemDatabase/master/GrowtopiaItemDatabase/CoreData.txt|Open link?|0|0|\nadd_url_button||``Discord: `1GT Private Server Discord``|NOFLAGS|https://discord.gg/8WUTs4v|Open the link?|0|0|\nadd_quick_exit|\n\nend_dialog|gazette|Close||";

#define REGISTRATION
#include <signal.h>
ENetHost* server;
int cId = 1;
BYTE* itemsDat = 0;
int itemsDatSize = 0;
int hasil = 0;
int prize = 0;
int resultnbr1 = 0;
bool serverIsFrozen = false;
bool DailyMaths = false;
int resultnbr2 = 0;
long long int quest = 0;
std::vector<std::thread> threads;
int configPort = 17091;
string configCDN = "0098/78237/cache/";

bool has_only_digits(const string str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}
bool has_only_digits_wnegative(const string str) {
	return str.find_first_not_of("-0123456789") == string::npos;
}
string hashPassword(string password) {
	char salt[BCRYPT_HASHSIZE];
	char hash[BCRYPT_HASHSIZE];
	int ret;

	ret = bcrypt_gensalt(12, salt);
	assert(ret == 0);
	ret = bcrypt_hashpw(password.c_str(), salt, hash);
	assert(ret == 0);
	return hash;
}

void sendData(ENetPeer* peer, int num, char* data, int len)
{
	/* Create a reliable packet of size 7 containing "packet\0" */
	ENetPacket* packet = enet_packet_create(0,
		len + 5,
		ENET_PACKET_FLAG_RELIABLE);
	/* Extend the packet so and append the string "foo", so it now */
	/* contains "packetfoo\0"                                      */
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	memcpy(packet->data, &num, 4);
	if (data != NULL)
	{
		memcpy(packet->data + 4, data, len);
	}
	char zero = 0;
	memcpy(packet->data + 4 + len, &zero, 1);
	enet_peer_send(peer, 0, packet);
	enet_host_flush(server);
}

int getPacketId(char* data)
{
	return *data;
}

char* getPacketData(char* data)
{
	return data + 4;
}

string text_encode(char* text)
{
	string ret = "";
	while (text[0] != 0)
	{
		switch (text[0])
		{
		case '\n':
			ret += "\\n";
			break;
		case '\t':
			ret += "\\t";
			break;
		case '\b':
			ret += "\\b";
			break;
		case '\\':
			ret += "\\\\";
			break;
		case '\r':
			ret += "\\r";
			break;
		default:
			ret += text[0];
			break;
		}
		text++;
	}
	return ret;
}

int ch2n(char x)
{
	switch (x)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	default:
		break;
	}
}


char* GetTextPointerFromPacket(ENetPacket* packet)
{
	char zero = 0;
	memcpy(packet->data + packet->dataLength - 1, &zero, 1);
	return (char*)(packet->data + 4);
}

BYTE* GetStructPointerFromTankPacket(ENetPacket* packet)
{
	unsigned int packetLenght = packet->dataLength;
	BYTE* result = NULL;
	if (packetLenght >= 0x3C)
	{
		BYTE* packetData = packet->data;
		result = packetData + 4;
		if (*(BYTE*)(packetData + 16) & 8)
		{
			if (packetLenght < *(int*)(packetData + 56) + 60)
			{
				cout << "Packet too small for extended packet to be valid" << endl;
				cout << "Sizeof float is 4.  TankUpdatePacket size: 56" << endl;
				result = 0;
			}
		}
		else
		{
			int zero = 0;
			memcpy(packetData + 56, &zero, 4);
		}
	}
	return result;
}

int GetMessageTypeFromPacket(ENetPacket* packet)
{
	int result;

	if (packet->dataLength > 3u)
	{
		result = *(packet->data);
	}
	else
	{
		cout << "Bad packet length, ignoring message" << endl;
		result = 0;
	}
	return result;
}


vector<string> explode(const string& delimiter, const string& str)
{
	vector<string> arr;

	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i < strleng)
	{
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng)//found delimiter
		{
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else
		{
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}

int maxItems = 11454;
std::mutex m;

void SendConsole(const string text, const string type) {
	time_t currentTime; time(&currentTime); const auto localTime = localtime(&currentTime);
	const auto Hour = localTime->tm_hour; const auto Min = localTime->tm_min; const auto Sec = localTime->tm_sec;
	if (type != "CHAT") {
		cout << "[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + " " + type + """]: " << text << endl;
	}
}

string getStrUpper(string txt) {
	string ret;
	for (char c : txt) ret += toupper(c);
	return ret;
}

std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

uint32_t HashString(unsigned char* str, int len)
{
	if (!str) return 0;

	unsigned char* n = (unsigned char*)str;
	uint32_t acc = 0x55555555;

	if (len == 0)
	{
		while (*n)
			acc = (acc >> 27) + (acc << 5) + *n++;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			acc = (acc >> 27) + (acc << 5) + *n++;
		}
	}
	return acc;

}

unsigned char* getA(string fileName, int* pSizeOut, bool bAddBasePath, bool bAutoDecompress)
{
	unsigned char* pData = NULL;
	FILE* fp = fopen(fileName.c_str(), "rb");
	if (!fp)
	{
		cout << "File not found" << endl;
		if (!fp) return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*pSizeOut = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pData = (unsigned char*)new unsigned char[((*pSizeOut) + 1)];
	if (!pData)
	{
		printf("Out of memory opening %s?", fileName.c_str());
		return 0;
	}
	pData[*pSizeOut] = 0;
	fread(pData, *pSizeOut, 1, fp);
	fclose(fp);

	return pData;
}

void SendPacket(int a1, string a2, ENetPeer* enetPeer)
{
	if (enetPeer)
	{
		ENetPacket* v3 = enet_packet_create(0, a2.length() + 5, 1);
		memcpy(v3->data, &a1, 4);
		memcpy((v3->data) + 4, a2.c_str(), a2.length());
		enet_peer_send(enetPeer, 0, v3);
	}
}

void SendPacketRaw(int a1, void* packetData, size_t packetDataSize, void* a4, ENetPeer* peer, int packetFlag)
{
	ENetPacket* p;

	if (peer)
	{
		if (a1 == 4 && *((BYTE*)packetData + 12) & 8)
		{
			p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
			int four = 4;
			memcpy(p->data, &four, 4);
			memcpy((char*)p->data + 4, packetData, packetDataSize);
			memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
			enet_peer_send(peer, 0, p);
		}
		else
		{
			p = enet_packet_create(0, packetDataSize + 5, packetFlag);
			memcpy(p->data, &a1, 4);
			memcpy((char*)p->data + 4, packetData, packetDataSize);
			enet_peer_send(peer, 0, p);
		}
	}
	delete (char*)packetData;
}

static inline void ltrim(string& s)
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
		return !isspace(ch);
		}));
}

static inline void rtrim(string& s)
{
	s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
		return !isspace(ch);
		}).base(), s.end());
}

static inline void trim(string& s)
{
	ltrim(s);
	rtrim(s);
}

static inline string trimString(string s)
{
	trim(s);
	return s;
}

int countSpaces(string& str)
{
	int count = 0;
	int length = str.length();
	for (int i = 0; i < length; i++)
	{
		int c = str[i];
		if (isspace(c))
			count++;
	}
	return count;
}