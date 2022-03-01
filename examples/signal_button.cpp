#include "Signal.h"

#include <iostream>
#include <string>

class Button {
 public:
  Signal<> on_click;
};

class Message {
 public:
  explicit Message(std::string message) : message_(std::move(message)) {}
  void display() const { std::cout << message_ << std::endl; }

 private:
  std::string message_;
};

int main() {
  Button button;
  Message message1("Hello World!");

  {
    Message message2("Say good by!");

    const auto connectionID = button.on_click.connect_member(&message1, &Message::display);
    (void)button.on_click.connect_member(&message2, &Message::display);
    button.on_click.emit();

    button.on_click.disconnect(connectionID);
    button.on_click.emit();
  }

  Message message3("!!!! ZOMBIE !!!!!");
  message3.display();
  button.on_click.emit();  // FIXME: this emits ZOMBIE again! CK

  button.on_click.disconnect_all();
  button.on_click.disconnect(-1);

  message3 = message1;

  (void)button.on_click.connect_member(&message3, &Message::display);
  button.on_click.emit();

  return 0;
}
