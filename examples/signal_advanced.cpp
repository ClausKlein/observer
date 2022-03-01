#include "Signal.h"

#include <iostream>
#include <string>

class Person {
 public:
  explicit Person(std::string name) : name_(std::move(name)) {}

  Signal<std::string const&> say;

  void listen(std::string const& message) const { std::cout << name_ << " received: " << message << std::endl; }

 private:
  std::string name_;
};

int main() {
  Person alice("Alice");
  Person bob("Bob");

  auto id = alice.say.connect_member(&bob, &Person::listen);
  bob.say.connect_member(&alice, &Person::listen);

  alice.say.emit("Have a nice day!");
  bob.say.emit("Thank you!");

  alice.say.emit_for_all_but_one(id, "Hidden message!");
  alice.say.emit_for(id, "Good by!");

  return 0;
}
