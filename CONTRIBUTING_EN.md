# 🤝 Contribution Guidelines

* [中文](CONTRIBUTING.md)

WM_IoT_SDK is an open source project. We welcome and appreciate contributions from community members. To ensure healthy project development and code quality, please follow these contribution guidelines.

## 📜 License

This project is licensed under the Apache 2.0 open source license. You are free to use, modify, distribute and sell products containing code from this project. Some third-party components may use other licenses, which are clearly identified in the code.

## 💻 Development Environment

Development is supported on Windows, and Linux systems. For detailed environment setup guide, please refer to the [WM_IoT_SDK Programming Guide](http://doc.winnermicro.net).

## 📦 Code Repository

- 🏠 Main Repository: https://github.com/winnermicro/wm_iot_sdk
- 🐛 Issue Reporting: Submit issues and suggestions via GitHub Issues
- 🔀 Code Submission: Submit changes via GitHub Pull Request

## 🔄 Contribution Process

1. 🍴 Fork the project to your personal account
2. 🌱 Create a feature branch
3. 💪 Submit changes, ensuring:
   - ✅ Compliance with code standards
   - ✅ Clear commit messages
   - ✅ Includes tests and documentation
4. 📤 Submit Pull Request
5. 🤖 Wait for GitHub Action CICD results. If the build succeeds, a ✅ green checkmark will appear on the PR page. If the build fails, a ❌ red cross will appear - in this case, click to view the detailed build logs, fix the errors based on the error messages, and resubmit the code.
6. ⏳ Wait for review and merge

## 📝 Commit Guidelines

- 📝 Commit Message Format:
  We use a standardized commit format. Each commit should follow these guidelines:

  ```
  <type>(<scope>): <subject>

  <body>
  ```

  Each commit message must include the following required parts:

  1. 📋 First Line Format `<type>(<scope>): <subject>`
     - type: Commit type, must be one of the following:
       - 🌟 `feat`: New features (e.g. new APIs, new feature modules)
       - 🐛 `fix`: Bug fixes (e.g. system crashes, memory leaks)
       - 📚 `docs`: Documentation updates (e.g. API docs, README)
       - 💅 `style`: Code formatting changes (spaces, indentation, line breaks, comments)
       - ♻️ `refactor`: Code refactoring (optimizing code structure without changing functionality)
       - ⚡️ `perf`: Performance optimization (improving code execution efficiency)
       - ✅ `test`: Test related (adding/modifying test cases)
       - 🔧 `chore`: Build process or auxiliary tool changes
       - 🗑️ `del`: File/code deletion
       - ⏪ `revert`: Reverting previous commits
     - 🔍 scope: Modification scope, for example:
       - fota: Firmware upgrade module
       - mqtt: MQTT protocol module
       - wifi: WiFi module
       - ble: Bluetooth module
       - driver: Driver module
       - utils: Utility module
     - 📌 subject: Brief description of changes (no more than 50 characters)
       - Start with a verb, use present tense
       - First letter lowercase
       - No period at the end

  2. 📋 Detailed Description `<body>`
     - Must describe in detail:
       - ❓ Why the change:
         - What problem was fixed
         - What requirement was added
         - What performance was optimized
       - ✨ What was changed:
         - List main modification points
         - Describe specific changes
         - Explain affected functionality
       - 💡 Implementation approach:
         - What technical solution was used
         - Why this solution was chosen
         - Pros and cons analysis of the solution
     - 📢 Writing requirements:
       - Use clear language, avoid obscure wording
       - Use consistent language
       - Organize in clear sections
       - Highlight important information in bold

## 🎨 Code Style

To maintain code consistency and readability, we have established detailed code style guidelines. Please refer to the [Code Style](https://doc.winnermicro.net/w800/en/latest/contribute/code_style.html) chapter in the WM_IoT_SDK online documentation for complete specifications.

### 🛠️ Automatic Formatting Tool

This project uses clang-format tool to automatically format code and ensure consistent code style:

- 📄 Configuration file: `.clang-format` in project root directory
- 📖 Usage: Please refer to [ClangFormat Official Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- 💡 Suggestion: Run clang-format before submitting code
- ⚠️ Note: Third-party code should not be formatted, please maintain original style of third-party code

> 💡 Tip: Good code style not only improves code readability but also helps team collaboration and code maintenance.

## 🤝 Contributor Code of Conduct

We are committed to providing an open, friendly and inclusive community environment for all participants.

### 🌟 Our Pledge

As community members, contributors and maintainers, we pledge to:

- 🙏 Respect everyone regardless of their background, identity or viewpoint
- 🤗 Communicate professionally and kindly
- 👂 Be open to constructive criticism
- 💪 Focus on what's best for the community
- 💬 Maintain open and transparent communication

### ⭐ Standards of Behavior

Positive behaviors include:
- ❤️ Showing empathy and kindness toward others
- 🌈 Respecting different viewpoints and experiences
- 🎯 Giving and accepting constructive feedback
- 🔄 Taking responsibility for mistakes and learning from them
- 🤲 Actively helping new members integrate into the community
- 📚 Sharing knowledge and experience

Unacceptable behaviors include:
- 🚫 Using inappropriate language or imagery
- ⛔ Personal attacks or derogatory comments
- ❌ Any form of harassment
- 🔒 Publishing others' private information without permission
- ⚠️ Other unprofessional or inappropriate conduct
- 🚯 Spreading false or misleading information

### 🛠️ Enforcement Guidelines

For code of conduct violations, community administrators will take the following actions:
1. 📝 First violation - Private warning explaining the specific issue
2. ⚡ Second violation - Public warning and request for correction
3. 🚷 Continued violations - Temporary or permanent ban from community activities
4. ⛔ Particularly serious violations may result in immediate permanent ban

### 📮 Reporting Process

To report violations, please use these channels:
- 📧 Email project administrators (githubadmin@winnermicro.com)
- 🎫 Submit an issue on GitHub Issues
- 📞 Contact community administrators directly

All reports will be:
- ⚡ Handled promptly
- 🔐 Kept confidential
- ⚖️ Investigated fairly
- 🎯 Addressed with appropriate action

> 💡 This Code of Conduct is based on the [Contributor Covenant](https://www.contributor-covenant.org), aimed at building a healthy and positive open source community.
