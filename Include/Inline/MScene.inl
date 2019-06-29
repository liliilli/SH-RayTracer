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

template <typename TType, typename... TArgs>
void MScene::AddHitableObject(TArgs&&... args)
{
  static_assert(
    std::is_base_of_v<IHitable, TType> == true,
    "TTYpe Must be Hitable object.");

  std::unique_ptr<IHitable> obj = std::make_unique<TType>(std::forward<TArgs>(args)...);
  this->mObjects.emplace_back(obj.release());
}

} /// ::ray namespace