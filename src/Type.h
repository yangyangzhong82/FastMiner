#pragma once


namespace fm {

using BlockID = short;

namespace tag {

struct CommandTag;

struct ConfigModelTag;
struct BlockConfigTag;
struct ConfigFactoryTag;

} // namespace tag

namespace internal {

template <typename T>
struct ImplType;

}

} // namespace fm