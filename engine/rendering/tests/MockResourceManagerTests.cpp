#include "../include/MockResourceManager.hpp"
#include <gtest/gtest.h>

using namespace ECS;

/**
 * MockResourceManager Tests
 * 
 * These tests define the expected behavior of MockResourceManager for testing resource systems.
 * MockResourceManager should simulate texture loading/unloading without requiring actual files.
 * 
 * Key test areas:
 * - Texture loading and handle generation
 * - Handle validation and path retrieval
 * - Texture unloading and resource cleanup
 * - Error handling and edge cases
 * - Call recording for test verification
 */

class MockResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>();
    }
    
    void TearDown() override {
        mockResourceManager.reset();
    }
    
    std::unique_ptr<MockResourceManager> mockResourceManager;
};

// Test basic interface compliance
TEST_F(MockResourceManagerTest, InterfaceCompliance) {
    // Should implement all IResourceManager methods without crashing
    EXPECT_NO_THROW(mockResourceManager->loadTexture("test.png"));
    EXPECT_NO_THROW(mockResourceManager->isTextureValid(1));
    EXPECT_NO_THROW(mockResourceManager->getTexturePath(1));
    EXPECT_NO_THROW(mockResourceManager->unloadTexture(1));
    EXPECT_NO_THROW(mockResourceManager->getLoadedTextureCount());
    EXPECT_NO_THROW(mockResourceManager->clearAllTextures());
}

// Test texture loading functionality
TEST_F(MockResourceManagerTest, LoadTexture) {
    // Should return valid handle for successful load
    TextureHandle handle = mockResourceManager->loadTexture("textures/player.png");
    EXPECT_NE(handle, INVALID_TEXTURE);
    
    // Should record the load call
    EXPECT_TRUE(mockResourceManager->wasMethodCalled("loadTexture"));
    EXPECT_EQ(mockResourceManager->getCallCount("loadTexture"), 1);
    
    // Should record call details
    EXPECT_EQ(mockResourceManager->loadTextureCalls.size(), 1);
    EXPECT_EQ(mockResourceManager->loadTextureCalls[0].filePath, "textures/player.png");
    EXPECT_EQ(mockResourceManager->loadTextureCalls[0].returnedHandle, handle);
}

// Test multiple texture loading
TEST_F(MockResourceManagerTest, LoadMultipleTextures) {
    TextureHandle handle1 = mockResourceManager->loadTexture("texture1.png");
    TextureHandle handle2 = mockResourceManager->loadTexture("texture2.png");
    TextureHandle handle3 = mockResourceManager->loadTexture("texture3.png");
    
    // Should return different handles for different textures
    EXPECT_NE(handle1, handle2);
    EXPECT_NE(handle2, handle3);
    EXPECT_NE(handle1, handle3);
    
    // All handles should be valid
    EXPECT_NE(handle1, INVALID_TEXTURE);
    EXPECT_NE(handle2, INVALID_TEXTURE);
    EXPECT_NE(handle3, INVALID_TEXTURE);
    
    // Should record all calls
    EXPECT_EQ(mockResourceManager->getCallCount("loadTexture"), 3);
    EXPECT_EQ(mockResourceManager->loadTextureCalls.size(), 3);
}

// Test texture handle validation
TEST_F(MockResourceManagerTest, IsTextureValid) {
    TextureHandle handle = mockResourceManager->loadTexture("test.png");
    
    // Loaded texture should be valid
    EXPECT_TRUE(mockResourceManager->isTextureValid(handle));
    
    // Invalid handle should not be valid
    EXPECT_FALSE(mockResourceManager->isTextureValid(INVALID_TEXTURE));
    
    // Non-existent handle should not be valid
    EXPECT_FALSE(mockResourceManager->isTextureValid(999));
}

