#pragma once
///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <optional>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <Helper/EJsonExistance.hpp>
#include <Helper/XJsonCallback.hpp>

//!
//! Implementation
//!

namespace ray::json
{

/// @brief Check key is exist in present json instance.
/// @param json json atlas to find `key`.
/// @param key Key string to find in `json`.
/// @return If found, return true.
[[nodiscard]] bool HasJsonKey(const nlohmann::json& json, const std::string& key) noexcept;

/// @brief Read json file and return json container. 
/// If any error has happended, just return null value.
/// @param iFilePath File path to read.
/// @return If successful to read file, return json atlas instance.
std::optional<nlohmann::json> GetAtlasFromFile(const std::string& iFilePath) noexcept;

/// @brief Read json file and return json container. 
/// If any error has happended, just return null value.
/// @param iFilePath File path to read.
/// @return If successful to read file, return json atlas instance.
std::optional<nlohmann::json> GetAtlasFromFile(const std::filesystem::path& iFilePath) noexcept;

/// @brief Read json structurized string buffer and return json container. 
/// If any error has happended, just return null value.
/// @param iSerializedString json serialized string buffer.
/// @return If successful to read file, return json atlas instance.
std::optional<nlohmann::json> GetAtlasFromString(const std::string& iSerializedString) noexcept;

/// @brief  Get `TReturnType` value in `name` key from json atlas. 
/// @param  jsonAtlas Immutable valid json atlas instance.
/// @param  name Key header string to find.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TParam1 Automatically bound json atlas type
/// @return Value instance from jsonAtlas if found.
template <typename TReturnType, typename TParam1>
TReturnType GetValueFrom(const TParam1& jsonAtlas, const char* name);

/// @brief  Get `TReturnType` value in `name` key from json atlas. 
/// @param  jsonAtlas Immutable valid json atlas instance.
/// @param  name Key header string to find.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TParam1 Automatically bound json atlas type
/// @return Value instance from jsonAtlas if found.
template <typename TReturnType, typename TParam1>
TReturnType GetValueFrom(const TParam1& jsonAtlas, const std::string_view& name);

/// @brief  Get `TReturnType` value from json atlas. 
/// @param  jsonAtlas Immutable valid json atlas instance.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TJsonParam Automatically bound json atlas type
/// @return Value instance from jsonAtlas if found.
template <typename TReturnType, typename TJsonParam>
TReturnType GetValue(const TJsonParam& jsonAtlas);

/// @brief This functions requires additional callback function that checks whether key is exist or not exist.
/// @param jsonAtlas Immutable valid json atlas.
/// @param name Key name to get value from.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TJsonParam Automatically bound json atlas type
/// @return First value is returned value and second is Existance flag list.
template <typename TReturnType, typename TJsonParam>
std::pair<TReturnType, FExistanceList> 
GetValueFromOptionally(const TJsonParam& jsonAtlas, const char* name); 

/// @brief This functions requires additional callback function that checks whether key is exist or not exist.
/// @param jsonAtlas Immutable valid json atlas.
/// @param name Key name to get value from.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TJsonParam Automatically bound json atlas type
/// @return First value is returned value and second is Existance flag list.
template <typename TReturnType, typename TJsonParam>
std::pair<TReturnType, FExistanceList> 
GetValueFromOptionally(const TJsonParam& jsonAtlas, const std::string_view& name); 

/// @brief This functions requires additional callback function that checks whether key is exist or not exist.
/// @param jsonAtlas Immutable valid json atlas.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TJsonParam Automatically bound json atlas type
/// @return First value is returned value and second is Existance flag list.
template <typename TReturnType, typename TJsonParam>
std::pair<TReturnType, FExistanceList> 
GetValueOptionally(const TJsonParam& jsonAtlas); 

/// @brief  Get value from json and bind value to oDestination automatically.
/// Destination type must implement copy assignment operator or default behavior.
/// @param  jsonAtlas Immutable valid json atlas like-a types.
/// @param  iKey Header string to find.
/// @param  oDestination Destination value.
/// @tparam TReturnType Type to retrieve from json atlas instance.
/// @tparam TJsonAtlas Json binding type paramter.
template <typename TReturnType, typename TJsonAtlas>
void GetValueFromTo(const TJsonAtlas& jsonAtlas, const std::string& iKey, TReturnType& oDestination);

} /// ::ray::json namespace
#include <Inline/XHelperJson.inl>