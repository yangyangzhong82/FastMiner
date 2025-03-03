#pragma once
#include "ll/api/reflection/Deserialization.h"
#include "ll/api/reflection/Serialization.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <iostream>


namespace json_utils {

template <class T>
concept TData =
    ll::reflection::Reflectable<T> && std::integral<std::remove_cvref_t<decltype((std::declval<T>().version))>>;


template <class T>
[[nodiscard]] inline nlohmann::ordered_json structToJson(T& obj) {
    return ll::reflection::serialize<nlohmann::ordered_json>(obj).value();
}

template <class T>
[[nodiscard]] inline std::string structToJsonString(T& obj, int indent = -1) {
    return structToJson(obj).dump(indent);
}

template <class T, class J = nlohmann::ordered_json>
inline void jsonToStructNoMerge(J& j, T& obj) {
    ll::reflection::deserialize(obj, j).value();
}

template <TData T, class J>
[[nodiscard]] inline bool tryMergePatch(T& obj, J& data) {
    data.erase("version");
    auto patch = ll::reflection::serialize<J>(obj);
    patch.value().merge_patch(data);
    data = *std::move(patch);
    return true;
}

template <TData T, class J = nlohmann::ordered_json, class F = bool(T&, J&)>
inline void jsonToStruct(J& j, T& obj, F&& fixer = tryMergePatch<T, J>) {
    bool noNeedMerge = true;
    if (!j.contains("version") || (int64)(j["version"]) != obj.version) {
        noNeedMerge = false;
    }
    if (noNeedMerge || std::invoke(std::forward<F>(fixer), obj, j)) {
        ll::reflection::deserialize(obj, j).value();
    }
}


} // namespace json_utils