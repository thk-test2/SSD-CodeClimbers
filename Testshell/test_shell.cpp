#include "gmock/gmock.h"

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct ShellExit : public std::exception {
  const char *what() const noexcept override { return "Shell exited."; }
};

class SSD_INTERFACE {
public:
  virtual void read(int lba) = 0;
  virtual void write(int lba, unsigned long value) = 0;
  virtual string getResult() = 0;
};

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class Command {
public:
  string command;
  vector<string> args;
};

class StdInOutCtrl {
public:
};

class ArgParser {};

class TestShell {
private:
  StdInOutCtrl *ctrl;
  ArgParser *parser;
  SSD_INTERFACE *ssd;
  Command command;

  const int HEX_BASE = 16;

public:
  TestShell() : ctrl(new StdInOutCtrl()), parser(new ArgParser()) {}
  TestShell(SSD_INTERFACE *_ssd)
      : ctrl(new StdInOutCtrl()), parser(new ArgParser()), ssd{_ssd} {}

  void run() {
    string userInput;
    while (true) {
      cout << "> ";
      getline(std::cin, userInput);
      command = parsing(userInput);
      if (command.command == "exit") {
        break;
      }
      executeCommand(command);
    }
  }

  void executeCommand(const Command &command) {
    string cmd = command.command;
    if (cmd == "read") {
      read(command);
    } else if (cmd == "write") {
      write(command);
    } else if (cmd == "exit") {
      exitTestShell(command);
    } else if (cmd == "fullread") {
      fullread(command.args);
    } else if (cmd == "fullwrite") {
      fullwrite(command.args);
    } else if (cmd == "help") {
      help();
    } else if (cmd == "1_" || cmd == "1_FullWriteAndReadCompare") {
      bool isFailed = isScriptOneExecutionSuccessful(command);
      if (isFailed)
        return;
    } else if (cmd == "2_" || cmd == "2_PartialLBAWrite") {
      bool isFailed = isScriptTwoExecutionSuccessful(command);
      if (isFailed)
        return;
    } else if (cmd == "3_" || cmd == "3_WriteReadAging") {
      bool isFailed = isScriptThreeExcutionSuccessful(command);
      if (isFailed)
        return;
    } else {
      cout << "INVALID COMMAND" << endl;
    }
  }

  bool isScriptOneExecutionSuccessful(const Command &command) {
    string valueStr = command.args[0];
    unsigned long value = std::stoul(valueStr, nullptr, HEX_BASE);

    for (int lba = 0; lba < 100; lba += 4) {
      for (int i = 0; i < 4; i++) {
        ssd->write(lba + i, value);
      }

      for (int i = 0; i < 4; i++) {
        ssd->read(lba);
        string result = ssd->getResult();
        if (result != valueStr) {
          cout << "Script 1 execution failed." << endl;
          return true;
        }
      }
    }
    cout << "Script 1 executed successfully." << endl;
    return false;
  }

  bool isScriptTwoExecutionSuccessful(const Command &command) {
    string valueStr = command.args[0];
    vector<int> lbaList{4, 0, 3, 1, 2};

    for (int turn = 0; turn < 30; ++turn) {
      for (auto lba : lbaList) {
        unsigned long value = std::stoul(valueStr, nullptr, 16);
        ssd->write(lba, value);
        if (!checkWriteSuccess(lba, valueStr)) {
          cout << "Script 2 execution failed." << endl;
          return false;
        }
      }
    }
    cout << "Script 2 executed successfully." << endl;
    return true;
  }

  bool isScriptThreeExcutionSuccessful(const Command &command) {
    string valueStr = command.args[0];
    unsigned long value = std::stoul(valueStr, nullptr, 16);

    for (int i = 0; i < 200; i++) {
      ssd->write(0, value);
      if (!checkWriteSuccess(0, valueStr)) {
        cout << "Script 3 execution failed." << endl;
        return false;
      }

      ssd->write(99, value);
      if (!checkWriteSuccess(99, valueStr)) {
        cout << "Script 3 execution failed." << endl;
        return false;
      }
    }
    cout << "Script 3 executed successfully." << endl;
    return true;
  }

  bool checkWriteSuccess(int lba, const string &valueStr) {
    ssd->read(lba);

    if (ssd->getResult() == "ERROR" || ssd->getResult() != valueStr) {
      return false;
    }
    return true;
  }

  // Split the userInput string into command and arguments
  // first word is the command, rest are arguments
  Command parsing(const string &userInput) {
    std::istringstream iss(userInput);
    string cmd;
    vector<string> args;
    if (iss >> cmd) {
      string arg;
      while (iss >> arg) {
        args.push_back(arg);
      }
    }
    return Command{cmd, args};
  }