// Test texture path retrieval
TEST_F(MockResourceManagerTest, GetTexturePath) {
    TextureHandle handle = mockResourceManager->loadTexture("assets/sprites/enemy.png");
    
    // Should return original path for valid handle
    EXPECT_EQ(mockResourceManager->getTexturePath(handle), "assets/sprites/enemy.png");
    
    // Should return empty string for invalid handle
    EXPECT_EQ(mockResourceManager->getTexturePath(INVALID_TEXTURE), "");
    EXPECT_EQ(mockResourceManager->getTexturePath(999), "");
}

// Test texture unloading
TEST_F(MockResourceManagerTest, UnloadTexture) {
    TextureHandle handle = mockResourceManager->loadTexture("test.png");
    
    // Should successfully unload existing texture
    EXPECT_TRUE(mockResourceManager->unloadTexture(handle));
    
    // Should record the unload call
    EXPECT_TRUE(mockResourceManager->wasMethodCalled("unloadTexture"));
    EXPECT_EQ(mockResourceManager->getCallCount("unloadTexture"), 1);
    
    // Should record call details
    EXPECT_EQ(mockResourceManager->unloadTextureCalls.size(), 1);
    EXPECT_EQ(mockResourceManager->unloadTextureCalls[0].handle, handle);
    EXPECT_TRUE(mockResourceManager->unloadTextureCalls[0].returnedSuccess);
    
    // Unloaded texture should no longer be valid
    EXPECT_FALSE(mockResourceManager->isTextureValid(handle));
}

// Test unloading non-existent texture
TEST_F(MockResourceManagerTest, UnloadNonExistentTexture) {
    // Should fail to unload non-existent texture
    EXPECT_FALSE(mockResourceManager->unloadTexture(999));
    EXPECT_FALSE(mockResourceManager->unloadTexture(INVALID_TEXTURE));
}

// Test loaded texture count tracking
TEST_F(MockResourceManagerTest, LoadedTextureCount) {
    // Initially should have no textures
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 0);
    
    // Count should increase with loads
    TextureHandle handle1 = mockResourceManager->loadTexture("texture1.png");
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 1);
    
    TextureHandle handle2 = mockResourceManager->loadTexture("texture2.png");
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 2);
    
    // Count should decrease with unloads
    mockResourceManager->unloadTexture(handle1);
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 1);
    
    mockResourceManager->unloadTexture(handle2);
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 0);
}

// Test clearing all textures
TEST_F(MockResourceManagerTest, ClearAllTextures) {
    // Load several textures
    mockResourceManager->loadTexture("texture1.png");
    mockResourceManager->loadTexture("texture2.png");
    mockResourceManager->loadTexture("texture3.png");
    
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 3);
    
    // Clear all textures
    mockResourceManager->clearAllTextures();
    
    // Should record the clear call
    EXPECT_TRUE(mockResourceManager->wasMethodCalled("clearAllTextures"));
    
    // Should have no loaded textures
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 0);
}

// Test reset functionality
TEST_F(MockResourceManagerTest, ResetClearsState) {
    mockResourceManager->loadTexture("test1.png");
    mockResourceManager->loadTexture("test2.png");
    mockResourceManager->unloadTexture(1);
    
    EXPECT_GT(mockResourceManager->methodCalls.size(), 0);
    EXPECT_GT(mockResourceManager->loadTextureCalls.size(), 0);
    EXPECT_GT(mockResourceManager->unloadTextureCalls.size(), 0);
    EXPECT_GT(mockResourceManager->getLoadedTextureCount(), 0);
    
    mockResourceManager->reset();
    
    EXPECT_EQ(mockResourceManager->methodCalls.size(), 0);
    EXPECT_EQ(mockResourceManager->loadTextureCalls.size(), 0);
    EXPECT_EQ(mockResourceManager->unloadTextureCalls.size(), 0);
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 0);
    EXPECT_FALSE(mockResourceManager->wasMethodCalled("loadTexture"));
}

