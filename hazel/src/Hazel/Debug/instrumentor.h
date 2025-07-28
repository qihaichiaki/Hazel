#pragma once

/**
 * instrumentor 仪表器, 简单用于生成分析运行效率的工具
 * 使用地址: chrome://tracing
 */
#include <fstream>

namespace Hazel
{

struct ProfileResult
{
    std::string Name;
    long long Start, End;
    size_t ThreadID;
};

struct InstrumentationSession
{
    std::string Name;
};

class Instrumentor
{
public:
    Instrumentor() : m_current_session{nullptr}, m_profile_count{0} {}

    static Instrumentor& get()
    {
        static Instrumentor instance;
        return instance;
    }

    void writeHeader()
    {
        m_output_stream << "{\"otherData\":{},\"traceEvents\":[";
        m_output_stream.flush();
    }

    void beginSession(const std::string& name, const std::string& path = "results.json")
    {
        m_output_stream.open(path);
        writeHeader();
        m_current_session = new InstrumentationSession{name};
    }

    void writeFooter()
    {
        m_output_stream << "]}";
        m_output_stream.flush();
    }

    void endSession()
    {
        writeFooter();
        m_output_stream.close();
        if (m_current_session) {
            delete m_current_session;
            m_current_session = nullptr;
        }
        m_profile_count = 0;
    }

    void writeProfile(const ProfileResult& result)
    {
        if (m_profile_count++ > 0) {
            m_output_stream << ", ";
        }

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_output_stream << "{";
        m_output_stream << "\"cat\":\"function\",";
        m_output_stream << "\"dur\":" << (result.End - result.Start) << ',';
        m_output_stream << "\"name\":\"" << name << "\",";
        m_output_stream << "\"ph\":\"X\",";
        m_output_stream << "\"pid\":0,";
        m_output_stream << "\"tid\":" << result.ThreadID << ',';
        m_output_stream << "\"ts\":" << result.Start;
        m_output_stream << "}";

        // 缓冲区直接刷新出去
        m_output_stream.flush();
    }

private:
    InstrumentationSession* m_current_session;
    std::ofstream m_output_stream;
    int m_profile_count;
};

class InstrumentationTimer
{
public:
    InstrumentationTimer(const char* name) : m_name{name}, m_stopped{false}
    {
        m_start_time_point = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_stopped) {
            stop();
        }
    }

    void stop()
    {
        if (m_stopped) return;

        auto end_time_point = std::chrono::high_resolution_clock::now();

        long long start =
            std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time_point)
                .time_since_epoch()
                .count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time_point)
                            .time_since_epoch()
                            .count();

        size_t thread_ID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::get().writeProfile({m_name, start, end, thread_ID});

        m_stopped = true;
    }

private:
    const char* m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time_point;
    bool m_stopped;
};
}  // namespace Hazel

#define HZ_PROFILE 1

#if HZ_PROFILE
    #define HZ_PROFILE_BEGIN_SESSION(name, file_path) \
        ::Hazel::Instrumentor::get().beginSession(name, file_path)
    #define HZ_PROFILE_END_SESSION() ::Hazel::Instrumentor::get().endSession()
    #define HZ_PROFILE_SCOPE(name) ::Hazel::InstrumentationTimer __LINE__timer(name)
    #define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)
#else
    #define HZ_PROFILE_BEGIN_SESSION(name, file_path)
    #define HZ_PROFILE_END_SESSION()
    #define HZ_PROFILE_SCOPE(name)
    #define HZ_PROFILE_FUNCTION()
#endif
