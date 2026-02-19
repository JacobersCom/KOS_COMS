#pragma once

#include <string>

enum class ServerCommands
{
	help,
	signup,
	login,

	Unknown,
};

inline ServerCommands CommandFromToken(const char* cmd)
{
	if (std::strcmp(cmd, "~help") == 0) return ServerCommands::help;
	if (std::strcmp(cmd, "~register") == 0) return ServerCommands::signup;
	if (std::strcmp(cmd,"~login") == 0) return ServerCommands::login;

	return ServerCommands::Unknown;
}