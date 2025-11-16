/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#ifndef _C_UTILTIY_H_
#define _C_UTILTIY_H_

//#include <thread>
//#include <string>
//#include <functional>
//#if defined(_MSC_VER)
//#include <Windows.h>
//#endif

// standard includes
#include <algorithm>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#define KITTY_WHILE_LOOP(x, y, z) \
  { \
    x; \
    while (y) z \
  }

template<typename T>
struct argument_type;

template<typename T, typename U>
struct argument_type<T(U)> {
  typedef U type;
};

#define KITTY_USING_MOVE_T(move_t, t, init_val, z) \
  class move_t { \
  public: \
    using element_type = typename argument_type<void(t)>::type; \
\
    move_t(): \
        el {init_val} { \
    } \
    template<class... Args> \
    move_t(Args &&...args): \
        el {std::forward<Args>(args)...} { \
    } \
    move_t(const move_t &) = delete; \
\
    move_t(move_t &&other) noexcept: \
        el {std::move(other.el)} { \
      other.el = element_type {init_val}; \
    } \
\
    move_t &operator=(const move_t &) = delete; \
\
    move_t &operator=(move_t &&other) { \
      std::swap(el, other.el); \
      return *this; \
    } \
    element_type *operator->() { \
      return &el; \
    } \
    const element_type *operator->() const { \
      return &el; \
    } \
\
    inline element_type release() { \
      element_type val = std::move(el); \
      el = element_type {init_val}; \
      return val; \
    } \
\
    ~move_t() z \
\
      element_type el; \
  }

#define KITTY_DECL_CONSTR(x) \
  x(x &&) noexcept = default; \
  x &operator=(x &&) noexcept = default; \
  x();

#define KITTY_DEFAULT_CONSTR_MOVE(x) \
  x(x &&) noexcept = default; \
  x &operator=(x &&) noexcept = default;

#define KITTY_DEFAULT_CONSTR_MOVE_THROW(x) \
  x(x &&) = default; \
  x &operator=(x &&) = default; \
  x() = default;

#define KITTY_DEFAULT_CONSTR(x) \
  KITTY_DEFAULT_CONSTR_MOVE(x) \
  x(const x &) noexcept = default; \
  x &operator=(const x &) = default;

