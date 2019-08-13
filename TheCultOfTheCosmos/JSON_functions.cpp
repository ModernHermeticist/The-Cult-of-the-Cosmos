#include "pch.h"
#include "main.h"

rapidjson::Document open_JSON()
{
 	std::ifstream ifs("dialogue\\file.json");
	if (!ifs.is_open())
	{
		std::cerr << "Could not open file for reading!\n";
		exit(1);
	}


	rapidjson::IStreamWrapper isw(ifs);

	rapidjson::Document d{};
	d.ParseStream(isw);

	return d;
}

rapidjson::Document openActorDialogueJSON()
{
	std::ifstream ifs("dialogue\\dialogue.json");
	if (!ifs.is_open())
	{
		std::cerr << "Could not open file for reading!\n";
		exit(1);
	}


	rapidjson::IStreamWrapper isw(ifs);

	rapidjson::Document d{};
	d.ParseStream(isw);

	return d;
}

void print_JSON_body(rapidjson::Document &d)
{
	assert(d.HasMember("body"));
	assert(d["body"].IsString());
	printf("body = %s\n", d["body"].GetString());
	const rapidjson::Value& body = d["body"];
}

void loadMapFile()
{
	std::ifstream ifs("maps\\test.json");
	if (!ifs.is_open())
	{
		std::cerr << "Could not open file for reading!\n";
		exit(1);
	}

	rapidjson::IStreamWrapper isw(ifs);

	rapidjson::Document d{};
	d.ParseStream(isw);

	int height = d["Map"]["height"].GetInt();
	int width = d["Map"]["width"].GetInt();
	rapidjson::Value& a = d["Map"]["layers"]["data"];
	engine.map->height = height;
	engine.map->width = width;
	engine.map->initFromFile(false, width, height, a);
}

void loadActorDialogue(rapidjson::Document &d, std::string curNode, Actor *owner)
{
	owner->dialogueSet.clear();
	rapidjson::Value& s = d[owner->talkableType.c_str()][owner->talkableName.c_str()][curNode.c_str()]["body"];
	std::string tempDialogueString = s.GetString();
	//std::string tempDialogueString = d["body"].GetString();

	while (true)
	{
		std::size_t pos = tempDialogueString.find_first_of("\n") + 1;
		std::string subDialogueString = tempDialogueString.substr(0, pos);
		owner->dialogueSet.push_back(subDialogueString);
		tempDialogueString.erase(0, pos);
		if (tempDialogueString.size() == 0)
		{
			break;
		}
	}
}

void loadActorDialogueResponses(rapidjson::Document &d, std::string curNode, Actor *owner)
{
	owner->talkOptions.clear();
	rapidjson::Value& s = d[owner->talkableType.c_str()][owner->talkableName.c_str()][curNode.c_str()]["responses"];
	for (rapidjson::Value::ConstMemberIterator itr = s.MemberBegin(); itr != s.MemberEnd(); ++itr)
	{
		std::string tempTalkOptionsString = itr->value.GetString();
		while (true)
		{
			std::size_t pos = tempTalkOptionsString.find_first_of("\n") + 1;
			std::string subTalkOptionsString = tempTalkOptionsString.substr(0, pos);
			owner->talkOptions.push_back(subTalkOptionsString);
			tempTalkOptionsString.erase(0, pos);
			if (tempTalkOptionsString.size() == 0)
			{
				break;
			}
		}
	}
}

std::string interpretDialogueResponse(rapidjson::Document &d, Actor *talker, std::string curNode, TCOD_key_t key)
{
	int numOfOptions = d[talker->talkableType.c_str()][talker->talkableName.c_str()][curNode.c_str()]["goTo"].Size();
	int keyValue = 25;

	for (int option = 0; option <= numOfOptions; option++)
	{
		if (numOfOptions > 0 && key.vk == keyValue)
		{
			rapidjson::Value& s = d[talker->talkableType.c_str()][talker->talkableName.c_str()][curNode.c_str()]["goTo"][option];
			return s.GetString();
		}
		keyValue++;
	}

	return curNode;
}