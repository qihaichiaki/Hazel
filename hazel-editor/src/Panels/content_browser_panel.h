#pragma once

#include <filesystem>

namespace Hazel
{

class ContentBrowserPanel
{
public:
    ContentBrowserPanel();
    ~ContentBrowserPanel();

    void onImGuiRenderer();

private:
    std::filesystem::path m_current_file_path;
};
}  // namespace Hazel