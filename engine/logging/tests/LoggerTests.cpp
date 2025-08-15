#include "../include/Logger.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace Engine;

// Mock output for testing
class MockOutput : public ILogOutput {
private:
    mutable std::vector<std::string> messages;
    mutable std::vector<LogLevel> levels;
    mutable std::vector<std::string> categories;
    mutable int flushCount = 0;

public:
    void write(LogLevel level, const std::string& category, const std::string& message) override {
        levels.push_back(level);
        categories.push_back(category);
        messages.push_back(message);
    }
    
    void flush() override {
        flushCount++;
    }
    
    // Test accessors
    const std::vector<std::string>& getMessages() const { return messages; }
    const std::vector<LogLevel>& getLevels() const { return levels; }
    const std::vector<std::string>& getCategories() const { return categories; }
    int getFlushCount() const { return flushCount; }
    size_t getMessageCount() const { return messages.size(); }
    
    void clear() {
        messages.clear();
        levels.clear();
        categories.clear();
        flushCount = 0;
    }
};

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockOutput = new MockOutput(); // Logger will take ownership
        logger = std::make_unique<Logger>(std::unique_ptr<ILogOutput>(mockOutput), LogLevel::DEBUG);
    }

    void TearDown() override {
        logger.reset();
        // mockOutput is deleted by logger
    }
    
    MockOutput* mockOutput; // Raw pointer for testing (owned by logger)
    std::unique_ptr<Logger> logger;
};

// Test logger construction and basic configuration
TEST_F(LoggerTest, ConstructorSetsMinLevel) {
    EXPECT_EQ(logger->getMinLevel(), LogLevel::DEBUG);
    EXPECT_TRUE(logger->isEnabled());
}

// Test shouldLog filtering
TEST_F(LoggerTest, ShouldLogRespectsMixLevel) {
    logger->setMinLevel(LogLevel::WARN);
    
    EXPECT_FALSE(logger->shouldLog(LogLevel::DEBUG));
    EXPECT_FALSE(logger->shouldLog(LogLevel::INFO));
    EXPECT_TRUE(logger->shouldLog(LogLevel::WARN));
    EXPECT_TRUE(logger->shouldLog(LogLevel::ERROR));
}

// Test log level filtering
TEST_F(LoggerTest, LogLevelFiltering) {
    logger->setMinLevel(LogLevel::WARN);
    
    logger->debug("Test", "Debug message");
    logger->info("Test", "Info message");
    logger->warn("Test", "Warn message");
    logger->error("Test", "Error message");
    
    // Should only have WARN and ERROR messages
    EXPECT_EQ(mockOutput->getMessageCount(), 2);
    EXPECT_EQ(mockOutput->getLevels()[0], LogLevel::WARN);
    EXPECT_EQ(mockOutput->getLevels()[1], LogLevel::ERROR);
}

// Test message content and categories
TEST_F(LoggerTest, MessageContentAndCategories) {
    logger->info("EntityManager", "Entity created");
    logger->error("Physics", "Collision detected");
    
    ASSERT_EQ(mockOutput->getMessageCount(), 2);
    
    EXPECT_EQ(mockOutput->getCategories()[0], "EntityManager");
    EXPECT_EQ(mockOutput->getMessages()[0], "Entity created");
    
    EXPECT_EQ(mockOutput->getCategories()[1], "Physics");
    EXPECT_EQ(mockOutput->getMessages()[1], "Collision detected");
}

// Test enable/disable functionality
TEST_F(LoggerTest, EnableDisableFunctionality) {
    logger->setEnabled(false);
    EXPECT_FALSE(logger->isEnabled());
    
    logger->info("Test", "Should not appear");
    EXPECT_EQ(mockOutput->getMessageCount(), 0);
    
    logger->setEnabled(true);
    EXPECT_TRUE(logger->isEnabled());
    
    logger->info("Test", "Should appear");
    EXPECT_EQ(mockOutput->getMessageCount(), 1);
}

// Test convenience methods
TEST_F(LoggerTest, ConvenienceMethods) {
    logger->debug("Debug", "Debug message");
    logger->info("Info", "Info message");
    logger->warn("Warn", "Warn message");
    logger->error("Error", "Error message");
    
    ASSERT_EQ(mockOutput->getMessageCount(), 4);
    
    EXPECT_EQ(mockOutput->getLevels()[0], LogLevel::DEBUG);
    EXPECT_EQ(mockOutput->getLevels()[1], LogLevel::INFO);
    EXPECT_EQ(mockOutput->getLevels()[2], LogLevel::WARN);
    EXPECT_EQ(mockOutput->getLevels()[3], LogLevel::ERROR);
}

