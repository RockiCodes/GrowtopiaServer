#pragma once

struct gamepacket_t
{
private:
	int index = 0;
	int len = 0;
	byte* packet_data = new byte[61];

public:
	gamepacket_t(int delay = 0, int NetID = -1) {

		len = 61;
		int MessageType = 0x4;
		int PacketType = 0x1;
		int CharState = 0x8;

		memset(packet_data, 0, 61);
		memcpy(packet_data, &MessageType, 4);
		memcpy(packet_data + 4, &PacketType, 4);
		memcpy(packet_data + 8, &NetID, 4);
		memcpy(packet_data + 16, &CharState, 4);
		memcpy(packet_data + 24, &delay, 4);
	};
	~gamepacket_t() {
		delete[] packet_data;
	}

	void Insert(string a) {
		byte* data = new byte[len + 2 + a.length() + 4];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x2;
		int str_len = a.length();
		memcpy(data + len + 2, &str_len, 4);
		memcpy(data + len + 6, a.data(), str_len);
		len = len + 2 + a.length() + 4;
		index++;
		packet_data[60] = (byte)index;
	}
	void Insert(int a) {
		byte* data = new byte[len + 2 + 4];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x9;
		memcpy(data + len + 2, &a, 4);
		len = len + 2 + 4;
		index++;
		packet_data[60] = (byte)index;
	}
	void Insert(unsigned int a) {
		byte* data = new byte[len + 2 + 4];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x5;
		memcpy(data + len + 2, &a, 4);
		len = len + 2 + 4;
		index++;
		packet_data[60] = (byte)index;
	}
	void Insert(float a) {
		byte* data = new byte[len + 2 + 4];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x1;
		memcpy(data + len + 2, &a, 4);
		len = len + 2 + 4;
		index++;
		packet_data[60] = (byte)index;
	}
	void Insert(float a, float b) {
		byte* data = new byte[len + 2 + 8];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x3;
		memcpy(data + len + 2, &a, 4);
		memcpy(data + len + 6, &b, 4);
		len = len + 2 + 8;
		index++;
		packet_data[60] = (byte)index;
	}
	void Insert(float a, float b, float c) {
		byte* data = new byte[len + 2 + 12];
		memcpy(data, packet_data, len);
		delete[] packet_data;
		packet_data = data;
		data[len] = index;
		data[len + 1] = 0x4;
		memcpy(data + len + 2, &a, 4);
		memcpy(data + len + 6, &b, 4);
		memcpy(data + len + 10, &c, 4);
		len = len + 2 + 12;
		index++;
		packet_data[60] = (byte)index;
	}
	void CreatePacket(ENetPeer* peer) {
		ENetPacket* packet = enet_packet_create(packet_data, len, 1);
		enet_peer_send(peer, 0, packet);
	}
};

struct GamePacket {
	BYTE* data;
	int len;
	int indexes;
};
GamePacket appendFloat(GamePacket p, float val) {
	BYTE* n = new BYTE[p.len + 2 + 4];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 1;
	memcpy(n + p.len + 2, &val, 4);
	p.len = p.len + 2 + 4;
	p.indexes++;
	return p;
}
GamePacket appendFloat(GamePacket p, float val, float val2) {
	BYTE* n = new BYTE[p.len + 2 + 8];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 3;
	memcpy(n + p.len + 2, &val, 4);
	memcpy(n + p.len + 6, &val2, 4);
	p.len = p.len + 2 + 8;
	p.indexes++;
	return p;
}
GamePacket appendFloat(GamePacket p, float val, float val2, float val3) {
	BYTE* n = new BYTE[p.len + 2 + 12];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 4;
	memcpy(n + p.len + 2, &val, 4);
	memcpy(n + p.len + 6, &val2, 4);
	memcpy(n + p.len + 10, &val3, 4);
	p.len = p.len + 2 + 12;
	p.indexes++;
	return p;
}
GamePacket appendInt(GamePacket p, int val) {
	BYTE* n = new BYTE[p.len + 2 + 4];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 9;
	memcpy(n + p.len + 2, &val, 4);
	p.len = p.len + 2 + 4;
	p.indexes++;
	return p;
}
GamePacket appendIntx(GamePacket p, int val) {
	BYTE* n = new BYTE[p.len + 2 + 4];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 5;
	memcpy(n + p.len + 2, &val, 4);
	p.len = p.len + 2 + 4;
	p.indexes++;
	return p;
}
GamePacket appendString(GamePacket p, string str) {
	BYTE* n = new BYTE[p.len + 2 + str.length() + 4];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	n[p.len] = p.indexes;
	n[p.len + 1] = 2;
	int sLen = str.length();
	memcpy(n + p.len + 2, &sLen, 4);
	memcpy(n + p.len + 6, str.c_str(), sLen);
	p.len = p.len + 2 + str.length() + 4;
	p.indexes++;
	return p;
}
GamePacket createPacket() {
	BYTE* data = new BYTE[61];
	string asdf = "0400000001000000FFFFFFFF00000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	for (int i = 0; i < asdf.length(); i += 2) {
		char x = ch2n(asdf[i]);
		x = x << 4;
		x += ch2n(asdf[i + 1]);
		memcpy(data + (i / 2), &x, 1);
		if (asdf.length() > 61 * 2) throw 0;
	}
	GamePacket packet;
	packet.data = data;
	packet.len = 61;
	packet.indexes = 0;
	return packet;
}
GamePacket packetEnd(GamePacket p) {
	BYTE* n = new BYTE[p.len + 1];
	memcpy(n, p.data, p.len);
	delete p.data;
	p.data = n;
	char zero = 0;
	memcpy(p.data + p.len, &zero, 1);
	p.len += 1;
	*(int*)(p.data + 56) = p.indexes;
	*(BYTE*)(p.data + 60) = p.indexes;
	return p;
}
