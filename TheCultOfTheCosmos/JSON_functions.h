#pragma once
#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTION_H

rapidjson::Document open_JSON();
rapidjson::Document openActorDialogueJSON();

void print_JSON_body(rapidjson::Document &d);

void loadMapFile();

void loadActorDialogue(rapidjson::Document &d, std::string curNode, Actor *owner);
void loadActorDialogueResponses(rapidjson::Document &d, std::string curNode, Actor *owner);

std::string interpretDialogueResponse(rapidjson::Document &d, Actor *talker, std::string curNode,TCOD_key_t key);


#endif