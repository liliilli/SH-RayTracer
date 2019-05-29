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

namespace ray::json
{

template <typename TReturnType, typename TParam1>
TReturnType GetValueFrom(const TParam1& jsonAtlas, const char* name)
{
  return jsonAtlas.at(name).template get<TReturnType>();
}

template <typename TReturnType, typename TParam1>
TReturnType GetValueFrom(const TParam1& jsonAtlas, const std::string_view& name)
{
  return json::GetValueFrom<TParam1>(jsonAtlas, (name));
}

template <typename TReturnType, typename TJsonParam>
TReturnType GetValue(const TJsonParam& jsonAtlas)
{
  return jsonAtlas.template get<TReturnType>();
}

template <typename TReturnType, typename TJsonAtlas>
void GetValueFromTo(const TJsonAtlas& jsonAtlas, const std::string& iKey, TReturnType& destination)
{
  if constexpr (std::is_move_assignable_v<TReturnType> == true)
  {
    destination = std::move(jsonAtlas.at(iKey).template get<TReturnType>());
  }
  else
  {
    destination = jsonAtlas.at(iKey).template get<TReturnType>();
  }
}

} /// ::ray::json namespace