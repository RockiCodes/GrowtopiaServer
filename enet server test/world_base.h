#pragma once

struct BlockVisual {
	int packetType;
	int characterState;
	int punchX;
	int punchY;
	float x;
	float y;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int charStat;
	int blockid;
	int visual;
	int signs;
	int backgroundid;
	int displayblock;
	int time;
	int netID;
	//int bpm;
};

struct WorldItem {
	__int16 foreground = 0;
	__int16 background = 0;
	int breakLevel = 0;
	long long int breakTime = 0;
	int PosFind = 0;
	bool isLocked = false;
	int displayblock;
	bool rotatedLeft = false;
	bool water = false;
	bool fire = false;
	bool glue = false;
	bool red = false;
	bool green = false;
	bool blue = false;

	int clothHair = 0;
	int clothHead = 0;
	int clothMask = 0;
	int clothHand = 0;
	int clothNeck = 0;
	int clothShirt = 0;
	int clothPants = 0;
	int clothFeet = 0;
	int clothBack = 0;

	int dropItem = 0;
	int amount = 0; // like this

	string text = "";
	string signn = "";

	vector<string> mailbox;

	int gravity = 0;
	bool flipped = false;
	bool active = false;
	bool silenced = false;
	int16_t lockId = 0;
	string label = "";
	string destWorld = "";
	string destId = "";
	string currId = "";
	string password = "";
	int intdata = 0;
	bool activated = false;
	int displayBlock = 0;
	bool isOpened = false;
};

struct DroppedItem { // TODO
	int id;
	int uid;
	int count;
	int x;
	int y;
};

struct WorldInfo {
	int width = 100;
	int height = 60;
	string name = "TEST";
	WorldItem* items;
	string owner = "";
	int stuffID = 2;
	int stuff_gravity = 50;
	bool stuff_spin = false;
	bool stuff_invert = false;
	bool isPublic = false;
	bool isNuked = false;
	int ownerID = 0;
	int droppeditemcount = 0;
	bool isCasino = false;
	int bgID = 14;
	vector<string> acclist;
	int weather = 0;
	bool ice = false;
	bool land = false;
	bool volcano = false;
	bool online = false;
	int droppedItemUid = 0;
	int droppedCount = 0;

	unsigned long currentItemUID = 1; //has to be 1 by default
	vector<DroppedItem> droppedItems;
};

struct AWorld {
	WorldInfo* ptr;
	WorldInfo info;
	int id;
};

class WorldDB {
public:
	WorldInfo get(string name);
	int getworldStatus(string name);
	AWorld get2(string name);
	void flush(WorldInfo info);
	void flush2(AWorld info);
	void save(AWorld info);
	void saveAll();
	void saveRedundant();
	vector<WorldInfo> getRandomWorlds();
	WorldDB();
private:
	vector<WorldInfo> worlds;
};

WorldDB::WorldDB() {
	// Constructor
}

struct TileExtra {
	int packetType;
	int characterState;
	float objectSpeedX;
	int punchX;
	int punchY;
	int charStat;
	int blockid;
	int visual;
	int signs;
	int backgroundid;
	int displayblock;
	int time;
	int netID;
	int weatherspeed;
	int bpm;
	int unused1;
	int unused2;
	int unused3;
};

enum BlockTypes {
	FOREGROUND,
	BACKGROUND,
	SEED,
	PAIN_BLOCK,
	BEDROCK,
	MAIN_DOOR,
	SIGN,
	DOOR,
	CLOTHING,
	FIST,
	CONSUMABLE,
	CHECKPOINT,
	GATEWAY,
	LOCK,
	WEATHER_MACHINE,
	JAMMER,
	GEM,
	BOARD,
	UNKNOWN
};

struct ItemDefinition {
	int id;

	unsigned char editableType = 0;
	unsigned char itemCategory = 0;
	unsigned char actionType = 0;
	unsigned char hitSoundType = 0;

	string name;

	string texture = "";
	int textureHash = 0;
	unsigned char itemKind = 0;
	int val1;
	unsigned char textureX = 0;
	unsigned char textureY = 0;
	unsigned char spreadType = 0;
	unsigned char isStripeyWallpaper = 0;
	unsigned char collisionType = 0;

	unsigned char breakHits = 0;

	int dropChance = 0;
	unsigned char clothingType = 0;
	BlockTypes blockType;
	int growTime;
	ClothTypes clothType;
	int rarity;
	string effect = "(Mod removed)";
	string effects = "(Mod added)";
	unsigned char maxAmount = 0;
	string extraFile = "";
	int extraFileHash = 0;
	int audioVolume = 0;
	string petName = "";
	string petPrefix = "";
	string petSuffix = "";
	string petAbility = "";
	unsigned	char seedBase = 0;
	unsigned	char seedOverlay = 0;
	unsigned	char treeBase = 0;
	unsigned	char treeLeaves = 0;
	int seedColor = 0;
	int seedOverlayColor = 0;
	bool isMultiFace = false;
	short val2;
	short isRayman = 0;
	string extraOptions = "";
	string texture2 = "";
	string extraOptions2 = "";
	string punchOptions = "";
	string description = "Nothing to see.";
};

vector<ItemDefinition> itemDefs;
