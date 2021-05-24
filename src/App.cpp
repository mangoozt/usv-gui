#include "App.h"
#include "MyScreen.h"
#include "oglwidget.h"
#include "usvdata/InputUtils.h"
#include "usvdata/UsvRun.h"
#include "ui/IgnorantTextBox.h"
#include "ui/ScrollableSlider.h"
#include "ui/SettingsWindow.h"
#include <iostream>

#define USV_GUI_USV_EXECUTABLE_ENV_NAME "USV_GUI_USV_EXECUTABLE"

void App::run() {
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Draw nanogui
        screen->draw_all();
    }
}

void App::initialize_gui() {
    // Create nanogui gui
    // Reload button
    ref<Button> reload_button = new Button(screen, "");
    reload_button->set_callback([this] { reload(); });
    reload_button->set_position({85, 10});
    reload_button->set_icon(FA_SYNC_ALT);

    // Open button
    ref<Button> open_button = new Button(screen, "Open");
    open_button->set_callback([this] { open(); });
    open_button->set_position({10, 10});
    open_button->set_icon(FA_FOLDER_OPEN);

    // Run usv
    run_usv_button = new Button(screen, "Run");
//    run_usv_button->set_enabled(usv_runner == nullptr);
    run_usv_button->set_callback([this] { run_case(); });
    run_usv_button->set_position({122, 10});
    run_usv_button->set_icon(FA_BRAIN);
    run_usv_button->set_enabled(false);

    // Open usv executable
    ref<Button> select_exec_button = new Button(screen, "exe...");
    select_exec_button->set_callback([this] { select_usv_executable(); });
    select_exec_button->set_position({189, 10});

    // Settings
    w_settings = new SettingsWindow(screen, &screen->map(), "Settings");

    w_settings->set_position({0,0});
    w_settings->set_visible(true);
    // color_picker.se

    // bottom
    ref<Widget> panel = new Widget(screen);
    panel->set_layout(new BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill));

    time_label = new IgnorantTextBox(panel);
    time_label->set_editable(true);
    time_label->set_fixed_width(200);
    time_label->set_value("");
    time_label->set_units("");

    slider = new ScrollableSlider(panel);
    slider->set_callback([this](float value)
                         {
                             const auto case_data = screen->map().case_data();
                             if (case_data) {
                                 auto starttime = case_data->min_time;
                                 auto endtime = case_data->max_time;
                                 auto time = starttime + (endtime - starttime) * value;
                                 update_time(time);
                             }
                         });
    slider->set_value(0.0f);
    slider->set_fixed_height(20);


    screen->set_visible(true);
    screen->perform_layout();
    panel->set_position({0, screen->height() - panel->height()});
    {
        int wwidth, wheight;
        glfwGetWindowSize(window, &wwidth, &wheight);
        panel->set_width(wwidth);
        slider->set_width(wwidth);
    }
    screen->clear();
    screen->redraw();

    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetDropCallback(window, DropCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
}

void App::load_directory(const std::string& data_directory) {
    try {
        screen->map().loadData(std::make_unique<USV::CaseData>(USV::InputUtils::loadInputData(data_directory)));
        update_time(screen->map().case_data()->min_time);
        if (slider)
            slider->set_value(0);
    } catch (std::runtime_error& e) {
        std::cout << "Couldn't open: " << e.what() << std::endl;
    }
}

namespace {
    void
    push_position(double time, const USV::Path& path, std::vector<Vessel>& vessels, double radius, glm::vec4& color, const USV::Ship* ship) {
        try {
            auto position = path.position(time);
            Vessel v{ship, position.point, position.course.radians(), radius, color};
            vessels.push_back(v);
        } catch (std::out_of_range&) {}
    }
}

