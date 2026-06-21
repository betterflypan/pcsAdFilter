# PC's AdFilter

A local HTTP proxy-based ad filtering system built with C++ (Visual Studio 2010).

## Features

- HTTP proxy server running on port 8080
- Ad blocking based on domain rules and URL keyword rules
- Configurable via `config.txt`
- Real-time console logging
- Test server for verifying blocking behavior

## Build & Run

1. Open `pc'sAdFilter.sln` in Visual Studio 2010
2. Build the solution (Debug/Release)
3. Run the executable OR use the menu system in debug mode

## Configuration

Edit `config.txt` in the same directory as the executable:

```
# Domain-based blocking
DOMAIN doubleclick.net
DOMAIN googleadservices.com

# URL keyword blocking
KEYWORD /ads/
KEYWORD /banner/
KEYWORD /tracker
```

## Usage

1. **Start the proxy**: Select menu option 1, the proxy listens on port 8080
2. **Configure browser proxy**:
   - Set HTTP proxy to `127.0.0.1:8080`
   - Configure in browser network/proxy settings
3. **Verify blocking**:
   - Visit `http://doubleclick.net` - should return 403 Ad Blocked
   - Visit `http://example.com` - should load normally
4. **Test with built-in server**: Select menu option 8, then visit `http://localhost:8081/`

## Verify Blocking

- Images containing ads are replaced with a "Normal Image" placeholder
- Blocked images show a `[X]` marker
- Check F12 Network tab for 403 status codes
- View logs in console: `[PROXY_DBG] Blocked request: ...`

## Log File

Runtime logs are written to `adfilter.log`:
```
[2026-05-22 21:24:15] [BLOCKED] Rule=DOMAIN | URL=http://doubleclick.net/
```

## Extending

To add new rule types:
1. Implement a new class inheriting from `IFilterRule`
2. Register the new rule type in `RuleFactory`
3. Add the rule syntax to `config.txt`
4. Use menu option 5 to reload rules

## Project Structure

```
pc'sAdFilter/
├── BlockLogFile.cpp/h     # Log file management
├── ConfigManager.cpp/h    # Configuration file parser
├── ConsoleUI.cpp/h        # Console menu system
├── DomainRule.cpp/h       # Domain-based blocking rules
├── FilterEngine.cpp/h      # Core filtering engine
├── IFilterRule.h          # Rule interface
├── KeywordRule.cpp/h      # URL keyword blocking rules
├── Logger.cpp/h           # Logging utilities
├── MenuSystem.cpp/h       # Interactive menu
├── ProxyServer.cpp/h      # HTTP proxy implementation
├── RuleFactory.cpp/h      # Rule object factory
├── RuleStatistics.cpp/h   # Blocking statistics
├── TestServer.cpp/h       # Built-in test HTTP server
├── WhiteListRule.cpp/h    # Whitelist rules
└── mainControl.cpp        # Program entry point
```

## Requirements

- Visual Studio 2010
- Windows (Win32 API for proxy functionality)
- C++ (no external dependencies)

## License

MIT License
