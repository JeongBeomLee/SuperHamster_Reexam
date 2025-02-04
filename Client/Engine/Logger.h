#pragma once

// �α� ���� ����
enum class LogLevel {
    Debug,      // ���߿� �� ����
    Info,       // �Ϲ����� ����
    Warning,    // ������ ����
    Error,      // �ɰ��� ����
    Fatal       // ġ���� ����
};

// �α� ��� �������̽�
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    virtual void Write(LogLevel level, const std::string& message) = 0;
};

// ����� â ���
class DebugOutput : public ILogOutput {
public:
    void Write(LogLevel level, const std::string& message) override {
        OutputDebugStringA(message.c_str());
    }
};

// ���� ���
class FileOutput : public ILogOutput {
public:
    explicit FileOutput(const std::string& filename) {
        m_file.open(filename, std::ios::out | std::ios::app);
        if (!m_file.is_open()) {
            throw std::runtime_error("Failed to open log file");
        }
    }
    ~FileOutput() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }
    void Write(LogLevel level, const std::string& message) override {
        if (m_file.is_open()) {
            m_file << message;
            m_file.flush();
        }
    }

private:
    std::ofstream m_file;
};

// �ΰ� Ŭ���� (�̱���)
class Logger {
public:
    static Logger& Instance();

    // �α� ��� �Լ���
    template<typename... Args>
    void Debug(const std::string& format, Args... args);

    template<typename... Args>
    void Info(const std::string& format, Args... args);

    template<typename... Args>
    void Warning(const std::string& format, Args... args);

    template<typename... Args>
    void Error(const std::string& format, Args... args);

    template<typename... Args>
    void Fatal(const std::string& format, Args... args);

    // ��� ��� �߰�/����
    void AddOutput(std::unique_ptr<ILogOutput> output);
    void RemoveAllOutputs();

    // ����/�̵� ����
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;

    template<typename... Args>
    void Log(LogLevel level, const std::string& format, Args... args);

    std::string FormatLogMessage(const std::string& message, LogLevel level) const;
    std::string GetLogLevelString(LogLevel level) const;
    std::string GetTimeString() const;

    std::mutex m_mutex;
    std::vector<std::unique_ptr<ILogOutput>> m_outputs;
};

// ���ø� �Լ� ����
template<typename... Args>
void Logger::Debug(const std::string& format, Args... args) {
    Log(LogLevel::Debug, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Info(const std::string& format, Args... args) {
    Log(LogLevel::Info, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Warning(const std::string& format, Args... args) {
    Log(LogLevel::Warning, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Error(const std::string& format, Args... args) {
    Log(LogLevel::Error, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Fatal(const std::string& format, Args... args) {
    Log(LogLevel::Fatal, format, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Log(LogLevel level, const std::string& format, Args... args) {
    try {
        std::string message = std::vformat(format, std::make_format_args(args...));
        message = FormatLogMessage(message, level);

        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& output : m_outputs) {
            output->Write(level, message);
        }
    }
    catch (const std::exception& e) {
        // ������ ���� �� �⺻ ���� �޽��� ���
        std::string errorMessage = std::format("Logging failed: {}", e.what());
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& output : m_outputs) {
            output->Write(LogLevel::Error, errorMessage);
        }
    }
}