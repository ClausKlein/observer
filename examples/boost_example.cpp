// Document/View sample for Boost.Signals2.
// Expands on doc_view.cpp example by using automatic
// connection management.
//
// modified by Claus Klein
//
// Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// For more information, see http://www.boost.org

#include <boost/io/ios_state.hpp>
#include <boost/signals2/signal.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

// ====================
class Document {
 public:
  typedef boost::signals2::signal<void()> signal_t;

  Document() = default;

  // connect a slot to the signal which will be emitted whenever text is appended to the document.
  boost::signals2::connection connect(const signal_t::slot_type& subscriber) { return m_sig.connect(subscriber); }

  void append(const char* s) {
    m_text += s;  // FIXME: this is not thread save! CK
    m_sig();
  }

  const std::string& getText() const {
    return m_text;  // FIXME: this is not thread save! CK
  }

 private:
  signal_t m_sig;
  std::string m_text;
};

// ====================
class TextView {
 public:
  // static factory function that sets up automatic connection tracking
  static boost::shared_ptr<TextView> create(Document& doc) {
    boost::shared_ptr<TextView> new_view(new TextView(doc));
    {
      typedef Document::signal_t::slot_type slot_type;
      slot_type myslot(&TextView::refresh, new_view.get());
      doc.connect(myslot.track(new_view));
    }
    return new_view;
  }

  void refresh() const { std::cout << "TextView: " << m_document.getText() << std::endl; }

 private:
  // private constructor to force use of static factory function
  explicit TextView(Document& doc) : m_document(doc) {}

  Document& m_document;
};

// ====================
class HexView {
 public:
  // static factory function that sets up automatic connection tracking
  static boost::shared_ptr<HexView> create(Document& doc) {
    boost::shared_ptr<HexView> new_view(new HexView(doc));
    {
      typedef Document::signal_t::slot_type slot_type;
      slot_type myslot(&HexView::refresh, new_view.get());
      doc.connect(myslot.track(new_view));
    }
    return new_view;
  }

  void refresh() const {
    boost::io::ios_flags_saver ifs(std::cout);
    const std::string& s = m_document.getText();

    std::cout << "HexView:";

    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
      std::cout << ' ' << std::hex << static_cast<int>(*it);

    std::cout << std::endl;
  }

 private:
  // private constructor to force use of static factory function
  explicit HexView(Document& doc) : m_document(doc) {}

  Document& m_document;
};

}  // namespace

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;

  Document doc;
  {
  auto v1 = TextView::create(doc); // NOTE: The Document must exists as long as the view! CK

  auto result = std::thread([&] {
    auto v2 = HexView::create(doc);

    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");
    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");
    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");

    // NOTE: destroy the HexView, automatically disconnecting
  });

  std::this_thread::sleep_for(123us);
  doc.append(argc >= 2 ? argv[1] : " Hello world!");

  result.join();
  doc.append(" ... HexView should no longer be connected!");
  }

  return 0;
}
