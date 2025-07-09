#include "ssd_cmd_buffer_control.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::invalid_argument;
using std::string;
using std::stringstream;
using std::vector;

void CmdBufferControl::flush() {
  // parsing cmd
  string bufNameList = getBufferNameList();
  vector<Cmd> cmdList = parseCommands(bufNameList);
  string cmdType = "";
  SSDDriver driver;

  for (const auto &cmd : cmdList) {
    if (cmd.cmdType == "W")
      driver.write(cmd.LBA, cmd.value);
    if (cmd.cmdType == "E")
      driver.erase(cmd.LBA, cmd.value);
    if (cmd.cmdType == "empty")
      break;
  }
  clearAllBuffer();
}

vector<Cmd> CmdBufferControl::parseCommands(const string &bufNameList) {
  vector<Cmd> parsedCommands;
  stringstream ssComma(bufNameList);
  string cmdString;

  while (getline(ssComma, cmdString, ',')) {
    if (cmdString.empty())
      continue;

    vector<string> tokens;
    stringstream ssUnderscore(cmdString);
    string paramToken;
    while (getline(ssUnderscore, paramToken, '_')) {
      tokens.push_back(paramToken);
    }

    if (tokens.size() < 2)
      throw invalid_argument("cmd must have at least 2 tokens");

    Cmd cmd;
    cmd.index = stoi(tokens[0]);
    cmd.cmdType = tokens[1];

    if (cmd.cmdType == "empty") {
      if (tokens.size() != 2)
        throw invalid_argument("empty must have 2 tokens");
      parsedCommands.push_back(cmd);
    } else if (cmd.cmdType == "W" || cmd.cmdType == "E") {
      if (tokens.size() < 4)
        throw invalid_argument("W or E must have LBA");
      cmd.LBA = stoi(tokens[2]);
    } else {
      continue;
    }

    if (tokens.size() >= 4) {
      cmd.value = std::stoul(tokens[3], nullptr, HEX_BASE);
    } else {
      cmd.value = 0;
    }

    parsedCommands.push_back(cmd);
  }
  return parsedCommands;
}