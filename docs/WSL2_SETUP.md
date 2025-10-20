# Running Train Heist Engine in WSL2

This guide explains how to run the Train Heist engine demo in WSL2, including both the headless tests and the graphical SFML demo.

## Prerequisites

### 1. Install Dependencies in WSL2

```bash
# Update package list
sudo apt update

# Install development tools
sudo apt install -y build-essential g++ cmake

# Install X11 and graphics libraries
sudo apt install -y libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev

# Install X11 forwarding support
sudo apt install -y x11-apps

# Optional: Install a simple X11 window manager for testing
sudo apt install -y xfce4
```

### 2. Setup X11 Forwarding

You have two main options for displaying graphics from WSL2:

#### Option A: Using Windows X11 Server (Recommended)

1. **Install VcXsrv or Xming on Windows:**
   - Download VcXsrv from: https://sourceforge.net/projects/vcxsrv/
   - Or download Xming from: https://sourceforge.net/projects/xming/

2. **Configure VcXsrv:**
   - Launch VcXsrv
   - Choose "Multiple windows"
   - Set Display number to `0`
   - Choose "Start no client"
   - **IMPORTANT**: Check "Disable access control"
   - Finish the setup

3. **Set DISPLAY variable in WSL2:**
   ```bash
   # Get Windows IP address
   export DISPLAY=$(grep nameserver /etc/resolv.conf | awk '{print $2}'):0.0
   
   # Add to your ~/.bashrc for persistence
   echo 'export DISPLAY=$(grep nameserver /etc/resolv.conf | awk "{print \$2}"):0.0' >> ~/.bashrc
   ```

4. **Test X11 forwarding:**
   ```bash
   # Test with a simple X11 app
   xclock
   # If you see a clock window, X11 forwarding is working!
   ```

#### Option B: Using WSL2 with Windows 11 WSLg

If you're on Windows 11, WSLg provides built-in GUI support:

```bash
# WSLg should work out of the box, just ensure you have Windows 11
# Test with:
xclock
```

## Running the Engine

### 1. Run Tests (Headless - Always Works)

```bash
# Navigate to project directory
cd /home/galbachten/projects/cpp/train_heist

# Run all ECS tests (no graphics required)
make test
```

**Expected output:** All 201 tests should pass, including ECS, rendering mocks, and component tests.

### 2. Run Main Demo (Requires X11)

```bash
# Build the main executable
make clean && make

# Run the graphical demo
make run
# or directly:
./build/linux/game
```

**Expected behavior:**
- Opens an 800x600 SFML window titled "Train Heist - Basic Demo"
- Displays 5 colored rectangles:
  - Red rectangle (top-left)
  - Green rectangle (top-right) 
  - Blue rectangle (bottom-left)
  - Yellow rectangle (center, animated in circular motion)
  - Purple rectangle (semi-transparent, overlapping center)
- Press any key to see keyboard input logged
- Close window to exit

### 3. Available Make Commands

```bash
# Build everything
make all

# Run tests only (headless)
make test

# Build and run main demo (requires X11)
make run

# Clean build artifacts
make clean

# Build main executable only
make
```

## Troubleshooting

### X11 Issues

**Problem**: `X Error of failed request: BadWindow` or `cannot connect to X server`

**Solutions**:
1. Ensure VcXsrv/Xming is running on Windows
2. Check DISPLAY variable: `echo $DISPLAY`
3. Test X11 with: `xclock` or `xeyes`
4. Restart VcXsrv with "Disable access control" checked

**Problem**: `Setting vertical sync not supported`

**Solution**: This is a warning, not an error. The demo should still work.

### Build Issues

**Problem**: `undefined reference to sf::...`

**Solution**: SFML dependencies are missing. Re-run the dependency installation:
```bash
sudo apt install -y libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev
```

**Problem**: `No rule to make target`

**Solution**: Clean and rebuild:
```bash
make clean && make
```

### Performance Issues

**Problem**: Slow rendering or lag

**Solutions**:
1. Close other applications to free resources
2. Try WSLg (Windows 11) instead of VcXsrv
3. Reduce window resolution in `src/main.cpp` (line 39)

## Architecture Overview

The demo showcases:

- **ECS System**: Entity-Component-System architecture with 5 entities
- **SFML Integration**: Real windowing, rendering, and event handling
- **Component Management**: Position and Renderable components stored in cache-friendly arrays
- **Manual Rendering Loop**: Direct entity iteration and rendering (simplified for demo)
- **Event Handling**: Window close and keyboard input
- **Animation**: Circular motion of yellow rectangle

## Next Steps

1. **Verify tests pass**: `make test` (201 tests should pass)
2. **Setup X11 forwarding**: Follow Option A or B above
3. **Run the demo**: `make run`
4. **Explore the code**: Start with `src/main.cpp` to see the integration

The engine is now ready for game development with a working rendering pipeline!