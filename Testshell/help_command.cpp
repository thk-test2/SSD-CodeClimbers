#include "command.h"

bool HelpCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  logger.print("HelpCommand.execute()", "Executing help command");
  printHeader();
  printTeamInfo();
  printCommands();
  return true;
}

 void HelpCommand::printHeader() {
   cout << "\033[1;36m"
        << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
        << "\033[0m";
 }

 void HelpCommand::printTeamInfo() {
   cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
   cout << "\n\033[1mTeam Members:\033[0m\n"
        << "  Taehyun Kyong, Sunghwan Kim, Hyeonseok Sim\n"
        << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"
        << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";
 }

 void HelpCommand::printCommands() {
   cout << "\n\033[1mCommands:\033[0m\n";

   // 명령어 순서를 고정하여 출력
   vector<string> commandOrder = _commandHandler->getAvailableCommands();

   for (const string& cmdName : commandOrder) {
     auto it = _commandHandler->getCommand(cmdName);
     if (it) {
       printCommandInfo(cmdName, it->getUsage(), it->getDescription(), it->getExample());
     }
   }
 }

 void HelpCommand::printCommandInfo(const string &command, const string &args,
                                  const string &description,
                                  const string &example) {
   cout << "  \033[1m" << command << "\033[0m";
   if (!args.empty()) {
     cout << " " << args;
   }
   cout << "\n    Description: " << description << "\n"
        << "    Example: " << example << "\n\n";
 }