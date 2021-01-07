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

using namespace nanogui;

#define MAIN_WINDOW_WIDTH 800
#define MAIN_WINDOW_HEIGHT 600

class MyScreen : public Screen {
    bool lbutton_down{false};
    bool mbutton_down{false};
    OGLWidget* map_ = nullptr;
public:

    MyScreen() : Screen(), map_(new OGLWidget) {

    }

    void draw_contents() override {
        clear();
        map_->paintGL();
    }

    ~MyScreen() override {
        delete map_;
    }

    void scroll_callback(double x, double y) {
        Screen::scroll_callback_event(x, y);
        // pointer not on GUI
        if (!m_redraw) {
            map_->scroll(x, y);
            m_redraw = true;
        }
    }

    void key_callback(int key, int scancode, int action, int mods) {
        Screen::key_callback_event(key, scancode, action, mods);
        // pointer not on GUI
        if (!m_redraw) {
            map_->keyPress(key);
            m_redraw = true;
        }
    }

    void mouse_button_callback(GLFWwindow* w, int button, int action, int modifiers) {
        Screen::mouse_button_callback_event(button, action, modifiers);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if(GLFW_PRESS == action)
                lbutton_down = true;
            else if(GLFW_RELEASE == action)
                lbutton_down = false;
        }
        if(button == GLFW_MOUSE_BUTTON_MIDDLE){
            if(GLFW_PRESS == action)
                mbutton_down = true;
            else if(GLFW_RELEASE == action)
                mbutton_down = false;
        }
        // pointer not on GUI
        if (!m_redraw) {
            double xpos, ypos;
            glfwGetCursorPos(w, &xpos, &ypos);
            map_->mousePressEvent(xpos, ypos, button, action, modifiers);
            m_redraw = true;
        }
    }

    void cursor_pos_callback(double x, double y){
        Screen::cursor_pos_callback_event(x, y);
        if (!m_redraw) {
            map_->mouseMoveEvent(x, y, lbutton_down, mbutton_down);
            m_redraw = map_->uniforms_dirty();
        }
    }

    OGLWidget& map(){return *map_;}
};

class IgnorantTextBox : public TextBox {
public:
    explicit IgnorantTextBox(Widget* parent) : TextBox(parent) {};

    bool keyboard_character_event(unsigned int codepoint) override {
        return false;
    }

};

class ScrollableSlider : public Slider {
public:
    explicit ScrollableSlider(Widget* parent) : Slider(parent) {};

    bool scroll_event(const Vector2i& p, const Vector2f& rel) override {
        if (!m_enabled)
            return false;

        if (!focused()) {
            float value_delta = (rel.y() > 0) ? 1.0f : -1.0f;
            auto value = m_value + value_delta / (float) width(), old_value = m_value;
            m_value = std::min(std::max(value, m_range.first), m_range.second);
            if (m_callback && m_value != old_value)
                m_callback(m_value);
            return true;
        }
        return false;
    };
};

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
    auto* gui = new FormHelper(screen);
    ref<Window> nanogui_window = gui->add_window(Vector2i(0, 0), "");
    nanogui_window->set_layout(new GroupLayout());
    gui->add_button("Open", []()
    {
        auto file = file_dialog({{"json", "JSON file"}}, false);
        if (!file.empty()) {
            size_t found;
            found = file.find_last_of("/\\");
            USV::CaseData case_data = USV::CaseData(USV::InputUtils::loadInputData(file.substr(0, found)));
            screen->map().loadData(case_data);
            update_time(case_data.route.getStartTime(), screen->map());
        }
    });

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
