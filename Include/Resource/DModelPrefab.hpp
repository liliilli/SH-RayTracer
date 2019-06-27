#pragma once
///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <XCommon.hpp>

namespace ray
{

/// @class DModelPrefab
/// @brief Model prefab (not populated yet) type.
class DModelPrefab final
{
public:
  std::filesystem::path mModelPath;
  TReal   mScale = 0;
  bool    mIsNormalized = false;
};

/// @brief Template function for automatic parsing from json.
void from_json(const nlohmann::json& json, DModelPrefab& oCtor);

} /// ::ray namespace