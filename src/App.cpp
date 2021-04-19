#include "App.h"
#include "MyScreen.h"
#include "oglwidget.h"
#include "usvdata/InputUtils.h"
#include "ui/IgnorantTextBox.h"
#include "ui/ScrollableSlider.h"
#include <iostream>

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
}

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
    reload_button->set_position({90, 10});
    reload_button->set_icon(FA_SYNC_ALT);

    // Open button
    ref<Button> open_button = new Button(screen, "Open");
    open_button->set_callback([this] { open(); });
    open_button->set_position({10, 10});
    open_button->set_icon(FA_FOLDER_OPEN);

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
                                 auto starttime = case_data->route.getStartTime();
                                 auto endtime = case_data->route.endTime();
                                 for (const auto& maneuver: case_data->maneuvers) {
                                     endtime = std::min(endtime, maneuver.endTime());
                                     starttime = std::min(starttime, maneuver.getStartTime());
                                 }
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
    screen->draw_all();

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
        USV::CaseData case_data = USV::CaseData(
                USV::InputUtils::loadInputData(data_directory));
        screen->map().loadData(case_data);
        update_time(case_data.route.getStartTime());
        slider->set_value(0);
    } catch (std::runtime_error& e) {
        std::cout << "Couldn't open: " << e.what() << std::endl;
    }
}

namespace {
    void push_position(double time, const USV::Path& path, std::vector<USV::Vessel>& vessels, USV::Color& color,
                       double radius) {
        try {
            auto position = path.position(time);
            vessels.push_back({position.point, position.course.radians(), radius, color});
        } catch (std::out_of_range&) {}
    }
}

void App::update_time(double time) {
    std::vector<USV::Vessel> vessels;
    auto& map = screen->map();
    auto case_data = map.case_data();
    USV::Color color{0, 1, 0};
    push_position(time, case_data->route, vessels, color, case_data->radius);

    color = {0, 0, 1};
    for (size_t i = 0; i < case_data->targets_maneuvers.size(); ++i)
        push_position(time, case_data->targets_maneuvers[i], vessels, color, case_data->radius);

    color = {0.8f, 0.8f, 0.8f};
    for (const auto& maneuver: case_data->maneuvers)
        push_position(time, maneuver, vessels, color, case_data->radius);

    map.updatePositions(vessels);
    map.updateTime(time / 3600);
    map.updateSunAngle(static_cast<long>(time), case_data->frame.getRefLat(), case_data->frame.getRefLon());

    time_t seconds = static_cast<time_t>(time) - case_data->start_time;
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    char t_plus_str[16];
    snprintf(t_plus_str, sizeof(t_plus_str), "(T+%02d:%02d:%02d)", hours, minutes, (int) seconds);
    time_label->set_value(std::to_string((int) (time)) + t_plus_str);
    time_label->focus_event(true);
    time_label->focus_event(false);
}

void App::reload() {
    auto case_data = screen->map().case_data();
    if (case_data && !case_data->directory.empty()) {
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
    }
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