// Test flush functionality with buffered output
TEST_F(LoggerTest, FlushFunctionality) {
    // Test basic flush counting
    logger->flush();
    EXPECT_EQ(mockOutput->getFlushCount(), 1);
    
    logger->flush();
    logger->flush();
    EXPECT_EQ(mockOutput->getFlushCount(), 3);
}

// Test that flush actually forces buffered output to be written
TEST(BufferedOutputTest, FlushForcesBufferedOutput) {
    // Create a buffered output that simulates real buffering behavior
    class BufferedMockOutput : public ILogOutput {
    private:
        mutable std::vector<std::string> buffer;
        mutable std::vector<std::string> writtenMessages;
        mutable int flushCount = 0;
        
    public:
        void write(LogLevel level, const std::string& category, const std::string& message) override {
            (void)level;
            (void)category;
            // Add to buffer but don't write to "output" until flush
            buffer.push_back(message);
        }
        
        void flush() override {
            flushCount++;
            // Move all buffered messages to "written" (simulating actual output)
            writtenMessages.insert(writtenMessages.end(), buffer.begin(), buffer.end());
            buffer.clear();
        }
        
        // Test accessors
        size_t getBufferedCount() const { return buffer.size(); }
        size_t getWrittenCount() const { return writtenMessages.size(); }
        const std::vector<std::string>& getWrittenMessages() const { return writtenMessages; }
        int getFlushCount() const { return flushCount; }
    };
    
    auto bufferedOutput = std::make_unique<BufferedMockOutput>();
    BufferedMockOutput* bufferedPtr = bufferedOutput.get();
    
    Logger logger(std::move(bufferedOutput), LogLevel::INFO);
    
    // Write some messages - they should be buffered, not written
    logger.info("Test1", "Message 1");
    logger.info("Test2", "Message 2");
    logger.info("Test3", "Message 3");
    
    // Messages should be buffered but not yet written
    EXPECT_EQ(bufferedPtr->getBufferedCount(), 3);
    EXPECT_EQ(bufferedPtr->getWrittenCount(), 0);
    EXPECT_EQ(bufferedPtr->getFlushCount(), 0);
    
    // Flush should move buffered messages to written output
    logger.flush();
    
    EXPECT_EQ(bufferedPtr->getBufferedCount(), 0);  // Buffer should be empty
    EXPECT_EQ(bufferedPtr->getWrittenCount(), 3);   // All messages should be written
    EXPECT_EQ(bufferedPtr->getFlushCount(), 1);     // Flush should have been called
    
    // Verify the actual messages were flushed correctly
    const auto& written = bufferedPtr->getWrittenMessages();
    EXPECT_EQ(written[0], "Message 1");
    EXPECT_EQ(written[1], "Message 2");
    EXPECT_EQ(written[2], "Message 3");
    
    // Additional messages after flush should work normally
    logger.info("Test4", "Message 4");
    EXPECT_EQ(bufferedPtr->getBufferedCount(), 1);  // New message buffered
    EXPECT_EQ(bufferedPtr->getWrittenCount(), 3);   // Previous messages still written
    
    logger.flush();
    EXPECT_EQ(bufferedPtr->getBufferedCount(), 0);  // Buffer empty again
    EXPECT_EQ(bufferedPtr->getWrittenCount(), 4);   // All messages now written
    EXPECT_EQ(bufferedPtr->getFlushCount(), 2);     // Two flushes total
}

// Test min level setting
TEST_F(LoggerTest, MinLevelSetting) {
    logger->setMinLevel(LogLevel::ERROR);
    EXPECT_EQ(logger->getMinLevel(), LogLevel::ERROR);
    
    logger->info("Test", "Should not appear");
    logger->error("Test", "Should appear");
    
    EXPECT_EQ(mockOutput->getMessageCount(), 1);
    EXPECT_EQ(mockOutput->getLevels()[0], LogLevel::ERROR);
}

// Test console output (basic functionality)
TEST(ConsoleOutputTest, BasicFunctionality) {
    ConsoleOutput console;
    
    // This test just ensures no crashes - output goes to stdout
    console.write(LogLevel::INFO, "Test", "Test message");
    console.flush();
    
    // If we get here without crashing, the test passes
    SUCCEED();
}

