#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <nameof.hpp>


struct Base { virtual ~Base() = default; };

struct Derived : Base {};

struct SomeStruct {
  int somefield = 0;

  void SomeMethod1(int i) { somefield = i; }

  int SomeMethod2() const { return somefield; }
};

void SomeMethod3() {
  std::cout << NAMEOF(SomeMethod3) << " no called!" << std::endl;
}

template <typename T, typename U>
std::string SomeMethod4(U value) {
  auto function_name = NAMEOF(SomeMethod4<T, U>).str()
                           .append("<")
                           .append(NAMEOF_TYPE(T))
                           .append(", ")
                           .append(NAMEOF_TYPE(U))
                           .append(">(")
                           .append(NAMEOF_TYPE(U))
                           .append(" ")
                           .append(NAMEOF(value).data())
                           .append(")");

  return function_name;
}

template <typename T>
class SomeClass {
 public:
  void SomeMethod5() const {
    std::cout << nameof::nameof_type<T>() << std::endl;
  }

  template <typename C>
  C SomeMethod6() const {
    C t{};
    std::cout << NAMEOF_TYPE_EXPR(t) << std::endl;
    return t;
  }
};

struct Long {
  struct LL {
    int field = 0;
  };
  LL ll;
};

enum class Color { RED, GREEN, BLUE };

enum AnimalFlags { HasClaws = 1 << 0, CanFly = 1 << 1, EatsFish = 1 << 2, Endangered = 1 << 3 };

SomeStruct structvar;
Long othervar;
SomeStruct* ptrvar = &structvar;

void name_to_chars(const char* name) {
  std::cout << name << std::endl;
}

void name_to_string(const std::string& name) {
  std::cout << name << std::endl;
}

void name_to_string_view(std::string_view name) {
  std::cout << name << std::endl;
}

int main() {
  // Compile-time.
  constexpr auto name = NAMEOF(structvar);
  using namespace std::literals::string_view_literals;
  static_assert("structvar"sv == name);

  name_to_chars(name.c_str()); // 'structvar'
  // or name_to_chars(name.data());
  // Note: c_str() return name as null-terminated C string, no memory allocation.

  name_to_string(name.str()); // 'structvar'
  // Note: str() occure memory allocation to copy name to std::string.
  // or name_to_string(std::string{name});
  // or name_to_string(static_cast<std::string>(name));
  // Note: cast to std::string occure memory allocation to copy name to std::string.

  name_to_string_view(name); // 'structvar'
  // Note: Implicit cast to std::string_view, no memory allocation.

#if defined(NAMEOF_ENUM_SUPPORTED)
  // Nameof enum variable.
  auto color = Color::RED;
  std::cout << nameof::nameof_enum(color) << std::endl; // 'RED'
  std::cout << NAMEOF_ENUM(color) << std::endl; // 'RED'
  std::cout << nameof::nameof_enum<Color::GREEN>() << std::endl; // 'GREEN'

  // Nameof enum flags.
  auto flag = static_cast<AnimalFlags>(AnimalFlags::CanFly | AnimalFlags::EatsFish);
  std::cout << nameof::nameof_enum(flag) << std::endl; // 'CanFly|EatsFish'
  std::cout << NAMEOF_ENUM(flag) << std::endl; // 'CanFly|EatsFish'
#endif

  // Nameof.
  std::cout << NAMEOF(structvar) << std::endl; // 'structvar'
  std::cout << NAMEOF(::structvar) << std::endl; // 'structvar'
  std::cout << NAMEOF(structvar.somefield) << std::endl; // 'somefield'
  std::cout << NAMEOF((&structvar)->somefield) << std::endl; // 'somefield'
  std::cout << NAMEOF(othervar.ll.field) << std::endl; // 'field'
  std::cout << NAMEOF(ptrvar) << std::endl; // 'ptrvar

  // Nameof function.
  std::cout << NAMEOF(&SomeStruct::SomeMethod1) << std::endl; // 'SomeMethod1'
  std::cout << NAMEOF(structvar.SomeMethod2()) << std::endl; // 'SomeMethod2'
  std::cout << NAMEOF(SomeMethod3) << std::endl; // 'SomeMethod3'
  std::cout << NAMEOF(SomeMethod4<int, float>(1.0f)) << std::endl; // 'SomeMethod4'
  std::cout << NAMEOF(&SomeClass<int>::SomeMethod5) << std::endl; // 'SomeMethod5'
  std::cout << NAMEOF(&SomeClass<int>::SomeMethod6<long int>) << std::endl; // 'SomeMethod6'

  // Nameof with template suffix.
  std::cout << NAMEOF_FULL(SomeMethod4<int, float>) << std::endl; // 'SomeMethod4<int, float>'
  std::cout << NAMEOF_FULL(&SomeClass<int>::SomeMethod6<long int>) << std::endl; // 'SomeMethod6<long int>'

  // Nameof type.
  std::cout << nameof::nameof_type<const Long::LL&>() << std::endl; // 'Long::LL'
  std::cout << NAMEOF_TYPE(const Long::LL&) << std::endl; // 'Long::LL'
  std::cout << nameof::nameof_full_type<const Long::LL&>() << std::endl; // 'const Long::LL &'
  std::cout << NAMEOF_FULL_TYPE(const Long::LL&) << std::endl; // 'const Long::LL &'

  // Nameof variable type.
  std::cout << nameof::nameof_type<decltype(structvar)>() << std::endl; // 'SomeStruct'
  std::cout << NAMEOF_TYPE_EXPR(structvar) << std::endl; // 'SomeStruct'
  std::cout << NAMEOF_TYPE_EXPR(std::declval<const SomeClass<int>>()) << std::endl; // 'SomeClass<int>'
  std::cout << NAMEOF_FULL_TYPE_EXPR(std::declval<const SomeClass<int>>()) << std::endl; // 'const SomeClass<int> &&'

  // Nameof macro.
  std::cout << NAMEOF(__LINE__) << std::endl; // '__LINE__'

  // Nameof raw.
  std::cout << NAMEOF_RAW(structvar.somefield) << std::endl; // 'structvar.somefield'
  std::cout << NAMEOF_RAW(&SomeStruct::SomeMethod1) << std::endl; // '&SomeStruct::SomeMethod1'

  // Some more complex example.

  std::cout << SomeMethod4<int>(structvar) << std::endl; // 'SomeMethod4<int, SomeStruct>(SomeStruct value)'

  auto div = [](int x, int y) -> int {
    if (y == 0) {
      throw std::invalid_argument(NAMEOF(y).str() + " should not be zero!");
    }
    return x / y;
  };

  try {
    auto z = div(10, 0);
    std::cout << z << std::endl;
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl; // 'y should not be zero!'
  }

  return 0;
}
