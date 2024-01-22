#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

class Terminal {
 public:
  [[nodiscard]] static Terminal Constant(std::string_view name);
  [[nodiscard]] static Terminal Variable(std::string_view name);

  [[nodiscard]] std::string_view name() const;
  void set_name(std::string_view name);

  [[nodiscard]] bool IsConstant() const;
  [[nodiscard]] bool IsVariable() const;
  void MakeConstant();

  [[nodiscard]] friend bool operator==(const Terminal&,
                                       const Terminal&) = default;

  friend std::ostream& operator<<(std::ostream&, const Terminal&);

 private:
  enum class Type : std::uint8_t {
    kConstant = 0,
    kVariable = 1,
  };

  Terminal(std::string_view name, Type type);

  std::string name_;
  Type type_;
};