void App::update_time(double time) {
    std::vector<Vessel> vessels;
    auto& map = screen->map();
    auto case_data = map.case_data();
    vessels.reserve(case_data->paths.size());
    for (const auto& pe: case_data->paths) {
        glm::vec4 color;
        switch (pe.pathType) {
            case USV::PathType::TargetManeuver:
                if (pe.ship->target_status == nullptr) {
                    color = {0, 1, 0, 1};
                } else {
                    switch (pe.ship->target_status->danger_level) {
                        case USV::DangerType::NotDangerous:
                            color = {0, 1, 0, 1};
                            break;
                        case USV::DangerType::PotentiallyDangerous:
                            color = {1.0f, 153.0f/255.0f, 51.0f/255.0f, 1.0f};
                            break;
                        case USV::DangerType::Dangerous:
                            color = {1, 0, 0, 1};
                            break;
                    }
                }
                break;
            case USV::PathType::TargetRealManeuver:
                color = {0.8f, 0.8f, 0.8f, 1.0f};
                break;
            case USV::PathType::ShipManeuver:
                color = {1.0f, 1.0f, 79.f/255.0f,1.0f};
                break;
            case USV::PathType::Route:
                color = {1.0f, 1.0f, 1.0f, 1.0f};
                break;
        }
        push_position(time, pe.path, vessels, case_data->radius, color, pe.ship);
    }

    map.updatePositions(vessels);
    map.updateTime(time / 3600);
    map.updateSunAngle(static_cast<long>(time), case_data->frame.getRefLat(), case_data->frame.getRefLon());

    if (time_label) {
        time_t seconds = static_cast<time_t>(time) - case_data->start_time;
        auto hours = seconds / 3600;
        seconds = seconds % 3600;
        auto minutes = seconds / 60;
        seconds = seconds % 60;
        char t_plus_str[32];
        snprintf(t_plus_str, sizeof(t_plus_str), "(T+%02ld:%02ld:%02d)", hours, minutes, (int) seconds);
        time_label->set_value(std::to_string((int) (time)) + t_plus_str);
        time_label->focus_event(true);
        time_label->focus_event(false);
    }
}

void App::reload() {
    auto case_data = screen->map().case_data();
    if (case_data && !case_data->directory.empty()) {
        load_directory(case_data->directory.string());
    }
}

void App::run_case() {
    auto case_data = screen->map().case_data();
    if (usv_runner && case_data && !case_data->directory.empty()) {
        usv_runner->run(case_data->directory, case_data->data_filenames);
        load_directory(case_data->directory.string());
    }
}

void App::open() {
    auto data_path = file_dialog({{"json", "JSON file"}}, false);
    if (!data_path.empty()) {
        size_t found;
        found = data_path.find_last_of("/\\");
        data_path = data_path.substr(0, found);
        load_directory(data_path);
        glfwSetWindowTitle(window, data_path.c_str());
        if (run_usv_button)
            run_usv_button->set_enabled(usv_runner != nullptr);
    }
}

// BEGIN FILE DIALOG

#if defined(_WIN32)
#  ifndef NOMINMAX
#  define NOMINMAX 1
#  endif
#  include <windows.h>
#endif

namespace {
#if !defined(__APPLE__)

    std::string special_file_dialog(const std::vector<std::pair<std::string, std::string>>& filetypes) {
        static const int FILE_DIALOG_MAX_BUFFER = 16384;
#if defined(EMSCRIPTEN)
        throw std::runtime_error("Opening files is not supported when NanoGUI is compiled via Emscripten");
#elif defined(_WIN32)
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        char tmp[FILE_DIALOG_MAX_BUFFER];
        ofn.lpstrFile = tmp;
        ZeroMemory(tmp, FILE_DIALOG_MAX_BUFFER);
        ofn.nMaxFile = FILE_DIALOG_MAX_BUFFER;
        ofn.nFilterIndex = 1;

        std::string filter;
        if(!filetypes.empty()){
            if (filetypes.size() > 1) {
                filter.append("Supported file types (");
                for (size_t i = 0; i < filetypes.size(); ++i) {
                    filter.append("*.");
                    filter.append(filetypes[i].first);
                    if (i + 1 < filetypes.size())
                        filter.append(";");
                }
                filter.append(")");
                filter.push_back('\0');
                for (size_t i = 0; i < filetypes.size(); ++i) {
                    filter.append("*.");
                    filter.append(filetypes[i].first);
                    if (i + 1 < filetypes.size())
                        filter.append(";");
                }
                filter.push_back('\0');
            }
            for (const auto& pair : filetypes) {
                filter.append(pair.second);
                filter.append(" (*.");
                filter.append(pair.first);
                filter.append(")");
                filter.push_back('\0');
                filter.append("*.");
                filter.append(pair.first);
                filter.push_back('\0');
            }
            filter.push_back('\0');
        }
        ofn.lpstrFilter = filter.data();

        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&ofn) == FALSE)
            return {};

        size_t i = 0;
        std::vector<std::string> result;
        while (tmp[i] != '\0') {
            result.emplace_back(&tmp[i]);
            i += result.back().size() + 1;
        }

        if (result.size() > 1) {
            for (i = 1; i < result.size(); ++i) {
                result[i] = result[0] + "\\" + result[i];
            }
            result.erase(begin(result));
        }