// Test file output
TEST(FileOutputTest, FileCreationAndWriting) {
    const std::string testFile = "test_log.txt";
    
    // Clean up any existing test file
    std::filesystem::remove(testFile);
    
    {
        FileOutput fileOutput(testFile, false); // Don't append, create new
        EXPECT_TRUE(fileOutput.isOpen());
        
        fileOutput.write(LogLevel::INFO, "Test", "Test message");
        fileOutput.flush();
    } // FileOutput destructor should close file
    
    // Verify file was created and contains expected content
    EXPECT_TRUE(std::filesystem::exists(testFile));
    
    std::ifstream file(testFile);
    EXPECT_TRUE(file.is_open());
    
    std::string line;
    bool foundMessage = false;
    while (std::getline(file, line)) {
        if (line.find("Test message") != std::string::npos) {
            foundMessage = true;
            break;
        }
    }
    EXPECT_TRUE(foundMessage);
    
    // Clean up
    std::filesystem::remove(testFile);
}

// Test file output append mode
TEST(FileOutputTest, AppendMode) {
    const std::string testFile = "test_append_log.txt";
    
    // Clean up any existing test file
    std::filesystem::remove(testFile);
    
    // Write first message
    {
        FileOutput fileOutput(testFile, false); // Create new file
        fileOutput.write(LogLevel::INFO, "Test", "First message");
    }
    
    // Append second message
    {
        FileOutput fileOutput(testFile, true); // Append mode
        EXPECT_TRUE(fileOutput.isOpen());
        fileOutput.write(LogLevel::INFO, "Test", "Second message");
    }
    
    // Verify both messages are in file
    std::ifstream file(testFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("First message") != std::string::npos);
    EXPECT_TRUE(content.find("Second message") != std::string::npos);
    
    // Clean up
    std::filesystem::remove(testFile);
}

// Test global logger functionality
TEST(GlobalLoggerTest, SetAndGetGlobalLogger) {
    // Initially no global logger
    EXPECT_FALSE(GlobalLogger::hasLogger());
    EXPECT_EQ(GlobalLogger::getLogger(), nullptr);
    
    // Set global logger
    auto mockOutput = std::make_unique<MockOutput>();
    MockOutput* mockPtr = mockOutput.get();
    auto globalLogger = std::make_unique<Logger>(std::move(mockOutput), LogLevel::INFO);
    
    GlobalLogger::setLogger(std::move(globalLogger));
    
    EXPECT_TRUE(GlobalLogger::hasLogger());
    EXPECT_NE(GlobalLogger::getLogger(), nullptr);
    
    // Test using global logger
    GlobalLogger::getLogger()->info("Global", "Global test message");
    
    EXPECT_EQ(mockPtr->getMessageCount(), 1);
    EXPECT_EQ(mockPtr->getMessages()[0], "Global test message");
    
    // Clean up - set to nullptr
    GlobalLogger::setLogger(nullptr);
    EXPECT_FALSE(GlobalLogger::hasLogger());
}

// Test logging macros
TEST(LoggingMacrosTest, MacroUsage) {
    // Set up global logger with mock output
    auto mockOutput = std::make_unique<MockOutput>();
    MockOutput* mockPtr = mockOutput.get();
    auto globalLogger = std::make_unique<Logger>(std::move(mockOutput), LogLevel::DEBUG);
    
    GlobalLogger::setLogger(std::move(globalLogger));
    
    // Test macros
    LOG_DEBUG("Debug", "Debug macro message");
    LOG_INFO("Info", "Info macro message");
    LOG_WARN("Warn", "Warn macro message");
    LOG_ERROR("Error", "Error macro message");
    
    EXPECT_EQ(mockPtr->getMessageCount(), 4);
    EXPECT_EQ(mockPtr->getMessages()[0], "Debug macro message");
    EXPECT_EQ(mockPtr->getMessages()[1], "Info macro message");
    EXPECT_EQ(mockPtr->getMessages()[2], "Warn macro message");
    EXPECT_EQ(mockPtr->getMessages()[3], "Error macro message");
    
    // Clean up
    GlobalLogger::setLogger(nullptr);
}

// Test macros with no global logger (should not crash)
TEST(LoggingMacrosTest, MacrosWithoutGlobalLogger) {
    // Ensure no global logger is set
    GlobalLogger::setLogger(nullptr);
    EXPECT_FALSE(GlobalLogger::hasLogger());
    
    // These should not crash
    LOG_DEBUG("Test", "Should not crash");
    LOG_INFO("Test", "Should not crash");
    LOG_WARN("Test", "Should not crash");
    LOG_ERROR("Test", "Should not crash");
    
    // If we get here, test passes
    SUCCEED();
}