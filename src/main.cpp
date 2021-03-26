// GLFW
//
#if defined(NANOGUI_USE_OPENGL)
#  if defined(NANOGUI_GLAD)
#    if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
#      define GLAD_GLAPI_EXPORT
#    endif

#    include <glad/glad.h>

#  else
#    if defined(__APPLE__)
#      define GLFW_INCLUDE_GLCOREARB
#    else
#      define GL_GLEXT_PROTOTYPES
#    endif
#  endif
#elif defined(NANOGUI_USE_GLES)
#  define GLFW_INCLUDE_ES2
#endif

#include "oglwidget.h"
#include "usvdata/InputUtils.h"
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
#include <iostream>
#include "MyScreen.h"
#include "ui/IgnorantTextBox.h"
#include "ui/ScrollableSlider.h"

using namespace nanogui;

#define MAIN_WINDOW_WIDTH 800
#define MAIN_WINDOW_HEIGHT 600

MyScreen* screen = nullptr;
Slider* slider = nullptr;

void
push_position(double time, const USV::Path& path, std::vector<USV::Vessel>& vessels, USV::Color& color, double radius) {
    try {
        auto position = path.position(time);
        vessels.push_back({position.point, position.course.radians(), radius, color});
    } catch (std::out_of_range&) {

    }
}

void update_time(double time, OGLWidget& ogl_widget) {
    std::vector<USV::Vessel> vessels;
    auto case_data = ogl_widget.case_data();
    USV::Color color{0, 1, 0};
    push_position(time, case_data->route, vessels, color, case_data->radius);

    color = {0, 0, 1};
    for (size_t i = 0; i < case_data->targets_maneuvers.size(); ++i)
        push_position(time, case_data->targets_maneuvers[i], vessels, color, case_data->radius);

    color = {0.8f, 0.8f, 0.8f};
    for (const auto& maneuver: case_data->maneuvers)
        push_position(time, maneuver, vessels, color, case_data->radius);

    ogl_widget.updatePositions(vessels);
    ogl_widget.updateTime(time / 3600);
    ogl_widget.updateSunAngle(static_cast<long>(time), case_data->frame.getRefLat(), case_data->frame.getRefLon());
}

void load_directory(const std::string& data_directory) {
    try {
        USV::CaseData case_data = USV::CaseData(
                USV::InputUtils::loadInputData(data_directory));
        screen->map().loadData(case_data);
        update_time(case_data.route.getStartTime(), screen->map());
        slider->set_value(0);
    } catch (std::runtime_error& e) {
        std::cout << "Couldn't open: " << e.what() << std::endl;
    }
}

int main(int /* argc */, char** /* argv */) {
//HIDE OWN CONSOLE WINDOW BUT still output to CLI (DIRTY)
#ifdef WIN32
    HWND consoleWnd = GetConsoleWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId(consoleWnd, &dwProcessId);
    if (GetCurrentProcessId()==dwProcessId) FreeConsole();
#endif

    glfwInit();
    glfwSetTime(0);

#if defined(NANOGUI_USE_OPENGL)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#elif defined(NANOGUI_USE_GLES)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#elif defined(NANOGUI_USE_METAL)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    metal_init();
#endif

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, "USV-gui", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#if defined(NANOGUI_GLAD)
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif

    // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new MyScreen();
    screen->set_background({1.0f, 1.0f, 1.0f, 1.0f});
    screen->initialize(window, true);
    screen->map().resizeGL(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    screen->map().initializeGL();

#if defined(NANOGUI_USE_OPENGL) || defined(NANOGUI_USE_GLES)
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
#endif

    // Create nanogui gui
    // Reload button
    ref<Button> reload_button = new Button(screen, "");
    reload_button->set_callback([]()
                                {
                                    auto case_data = screen->map().case_data();
                                    if (case_data && !case_data->directory.empty()) {
                                        load_directory(case_data->directory.string());
                                    }
                                });
    reload_button->set_position({90, 10});
    reload_button->set_icon(FA_SYNC_ALT);

    // Open button
    ref<Button> open_button = new Button(screen, "Open");
    open_button->set_callback([window]()
                              {
                                  auto data_path = file_dialog({{"json", "JSON file"}}, false);
                                  if (!data_path.empty()) {
                                      size_t found;
                                      found = data_path.find_last_of("/\\");
                                      data_path = data_path.substr(0, found);
                                      load_directory(data_path);
                                      glfwSetWindowTitle(window, data_path.c_str());
                                  }
                              });
    open_button->set_position({10, 10});
    open_button->set_icon(FA_FOLDER_OPEN);

    ref<Widget> panel = new Widget(screen);
    panel->set_layout(new BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill));

    auto* time_label = new IgnorantTextBox(panel);
    time_label->set_editable(true);
    time_label->set_fixed_width(120);
    time_label->set_value("");
    time_label->set_units("");
    slider = new ScrollableSlider(panel);
    slider->set_callback([time_label](float value)
                         {
                             auto case_data = screen->map().case_data();
                             if (case_data) {
                                 auto starttime = case_data->route.getStartTime();
                                 auto endtime = case_data->route.endTime();
                                 for (auto& maneuver: case_data->maneuvers) {
                                     endtime = std::min(endtime, maneuver.endTime());
                                     starttime = std::min(starttime, maneuver.getStartTime());
                                 }
                                 auto time = starttime + (endtime - starttime) * value;
                                 time_label->set_value(std::to_string((int) (time)));
                                 time_label->focus_event(true);
                                 time_label->focus_event(false);
                                 update_time(time, screen->map());
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

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow*, double x, double y)
                             {
                                 screen->cursor_pos_callback(x, y);
                             }
    );

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow* w, int button, int action, int modifiers)
                               {
                                   screen->mouse_button_callback(w, button, action, modifiers);
                               }
    );

    glfwSetKeyCallback(window,
                       [](GLFWwindow*, int key, int scancode, int action, int mods)
                       {
                           screen->key_callback(key, scancode, action, mods);
                       }
    );

    glfwSetCharCallback(window,
                        [](GLFWwindow*, unsigned int codepoint)
                        {
                            screen->char_callback_event(codepoint);
                        }
    );

    glfwSetDropCallback(window,
                        [](GLFWwindow*, int count, const char** filenames)
                        {
                            screen->drop_callback_event(count, filenames);
                        }
    );

    glfwSetScrollCallback(window,
                          [](GLFWwindow*, double x, double y)
                          {
                              screen->scroll_callback(x, y);
                          }
    );



    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow*, int width, int height)
                                   {
                                       slider->parent()->set_width(width);
                                       slider->parent()->set_position({0, height - slider->parent()->height()});
                                       slider->set_width(width);
                                       screen->resize_callback_event(width, height);
                                       screen->map().resizeGL(width, height);
                                   }
    );

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Draw nanogui
        screen->draw_all();
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

#if defined(NANOGUI_USE_METAL)
    metal_shutdown();
#endif

    return 0;
}
