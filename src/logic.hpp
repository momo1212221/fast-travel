#pragma once
#include <cstddef>
#include <cstdint>
namespace qol {
inline std::uint16_t pad_buttons(const std::uint8_t* report) {
 return static_cast<std::uint16_t>((~report[0])&0xffu)|static_cast<std::uint16_t>(((~report[1])&0xffu)<<8);
}
inline bool combo_pressed(std::uint16_t now,std::uint16_t before,std::uint16_t mask){ return (now&mask)==mask && (before&mask)!=mask; }
inline std::size_t step(std::size_t current,std::size_t count,int delta){
 if(count==0) return 0; const auto n=static_cast<long long>(count);
 auto x=(static_cast<long long>(current)+delta)%n; if(x<0)x+=n; return static_cast<std::size_t>(x);
}
inline void consume_buttons(std::uint8_t* report, std::uint16_t mask) {
    report[0] = static_cast<std::uint8_t>(report[0] | (mask & 0xffu));
    report[1] = static_cast<std::uint8_t>(report[1] | ((mask >> 8) & 0xffu));
}
}