  void exitTestShell(const Command &command) {
    if (command.args.size() != 0) {
      cout << "INVALID COMMAND\n";
      return;
    }
    cout << "Exiting shell..." << endl;
    throw ShellExit();
  }

  void fullread(vector<string> args) {
    if (args.size() != 0) {
      cout << "INVALID COMMAND\n";
      return;
    }
    int lba = 0;
    ssd->read(lba);
    string result = ssd->getResult();
    while (result != "ERROR") {
      cout << "[Full Read] LBA: " << lba << " Result: " << result << endl;
      lba++;
      ssd->read(lba);
      result = ssd->getResult();
    }
  }

  void fullwrite(vector<string> args) {
    if (args.size() != 1) {
      cout << "INVALID COMMAND\n";
      return;
    }
    unsigned long value;
    try {
      value = stoul(args[0], nullptr, HEX_BASE);
    } catch (std::exception &e) {
      cout << "INVALID COMMAND\n";
      return;
    }
    int lba = 0;
    string result = ssd->getResult();
    ssd->write(lba, value);
    while (result != "ERROR") {
      cout << "[Full Write] LBA: " << lba << " Done" << endl;
      lba++;
      ssd->write(lba, value);
      result = ssd->getResult();
    }
  }

  void help() {
    printHeader();
    printTeamInfo();
    printCommands();
    printTestScripts();
  }

  void printHeader() {
    cout << "\033[1;36m"
         << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
         << "\033[0m";
  }

  void printTeamInfo() {
    cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
    cout << "\n\033[1mTeam Members:\033[0m\n"
         << "  Taehyun Kyong, Sunghwan Kim, Hyeonseok Sim\n"
         << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"

         << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";
  }

  void printCommands() {
    cout << "\n\033[1mCommands:\033[0m\n";
    printCommandInfo("read", "<lba>",
                     "Read from SSD at logical block address <lba>", "read 10");
    printCommandInfo("write", "<lba> <value>",
                     "Write <value> to SSD at logical block address <lba>",
                     "write 5 0xFF");
    printCommandInfo("fullread", "", "Read the entire SSD", "fullread");
    printCommandInfo("fullwrite", "<value>", "Write <value> to the entire SSD",
                     "fullwrite 0x00");
    printCommandInfo("help", "", "Show this help message", "help");
    printCommandInfo("exit", "", "Exit the shell", "exit");
  }

  void printTestScripts() {
    cout << "\033[1mTest Scripts:\033[0m\n";
    printCommandInfo("1_FullWriteAndReadCompare", "",
                     "Run comprehensive write/read test for entire SSD",
                     "'1_' or '1_FullWriteAndReadCompare'");
    printCommandInfo("2_PartialLBAWrite", "",
                     "Run partial LBA write consistency test (30 iterations)",
                     "'2_' or '2_PartialLBAWrite'");
    printCommandInfo("3_WriteReadAging", "",
                     "Run write/read aging test (200 iterations)",
                     "'3_' or '3_WriteReadAging'");
  }

  void printCommandInfo(const string &command, const string &args,
                        const string &description, const string &example) {
    cout << "  \033[1m" << command << "\033[0m";
    if (!args.empty()) {
      cout << " " << args;
    }
    cout << "\n    Description: " << description << "\n"
         << "    Example: " << example << "\n\n";
  }

  void write(const Command &command) {
    if (!isValidWriteUsage(command)) {
      cout << "INVALID COMMAND\n";
      return;
    }
    ssd->write(stoi(command.args[0]),
               stoul(command.args[1], nullptr, HEX_BASE));
    string result = ssd->getResult();
    if (result == "")
      result = "Done";
    cout << "[Write] " << result << "\n";
  }

  bool isValidWriteUsage(const Command &command) {
    if (command.args.size() != 2)
      return false;
    try {
      int lba = stoi(command.args[0]);
      unsigned long value = stoul(command.args[1], nullptr, HEX_BASE);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }

  void read(const Command &command) {
    if (!isValidReadUsage(command)) {
      cout << "INVALID COMMAND\n";
      return;
    }

    int lba = stoi(command.args[0]);
    ssd->read(lba);
    cout << "[Read] " << command.args[0] << " : " << ssd->getResult() << "\n";
  }

  bool isValidReadUsage(const Command &command) {
    if (command.args.size() != 1)
      return false;
    try {
      int lba = stoi(command.args[0]);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }
};
