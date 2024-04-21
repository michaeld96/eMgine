#include "ConfigManager.h"

std::string ConfigManager::game_start_message = "";
std::string ConfigManager::game_over_bad_message = "";
std::string ConfigManager::game_over_good_message = "";
int ConfigManager::x_resolution = 640;
int ConfigManager::y_resolution = 360;
const std::string ConfigManager::make_decision_string = "Please make a decision...";
const std::string ConfigManager::decision_string = "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"";
