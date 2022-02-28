// Document/View sample for Boost.Signals
//
// modified by Claus Klein
//
// Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// For more information, see http://www.boost.org

#include <boost/signals2/signal.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace {

/// document_def_code_snippet
class Document {
 public:
  typedef boost::signals2::signal<void()> signal_t;

 public:
  Document() = default;

  // Connect a slot to the signal which will be emitted whenever text is appended to the document.
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

/// text_view_def_code_snippet
class TextView {
 public:
  TextView(Document& doc) : m_document(doc) { m_connection = m_document.connect(std::bind(&TextView::refresh, this)); }

  ~TextView() { m_connection.disconnect(); }

  void refresh() const { std::cout << "TextView: " << m_document.getText() << std::endl; }

 private:
  Document& m_document;
  boost::signals2::connection m_connection;
};

/// hex_view_def_code_snippet
class HexView {
 public:
  HexView(Document& doc) : m_document(doc) { m_connection = m_document.connect(std::bind(&HexView::refresh, this)); }

  ~HexView() { m_connection.disconnect(); }

  void refresh() const {
    const std::string& s = m_document.getText();

    std::cout << "HexView:";

    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
      std::cout << ' ' << std::hex << static_cast<int>(*it);

    std::cout << std::endl;
  }

 private:
  Document& m_document;
  boost::signals2::connection m_connection;
};

}  // namespace

/// document_view_main_code_snippet
int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;

  Document doc;
  TextView v1(doc); // NOTE: The Document must exists as long as the view! CK

  auto result = std::thread([&] {
    HexView v2(doc);
    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");
    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");
    std::this_thread::sleep_for(100us);
    doc.append(" ... conccurend signal!");
  });

  std::this_thread::sleep_for(123us);
  doc.append(argc == 2 ? argv[1] : " Hello world!");

  result.join();
  doc.append(" ... Again, but without HexView!");

  return 0;
}
