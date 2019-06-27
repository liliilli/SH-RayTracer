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

#include <Resource/DModelPrefab.hpp>
#include <nlohmann/json.hpp>
#include <XHelperJson.hpp>

namespace ray
{

void from_json(const nlohmann::json& json, DModelPrefab& oCtor)
{
  assert(json::HasJsonKey(json, "path") == true);
  assert(json::HasJsonKey(json, "scale") == true);
  assert(json::HasJsonKey(json, "normalized") == true);

  oCtor.mModelPath    = json::GetValueFrom<std::string>(json, "path");
  oCtor.mScale        = json::GetValueFrom<TReal>(json, "scale");
  oCtor.mIsNormalized = json::GetValueFrom<bool>(json, "normalized");
}

} /// ::ray namespace