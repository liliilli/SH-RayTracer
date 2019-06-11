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
std::optional<DMatId> MMaterial::AddOldMaterial(const nlohmann::json& json)
{
  if constexpr(std::is_same_v<TType, FMatLambertian> == true)
  {
    return this->AddOldMaterial_FMatLambertian(json);
  }
  else if constexpr (std::is_same_v<TType, FMatMetal> == true)
  {
    return this->AddOldMaterial_FMatMetal(json);
  }
  else if constexpr (std::is_same_v<TType, FMatDielectric> == true)
  {
    return this->AddOldMaterial_FMatDielectric(json);
  }
  else
  {
    return std::nullopt;
  }
}

template <typename TType>
std::optional<DMatId> MMaterial::AddMaterial(const nlohmann::json& json, const std::string& name)
{
  // Check there is duplicated id name in material container.
  if (this->HasMaterial(name) == true)
  {
    std::cerr << "Failed to create material, `" << name << "`. The other material that has duplicated ID is exist.\n";
    return std::nullopt;
  }

  // Compile time code branch.
  if constexpr(std::is_same_v<TType, FMatLambertian> == true)
  {
    return this->AddMaterial_FMatLambertian(json, name);
  }
  else if constexpr (std::is_same_v<TType, FMatMetal> == true)
  {
    return this->AddMaterial_FMatMetal(json, name);
  }
  else if constexpr (std::is_same_v<TType, FMatDielectric> == true)
  {
    return this->AddMaterial_FMatDielectric(json, name);
  }
  else
  {
    return std::nullopt;
  }
}

} /// ::ray namespace

