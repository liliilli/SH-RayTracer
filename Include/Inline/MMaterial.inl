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

namespace ray
{

template <typename TType>
std::optional<std::string> MMaterial::AddOldMaterial(const nlohmann::json& json)
{
  if constexpr(std::is_same_v<TType, FMatLambertian> == true)
  {
    return this->AddOldMaterial(json, std::declval<FMatLambertian>());
  }
  else if constexpr (std::is_same_v<TType, FMatMetal> == true)
  {
    return this->AddOldMaterial(json, std::declval<FMatMetal>());
  }
  else if constexpr (std::is_same_v<TType, FMatDielectric> == true)
  {
    return this->AddOldMaterial(json, std::declval<FMatDielectric>());
  }

  return std::nullopt;
}

} /// ::ray namespace