#define TUPLE_2D(a, b, expr) \
  decltype(expr) a##_##b = expr; \
  auto &a = std::get<0>(a##_##b); \
  auto &b = std::get<1>(a##_##b)

#define TUPLE_2D_REF(a, b, expr) \
  auto &a##_##b = expr; \
  auto &a = std::get<0>(a##_##b); \
  auto &b = std::get<1>(a##_##b)

#define TUPLE_3D(a, b, c, expr) \
  decltype(expr) a##_##b##_##c = expr; \
  auto &a = std::get<0>(a##_##b##_##c); \
  auto &b = std::get<1>(a##_##b##_##c); \
  auto &c = std::get<2>(a##_##b##_##c)

#define TUPLE_3D_REF(a, b, c, expr) \
  auto &a##_##b##_##c = expr; \
  auto &a = std::get<0>(a##_##b##_##c); \
  auto &b = std::get<1>(a##_##b##_##c); \
  auto &c = std::get<2>(a##_##b##_##c)

#define TUPLE_EL(a, b, expr) \
  decltype(expr) a##_ = expr; \
  auto &a = std::get<b>(a##_)

#define TUPLE_EL_REF(a, b, expr) \
  auto &a = std::get<b>(expr)

namespace chen {

  template<template<typename...> class X, class... Y>
  struct __instantiation_of: public std::false_type {};

  template<template<typename...> class X, class... Y>
  struct __instantiation_of<X, X<Y...>>: public std::true_type {};

  template<template<typename...> class X, class T, class... Y>
  static constexpr auto instantiation_of_v = __instantiation_of<X, T, Y...>::value;

  template<bool V, class X, class Y>
  struct __either;

  template<class X, class Y>
  struct __either<true, X, Y> {
    using type = X;
  };

  template<class X, class Y>
  struct __either<false, X, Y> {
    using type = Y;
  };

  template<bool V, class X, class Y>
  using either_t = typename __either<V, X, Y>::type;

  template<class... Ts>
  struct overloaded: Ts... {
    using Ts::operator()...;
  };
  //template<class... Ts>
  //overloaded(Ts...) -> overloaded<Ts...>;

  template<class T>
  class FailGuard {
  public:
    FailGuard() = delete;

    FailGuard(T &&f) noexcept:
        _func {std::forward<T>(f)} {
    }

    FailGuard(FailGuard &&other) noexcept:
        _func {std::move(other._func)} {
      this->failure = other.failure;

      other.failure = false;
    }

    FailGuard(const FailGuard &) = delete;

    FailGuard &operator=(const FailGuard &) = delete;
    FailGuard &operator=(FailGuard &&other) = delete;

    ~FailGuard() noexcept {
      if (failure) {
        _func();
      }
    }

    void disable() {
      failure = false;
    }

    bool failure {true};

  private:
    T _func;
  };

  template<class T>
  [[nodiscard]] auto fail_guard(T &&f) {
    return FailGuard<T> {std::forward<T>(f)};
  }

  template<class T>
  void append_struct(std::vector<uint8_t> &buf, const T &_struct) {
    constexpr size_t data_len = sizeof(_struct);

    buf.reserve(data_len);

    auto *data = (uint8_t *) &_struct;

    for (size_t x = 0; x < data_len; ++x) {
      buf.push_back(data[x]);
    }
  }

  template<class T>
  class Hex {
  public:
    typedef T elem_type;

  private:
    const char _bits[16] {
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'A',
      'B',
      'C',
      'D',
      'E',
      'F'
    };

    char _hex[sizeof(elem_type) * 2];

  public:
    Hex(const elem_type &elem, bool rev) {
      if (!rev) {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(&elem) + sizeof(elem_type) - 1;
        for (auto it = begin(); it < cend();) {
          *it++ = _bits[*data / 16];
          *it++ = _bits[*data-- % 16];
        }
      } else {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(&elem);
        for (auto it = begin(); it < cend();) {
          *it++ = _bits[*data / 16];
          *it++ = _bits[*data++ % 16];
        }
      }
    }

    char *begin() {
      return _hex;
    }

    char *end() {
      return _hex + sizeof(elem_type) * 2;
    }

    const char *begin() const {
      return _hex;
    }

    const char *end() const {
      return _hex + sizeof(elem_type) * 2;
    }

    const char *cbegin() const {
      return _hex;
    }

    const char *cend() const {
      return _hex + sizeof(elem_type) * 2;
    }

    std::string to_string() const {
      return {begin(), end()};
    }

    std::string_view to_string_view() const {
      return {begin(), sizeof(elem_type) * 2};
    }
  };

  template<class T>
  Hex<T> hex(const T &elem, bool rev = false) {
    return Hex<T>(elem, rev);
  }

  template<typename T>
  std::string log_hex(const T &value) {
    return "0x" + Hex<T>(value, false).to_string();
  }

  template<class It>
  std::string hex_vec(It begin, It end, bool rev = false) {
    auto str_size = 2 * std::distance(begin, end);

    std::string hex;
    hex.resize(str_size);

    const char _bits[16] {
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'A',
      'B',
      'C',
      'D',
      'E',
      'F'
    };

    if (rev) {
      for (auto it = std::begin(hex); it < std::end(hex);) {
        *it++ = _bits[((uint8_t) *begin) / 16];
        *it++ = _bits[((uint8_t) *begin++) % 16];
      }
    } else {
      --end;
      for (auto it = std::begin(hex); it < std::end(hex);) {
        *it++ = _bits[((uint8_t) *end) / 16];
        *it++ = _bits[((uint8_t) *end--) % 16];
      }
    }

    return hex;
  }

  template<class C>
  std::string hex_vec(C &&c, bool rev = false) {
    return hex_vec(std::begin(c), std::end(c), rev);
  }

  template<class T>
  T from_hex(const std::string &hex, bool rev = false) {
    std::uint8_t buf[sizeof(T)];

    static char constexpr shift_bit = 'a' - 'A';

    auto is_convertable = [](char ch) -> bool {
      if (isdigit(ch)) {
        return true;
      }

      ch |= shift_bit;

      if ('a' > ch || ch > 'z') {
        return false;
      }

      return true;
    };

    auto buf_size = std::count_if(std::begin(hex), std::end(hex), is_convertable) / 2;
    auto padding = sizeof(T) - buf_size;

    const char *data = hex.data() + hex.size() - 1;

    auto convert = [](char ch) -> std::uint8_t {
      if (ch >= '0' && ch <= '9') {
        return (std::uint8_t) ch - '0';
      }

      return (std::uint8_t) (ch | (char) 32) - 'a' + (char) 10;
    };

    std::fill_n(buf + buf_size, padding, 0);

    std::for_each_n(buf, buf_size, [&](auto &el) {
      while (!is_convertable(*data)) {
        --data;
      }
      std::uint8_t ch_r = convert(*data--);

      while (!is_convertable(*data)) {
        --data;
      }
      std::uint8_t ch_l = convert(*data--);

      el = (ch_l << 4) | ch_r;
    });

    if (rev) {
      std::reverse(std::begin(buf), std::end(buf));
    }

    return *reinterpret_cast<T *>(buf);
  }

  inline std::string from_hex_vec(const std::string &hex, bool rev = false) {
    std::string buf;

    static char constexpr shift_bit = 'a' - 'A';
    auto is_convertable = [](char ch) -> bool {
      if (isdigit(ch)) {
        return true;
      }

      ch |= shift_bit;

      if ('a' > ch || ch > 'z') {
        return false;
      }

      return true;
    };

    auto buf_size = std::count_if(std::begin(hex), std::end(hex), is_convertable) / 2;
    buf.resize(buf_size);

    const char *data = hex.data() + hex.size() - 1;

    auto convert = [](char ch) -> std::uint8_t {
      if (ch >= '0' && ch <= '9') {
        return (std::uint8_t) ch - '0';
      }

      return (std::uint8_t) (ch | (char) 32) - 'a' + (char) 10;
    };

    for (auto &el : buf) {
      while (!is_convertable(*data)) {
        --data;
      }
      std::uint8_t ch_r = convert(*data--);

      while (!is_convertable(*data)) {
        --data;
      }
      std::uint8_t ch_l = convert(*data--);

      el = (ch_l << 4) | ch_r;
    }

    if (rev) {
      std::reverse(std::begin(buf), std::end(buf));
    }

    return buf;
  }

  template<class T>
  class hash {
  public:
    using value_type = T;

    std::size_t operator()(const value_type &value) const {
      const auto *p = reinterpret_cast<const char *>(&value);

      return std::hash<std::string_view> {}(std::string_view {p, sizeof(value_type)});
    }
  };

  template<class T>
  auto enm(const T &val) -> const std::underlying_type_t<T> & {
    return *reinterpret_cast<const std::underlying_type_t<T> *>(&val);
  }

  template<class T>
  auto enm(T &val) -> std::underlying_type_t<T> & {
    return *reinterpret_cast<std::underlying_type_t<T> *>(&val);
  }

  inline std::int64_t from_chars(const char *begin, const char *end) {
    if (begin == end) {
      return 0;
    }

    std::int64_t res {};
    std::int64_t mul = 1;
    while (begin != --end) {
      res += (std::int64_t) (*end - '0') * mul;

      mul *= 10;
    }

    return *begin != '-' ? res + (std::int64_t) (*begin - '0') * mul : -res;
  }

  

  template<class X, class Y>
  class Either: public std::variant<std::monostate, X, Y> {
  public:
    using std::variant<std::monostate, X, Y>::variant;

    constexpr bool has_left() const {
      return std::holds_alternative<X>(*this);
    }

    constexpr bool has_right() const {
      return std::holds_alternative<Y>(*this);
    }

    X &left() {
      return std::get<X>(*this);
    }

    Y &right() {
      return std::get<Y>(*this);
    }

    const X &left() const {
      return std::get<X>(*this);
    }

    const Y &right() const {
      return std::get<Y>(*this);
    }
  };

  // Compared to std::unique_ptr, it adds the ability to get the address of the pointer itself
  template<typename T, typename D = std::default_delete<T>>
  class uniq_ptr {
  public:
    using element_type = T;
    using pointer = element_type *;
    using const_pointer = element_type const *;
    using deleter_type = D;

    constexpr uniq_ptr() noexcept:
        _p {nullptr} {
    }

    constexpr uniq_ptr(std::nullptr_t) noexcept:
        _p {nullptr} {
    }

    uniq_ptr(const uniq_ptr &other) noexcept = delete;
    uniq_ptr &operator=(const uniq_ptr &other) noexcept = delete;

    template<class V>
    uniq_ptr(V *p) noexcept:
        _p {p} {
      static_assert(std::is_same_v<element_type, void> || std::is_same_v<element_type, V> || std::is_base_of_v<element_type, V>, "element_type must be base class of V");
    }

    template<class V>
    uniq_ptr(std::unique_ptr<V, deleter_type> &&uniq) noexcept:
        _p {uniq.release()} {
      static_assert(std::is_same_v<element_type, void> || std::is_same_v<T, V> || std::is_base_of_v<element_type, V>, "element_type must be base class of V");
    }

    template<class V>
    uniq_ptr(uniq_ptr<V, deleter_type> &&other) noexcept:
        _p {other.release()} {
      static_assert(std::is_same_v<element_type, void> || std::is_same_v<T, V> || std::is_base_of_v<element_type, V>, "element_type must be base class of V");
    }

    template<class V>
    uniq_ptr &operator=(uniq_ptr<V, deleter_type> &&other) noexcept {
      static_assert(std::is_same_v<element_type, void> || std::is_same_v<T, V> || std::is_base_of_v<element_type, V>, "element_type must be base class of V");
      reset(other.release());

      return *this;
    }

    template<class V>
    uniq_ptr &operator=(std::unique_ptr<V, deleter_type> &&uniq) noexcept {
      static_assert(std::is_same_v<element_type, void> || std::is_same_v<T, V> || std::is_base_of_v<element_type, V>, "element_type must be base class of V");

      reset(uniq.release());

      return *this;
    }

    ~uniq_ptr() {
      reset();
    }

    void reset(pointer p = pointer()) {
      if (_p) {
        _deleter(_p);
      }

      _p = p;
    }

    pointer release() {
      auto tmp = _p;
      _p = nullptr;
      return tmp;
    }

    pointer get() {
      return _p;
    }

    const_pointer get() const {
      return _p;
    }

    std::add_lvalue_reference_t<element_type const> operator*() const {
      return *_p;
    }

    std::add_lvalue_reference_t<element_type> operator*() {
      return *_p;
    }

    const_pointer operator->() const {
      return _p;
    }

    pointer operator->() {
      return _p;
    }

    pointer *operator&() const {
      return &_p;
    }

    pointer *operator&() {
      return &_p;
    }

    deleter_type &get_deleter() {
      return _deleter;
    }

    const deleter_type &get_deleter() const {
      return _deleter;
    }

    explicit operator bool() const {
      return _p != nullptr;
    }

  protected:
    pointer _p;
    deleter_type _deleter;
  };

  template<class T1, class D1, class T2, class D2>
  bool operator==(const uniq_ptr<T1, D1> &x, const uniq_ptr<T2, D2> &y) {
    return x.get() == y.get();
  }

  template<class T1, class D1, class T2, class D2>
  bool operator!=(const uniq_ptr<T1, D1> &x, const uniq_ptr<T2, D2> &y) {
    return x.get() != y.get();
  }

  template<class T1, class D1, class T2, class D2>
  bool operator==(const std::unique_ptr<T1, D1> &x, const uniq_ptr<T2, D2> &y) {
    return x.get() == y.get();
  }

  template<class T1, class D1, class T2, class D2>
  bool operator!=(const std::unique_ptr<T1, D1> &x, const uniq_ptr<T2, D2> &y) {
    return x.get() != y.get();
  }

  template<class T1, class D1, class T2, class D2>
  bool operator==(const uniq_ptr<T1, D1> &x, const std::unique_ptr<T1, D1> &y) {
    return x.get() == y.get();
  }

  template<class T1, class D1, class T2, class D2>
  bool operator!=(const uniq_ptr<T1, D1> &x, const std::unique_ptr<T1, D1> &y) {
    return x.get() != y.get();
  }

  template<class T, class D>
  bool operator==(const uniq_ptr<T, D> &x, std::nullptr_t) {
    return !(bool) x;
  }

  template<class T, class D>
  bool operator!=(const uniq_ptr<T, D> &x, std::nullptr_t) {
    return (bool) x;
  }

  template<class T, class D>
  bool operator==(std::nullptr_t, const uniq_ptr<T, D> &y) {
    return !(bool) y;
  }

  template<class T, class D>
  bool operator!=(std::nullptr_t, const uniq_ptr<T, D> &y) {
    return (bool) y;
  }

  template<class P>
  using shared_t = std::shared_ptr<typename P::element_type>;

  template<class P, class T>
  shared_t<P> make_shared(T *pointer) {
    return shared_t<P>(reinterpret_cast<typename P::pointer>(pointer), typename P::deleter_type());
  }
   
}  // namespace chen
#endif // C_UTILTIY_H_