#else
        char buffer[FILE_DIALOG_MAX_BUFFER];
        buffer[0] = '\0';

        std::string cmd = "zenity --file-selection ";
        // The safest separator for multiple selected paths is /, since / can never occur
        // in file names. Only where two paths are concatenated will there be two / following
        // each other.
        cmd += "--save ";
        if (!filetypes.empty()) {
            cmd += "--file-filter=\"";
            if (!filetypes.empty()) {
                for (const auto& pair : filetypes)
                    cmd += "\"*." + pair.first + "\" ";
            } else {
                cmd += "\"*\"";
            }
            cmd += "\"";
        }

        FILE* output = popen(cmd.c_str(), "r");
        if (output == nullptr)
            throw std::runtime_error("popen() failed -- could not launch zenity!");
        while (fgets(buffer, FILE_DIALOG_MAX_BUFFER, output) != nullptr);
        pclose(output);
        std::string paths(buffer);
        paths.erase(std::remove(paths.begin(), paths.end(), '\n'), paths.end());

        std::vector<std::string> result;
        while (!paths.empty()) {
            size_t end = paths.find("//");
            if (end == std::string::npos) {
                result.emplace_back(paths);
                paths = "";
            } else {
                result.emplace_back(paths.substr(0, end));
                paths = paths.substr(end + 1);
            }
        }
#endif
        return result.empty() ? "" : result.front();
    }
#endif

    void save_usv_exec_path(std::string& path) {
        std::ofstream myfile;
        myfile.open(USV_GUI_USV_EXECUTABLE_ENV_NAME);
        myfile << path;
        myfile.close();
    }

    std::string get_usv_exec_path() {
        std::string path;
        std::ifstream myfile;
        myfile.open(USV_GUI_USV_EXECUTABLE_ENV_NAME);
        if (myfile.good()) {
            myfile >> path;
            myfile.close();
        } else
            path = "";

        return path;
    }
}
// END FILE DIALOG

void App::select_usv_executable() {
    auto executable = special_file_dialog({});
    if (!executable.empty()) {
        usv_runner = std::make_unique<USV::USVRunner>(executable);
        save_usv_exec_path(executable);
        if (run_usv_button)
            run_usv_button->set_enabled(screen->map().case_data());
    }
}

App::App(GLFWwindow* glfw_window) : screen(new MyScreen()), window(glfw_window) {
    // Create a nanogui screen and pass the glfw pointer to initialize
    screen->set_background({1.0f, 1.0f, 1.0f, 1.0f});
    screen->initialize(window, true);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowUserPointer(window, this);
    // initialize Map
    screen->map().resizeGL(width, height);
    screen->map().initializeGL();
    auto usv_executable = get_usv_exec_path();
    if (!usv_executable.empty())
        usv_runner = std::make_unique<USV::USVRunner>(usv_executable);
    initialize_gui();
}

void App::CursorPosCallback(GLFWwindow* window, double x, double y) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->cursor_pos_callback(x, y);
}

void App::MouseButtonCallback(GLFWwindow* window, int button, int action, int modifiers) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->mouse_button_callback(window, button, action, modifiers);
}

void App::CharCallback(GLFWwindow* window, unsigned int codepoint) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->char_callback_event(codepoint);
}

void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->key_callback(key, scancode, action, mods);
}

void App::ScrollCallback(GLFWwindow* window, double x, double y) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->scroll_callback(x, y);
}

void App::DropCallback(GLFWwindow* window, int count, const char** filenames) {
    App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->screen->drop_callback_event(count, filenames);
}

void App::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    auto& slider = app->slider;
    slider->parent()->set_width(width);
    slider->parent()->set_position({0, height - slider->parent()->height()});
    slider->set_width(width);
    app->screen->resize_callback_event(width, height);
    app->screen->map().resizeGL(width, height);
}

App::~App() {
    delete screen;
}
