#include "command.h"

void ReadCommand::execute(int lba, int size, unsigned long value, char *argv[],
                          CmdBufferControl &control) {
  unsigned long bufferRead = 0x0;

  if (control.isBufferContainReadValue(lba, bufferRead))
    control.getDriver()->readBuffer(lba, bufferRead);
  else
    control.getDriver()->read(lba);
}

void WriteCommand::execute(int lba, int size, unsigned long value, char *argv[],
                           CmdBufferControl &control) {
  unsigned long bufferRead = 0x0;

  control.removeAndUpdateWriteCommand(lba, argv);
}

void EraseCommand::execute(int lba, int size, unsigned long value, char *argv[],
                           CmdBufferControl &control) {
  control.setEraseMap(lba, size, 1);
  control.mergeAndUpdateEraseCommand(lba, size);
}

void FlushCommand::execute(int lba, int size, unsigned long value, char *argv[],
                           CmdBufferControl &control) {
  control.flush();
}