// Test load failure configuration
TEST_F(MockResourceManagerTest, LoadFailureConfiguration) {
    // Configure load failure mode
    mockResourceManager->setLoadFailureMode(true);
    
    TextureHandle handle = mockResourceManager->loadTexture("nonexistent.png");
    
    // Should return invalid handle in failure mode
    EXPECT_EQ(handle, INVALID_TEXTURE);
    EXPECT_FALSE(mockResourceManager->isTextureValid(handle));
}

// Test custom load result configuration
TEST_F(MockResourceManagerTest, CustomLoadResultConfiguration) {
    // Configure specific return handle
    TextureHandle expectedHandle = 42;
    mockResourceManager->setNextLoadResult(expectedHandle);
    
    TextureHandle handle = mockResourceManager->loadTexture("test.png");
    
    // Should return configured handle
    EXPECT_EQ(handle, expectedHandle);
}

// Test unload result configuration
TEST_F(MockResourceManagerTest, UnloadResultConfiguration) {
    TextureHandle handle = mockResourceManager->loadTexture("test.png");
    
    // Configure unload to fail
    mockResourceManager->setNextUnloadResult(false);
    
    // Should return configured result
    EXPECT_FALSE(mockResourceManager->unloadTexture(handle));
}

// Test edge cases and robustness
TEST_F(MockResourceManagerTest, EdgeCases) {
    // Empty file path should be handled gracefully
    TextureHandle handle = mockResourceManager->loadTexture("");
    EXPECT_NE(handle, INVALID_TEXTURE); // Mock should still return valid handle
    
    // Very long file path should be handled
    std::string longPath(1000, 'a');
    longPath += ".png";
    EXPECT_NO_THROW(mockResourceManager->loadTexture(longPath));
    
    // Special characters in path should be handled
    EXPECT_NO_THROW(mockResourceManager->loadTexture("textures/special chars @#$.png"));
}

// Test typical usage pattern
TEST_F(MockResourceManagerTest, TypicalUsagePattern) {
    // Load several game textures
    TextureHandle playerTexture = mockResourceManager->loadTexture("sprites/player.png");
    TextureHandle enemyTexture = mockResourceManager->loadTexture("sprites/enemy.png");
    TextureHandle backgroundTexture = mockResourceManager->loadTexture("backgrounds/level1.png");
    
    // All should be valid
    EXPECT_TRUE(mockResourceManager->isTextureValid(playerTexture));
    EXPECT_TRUE(mockResourceManager->isTextureValid(enemyTexture));
    EXPECT_TRUE(mockResourceManager->isTextureValid(backgroundTexture));
    
    // Should track correct count
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 3);
    
    // Should be able to retrieve paths
    EXPECT_EQ(mockResourceManager->getTexturePath(playerTexture), "sprites/player.png");
    EXPECT_EQ(mockResourceManager->getTexturePath(enemyTexture), "sprites/enemy.png");
    EXPECT_EQ(mockResourceManager->getTexturePath(backgroundTexture), "backgrounds/level1.png");
    
    // Clean up some textures
    EXPECT_TRUE(mockResourceManager->unloadTexture(enemyTexture));
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 2);
    EXPECT_FALSE(mockResourceManager->isTextureValid(enemyTexture));
    
    // Clear remaining textures
    mockResourceManager->clearAllTextures();
    EXPECT_EQ(mockResourceManager->getLoadedTextureCount(), 0);
}

// Test polymorphic usage through interface
TEST_F(MockResourceManagerTest, PolymorphicUsage) {
    IResourceManager* resourceManager = mockResourceManager.get();
    
    // Should work through base interface
    TextureHandle handle = resourceManager->loadTexture("test.png");
    EXPECT_NE(handle, INVALID_TEXTURE);
    EXPECT_TRUE(resourceManager->isTextureValid(handle));
    EXPECT_EQ(resourceManager->getTexturePath(handle), "test.png");
    EXPECT_TRUE(resourceManager->unloadTexture(handle));
    
    // Verify calls were recorded
    EXPECT_TRUE(mockResourceManager->wasMethodCalled("loadTexture"));
    EXPECT_TRUE(mockResourceManager->wasMethodCalled("unloadTexture"));
}