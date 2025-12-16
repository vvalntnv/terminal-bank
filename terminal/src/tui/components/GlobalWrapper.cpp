#include "GlobalWrapper.hpp"
#include <ftxui/dom/elements.hpp>
#include <iostream>

using namespace ftxui;

namespace tui {
    namespace components {

        Component GlobalWrapper(Component child, std::function<void()> onQuit) {
            class Impl : public ComponentBase {
            public:
                Impl(Component child, std::function<void()> onQuit) 
                    : child_(child), onQuit_(onQuit) {
                    Add(child);
                }

                Element Render() override {
                    return vbox({
                        child_->Render() | flex,
                        separator(),
                        text("Controls: Arrow Keys + Enter | Esc x2 or Ctrl+C to Quit") | dim | center
                    });
                }

                bool OnEvent(Event event) override {
                    // 1. Handle Ctrl+C
                    if (event == Event::Special({3})) {
                        onQuit_();
                        return true;
                    }

                    // 2. Pass to child first
                    bool child_handled = child_->OnEvent(event);

                    // 3. Handle Double Escape
                    if (event == Event::Escape) {
                        escape_counter++;
                        if (escape_counter >= 2) {
                            onQuit_();
                            return true;
                        }
                        // If child didn't handle it, we claim it. 
                        // If child did handle it (e.g. close menu), we still count it.
                        return true; 
                    } else {
                        // Reset counter on any other key
                        escape_counter = 0;
                    }

                    return child_handled;
                }

            private:
                Component child_;
                std::function<void()> onQuit_;
                int escape_counter = 0;
            };

            return Make<Impl>(child, onQuit);
        }

    }
}
