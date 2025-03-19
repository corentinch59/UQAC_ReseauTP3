#pragma once

#include <spdlog/common.h>
#include <spdlog/sinks/base_sink.h>

#include <string>

#include <imgui.h>

#include <deque>

template <typename Mutex>
class ImGuiSpdlogSink : public spdlog::sinks::base_sink<Mutex> {
  protected:
    struct Line {
        ImColor color;
        std::string text;
    };

    void sink_it_(const spdlog::details::log_msg &msg) override {

        // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
        // msg.payload (before v1.3.0: msg.raw) contains pre formatted log

        // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

        ImColor color = ImColor(255, 255, 255, 255);
        switch (msg.level) {
        case spdlog::level::warn:
            color = ImColor(255, 216, 0, 255);
            break;
        case spdlog::level::err:
            color = ImColor(255, 0, 0, 255);
            break;
        case spdlog::level::debug:
            color = ImColor(0, 220, 255, 255);
            break;
        default:
            break;
        }
        mLines.push_back({color, fmt::to_string(formatted)});

        while (mLines.size() > mMaxLineCount) {
            mLines.pop_front();
        }
    }

    void flush_() override {
        mLines.clear();
    }

  public:
    float currentScroll = 0;
    bool mAutoScroll = true;

    void DrawConsole(float dt, const char *name, ImGuiWindowFlags flags) {
        ImGui::Begin(name, nullptr, flags | ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar()) {
            ImGui::MenuItem("Auto scroll", NULL, &mAutoScroll);
            ImGui::EndMenuBar();
        }
        for (auto line : mLines) {
            ImGui::PushStyleColor(ImGuiCol_Text, line.color.Value);
            ImGui::Text(line.text.c_str());
            ImGui::PopStyleColor();
        }
        if (mAutoScroll)
            ImGui::SetScrollY(ImGui::GetScrollMaxY());
        ImGui::End();
    }

  private:
    int mMaxLineCount = 2048;
    std::deque<Line> mLines;
};

#include <spdlog/details/null_mutex.h>
#include <mutex>

using ImGuiSpdlogSinkMt = ImGuiSpdlogSink<std::mutex>;
using ImGuiSpdlogSinkSt = ImGuiSpdlogSink<spdlog::details::null_mutex>;