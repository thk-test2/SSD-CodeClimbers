#include "file_system_interface.h"
#include "logger.h"
#include <gmock/gmock.h>
#include <memory>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

class MockFileSystem : public IFileSystem {
public:
  MOCK_METHOD(bool, fileExists, (const std::string &filename), (override));
  MOCK_METHOD(size_t, getFileSize, (const std::string &filename), (override));
  MOCK_METHOD(bool, renameFile,
              (const std::string &oldName, const std::string &newName),
              (override));
  MOCK_METHOD(std::vector<std::string>, getUntilLogFiles, (), (override));
  MOCK_METHOD(void, writeToFile,
              (const std::string &filename, const std::string &content),
              (override));
};

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    Logger::resetInstance();
    mockFileSystem = std::make_unique<StrictMock<MockFileSystem>>();
    mockFileSystemPtr = mockFileSystem.get();
  }

  void TearDown() override { Logger::resetInstance(); }

  std::unique_ptr<MockFileSystem> mockFileSystem;
  MockFileSystem *mockFileSystemPtr;
};

TEST_F(LoggerTest, BasicLogging_WritesToFile) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _)).Times(1);
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillOnce(Return(false));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger = Logger::getInstance();
  logger.print("TestClass", "Test message");

  // Assert
}

TEST_F(LoggerTest, FileRotation_WhenFileSizeExceedsLimit) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _));
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*mockFileSystemPtr, getFileSize("latest.log"))
      .WillOnce(Return(11 * 1024)); // Exceeds 10KB
  EXPECT_CALL(*mockFileSystemPtr, renameFile("latest.log", _))
      .WillOnce(Return(true));

  // Old file management
  EXPECT_CALL(*mockFileSystemPtr, getUntilLogFiles())
      .WillOnce(
          Return(std::vector<std::string>{"until_250701_10h_30m_00s.log"}));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger = Logger::getInstance();
  logger.print("RotationTest", "This should trigger rotation");
}

TEST_F(LoggerTest, FileRotation_WhenFileSizeWithinLimit) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _));
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillOnce(Return(true));
  EXPECT_CALL(*mockFileSystemPtr, getFileSize("latest.log"))
      .WillOnce(Return(5 * 1024)); // Under 10KB

  // Rotation should not occur
  EXPECT_CALL(*mockFileSystemPtr, renameFile(_, _)).Times(0);

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger = Logger::getInstance();
  logger.print("NoRotationTest", "This should not trigger rotation");
}

TEST_F(LoggerTest, OldFileManagement_CompressesOldestFile) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _));
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*mockFileSystemPtr, getFileSize("latest.log"))
      .WillOnce(Return(11 * 1024));
  EXPECT_CALL(*mockFileSystemPtr, renameFile("latest.log", _))
      .WillOnce(Return(true));

  // Multiple until_ files exist
  std::vector<std::string> untilFiles = {"until_250701_10h_30m_00s.log",
                                         "until_250701_11h_30m_00s.log"};
  EXPECT_CALL(*mockFileSystemPtr, getUntilLogFiles())
      .WillOnce(Return(untilFiles));

  // Change oldest file to .zip
  EXPECT_CALL(*mockFileSystemPtr, renameFile("until_250701_10h_30m_00s.log",
                                             "until_250701_10h_30m_00s.zip"))
      .WillOnce(Return(true));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger = Logger::getInstance();
  logger.print("OldFileTest", "Testing old file management");
}

TEST_F(LoggerTest, LogEntryFormat_CorrectTimestampAndMessage) {
  // Arrange
  std::string capturedLogEntry;
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _))
      .WillOnce([&capturedLogEntry](const std::string &filename,
                                    const std::string &content) {
        capturedLogEntry = content;
      });
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillOnce(Return(false));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger = Logger::getInstance();
  logger.print("TestClass", "Test message");

  // Assert
  EXPECT_THAT(capturedLogEntry, ::testing::HasSubstr("TestClass"));
  EXPECT_THAT(capturedLogEntry, ::testing::HasSubstr("Test message"));
  EXPECT_THAT(
      capturedLogEntry,
      ::testing::MatchesRegex(
          R"(\[\d\d\.\d\d\.\d\d \d\d:\d\d\] TestClass : Test message\.)"));
}

TEST_F(LoggerTest, FileOperationFailure_HandlesGracefully) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _));
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*mockFileSystemPtr, getFileSize("latest.log"))
      .WillOnce(Return(11 * 1024));
  EXPECT_CALL(*mockFileSystemPtr, renameFile("latest.log", _))
      .WillOnce(Return(false)); // Simulate failure

  // renameFile 실패 후에도 manageOldLogFiles()가 호출되므로 추가
  EXPECT_CALL(*mockFileSystemPtr, getUntilLogFiles())
      .WillOnce(Return(std::vector<std::string>{}));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act & Assert - Should not throw exception
  EXPECT_NO_THROW({
    Logger &logger = Logger::getInstance();
    logger.print("FailureTest", "Testing failure handling");
  });
}

TEST_F(LoggerTest, SingletonPattern_ReturnsSameInstance) {
  // Arrange
  Logger::setFileSystem(std::move(mockFileSystem));

  // Act
  Logger &logger1 = Logger::getInstance();
  Logger &logger2 = Logger::getInstance();

  // Assert
  EXPECT_EQ(&logger1, &logger2);
}

TEST_F(LoggerTest, EmptyMessage_HandledCorrectly) {
  // Arrange
  EXPECT_CALL(*mockFileSystemPtr, writeToFile("latest.log", _));
  EXPECT_CALL(*mockFileSystemPtr, fileExists("latest.log"))
      .WillOnce(Return(false));

  Logger::setFileSystem(std::move(mockFileSystem));

  // Act & Assert
  EXPECT_NO_THROW({
    Logger &logger = Logger::getInstance();
    logger.print("EmptyTest", "");
  });
}
